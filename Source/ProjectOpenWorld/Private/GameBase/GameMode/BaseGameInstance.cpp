#include "GameBase/GameMode/BaseGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "Pal/Interface/PalSaveGameObject.h"

TObjectPtr<UBaseGameInstance> UBaseGameInstance::Instance = nullptr;
const FString UBaseGameInstance::PlayerSlotName = TEXT("PlayerSave");

UBaseGameInstance::UBaseGameInstance()
{
	Instance = this;
}

void UBaseGameInstance::Init()
{
	Super::Init();
	// 맵 로드 완료 시점 훅(복원용) — 모든 전환 경로가 자동으로 탐
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UBaseGameInstance::OnPostLoadMap);
	if (UWorld* World = GetWorld())
		OnPostLoadMap(World);
}

void UBaseGameInstance::Shutdown()
{
	if (UWorld* World = GetWorld())
		SaveCurrentWorld(World);

	if (PostLoadMapHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
		PostLoadMapHandle.Reset();
	}
	Super::Shutdown();
}

void UBaseGameInstance::LoadLevelStatic(UObject* pWorldContext)
{
	if (!pWorldContext || !pWorldContext->GetWorld())
		return;
	UWorld* World = pWorldContext->GetWorld();
	if (!Instance || Instance->TargetLevel.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetLevel is Nullptr"));
		UKismetSystemLibrary::QuitGame(World, nullptr, EQuitPreference::Type::Quit, false);
		return;
	}

	Instance->Count = 0;
	FLoadPackageAsyncDelegate LoadedDelegate{};
	LoadedDelegate.BindUFunction(Instance, FName(TEXT("OpenLoadedLevel")));
	LoadPackageAsync(FPackageName::ObjectPathToPackageName(Instance->TargetLevel.ToString()), MoveTemp(LoadedDelegate));
}

void UBaseGameInstance::OpenLevelStatic(UObject* pWorldContext, TSoftObjectPtr<UWorld> newLevel)
{
	if (newLevel.IsNull() || !pWorldContext || !pWorldContext->GetWorld())
		return;
	UWorld* World = pWorldContext->GetWorld();
	if (!Instance)
		return;

	// 맵 이동 직전 — 레벨 트래블은 프레임 끝에 처리되므로 현재 월드(빌딩)는 아직 유효
	Instance->SaveCurrentWorld(World);

	Instance->TargetLevel = newLevel;
	if (Instance->LoadingLevel)
		UGameplayStatics::OpenLevelBySoftObjectPtr(World, Instance->LoadingLevel);
	LoadLevelStatic(pWorldContext);
}

void UBaseGameInstance::SetTargetLevel(TSoftObjectPtr<UWorld> newLevel)
{
	if (!Instance)
		return;
	Instance->TargetLevel = newLevel;
}

void UBaseGameInstance::OpenLoadedLevel()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), TargetLevel, true);
	TargetLevel = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("OpenLoadedLevel %d"), Count);
	Count++;
}

void UBaseGameInstance::ProgressFunction()
{
	UE_LOG(LogTemp, Warning, TEXT("Load %d"), Count);
	Count++;
}

FString UBaseGameInstance::GetShortMapName(UWorld* World)
{
	if (!World)
		return FString();
	FString MapName = World->GetMapName();
	MapName.RemoveFromStart(World->StreamingLevelsPrefix); // PIE 프리픽스 제거
	return MapName;
}

FString UBaseGameInstance::MakeLevelSlotName(UWorld* World)
{
	return FString::Printf(TEXT("Level_%s"), *GetShortMapName(World));
}

bool UBaseGameInstance::IsBuildingLevel(UWorld* World) const
{
	if (!World)
		return false;
	const FString CurrentMap = GetShortMapName(World);
	for (const TSoftObjectPtr<UWorld>& Lv : BuildingLevels)
	{
		if (Lv.IsNull())
			continue;
		const FString LvShort = FPackageName::GetShortName(FPackageName::ObjectPathToPackageName(Lv.ToString()));
		if (LvShort == CurrentMap)
			return true;
	}
	return false;
}

void UBaseGameInstance::ClearSaves()
{
	if (!Instance)
		return;

	if (UGameplayStatics::DoesSaveGameExist(PlayerSlotName, Instance->UserIndex))
		UGameplayStatics::DeleteGameInSlot(PlayerSlotName, Instance->UserIndex);

	for (const TSoftObjectPtr<UWorld>& Lv : Instance->BuildingLevels)
	{
		if (Lv.IsNull())
			continue;
		const FString Slot = FString::Printf(TEXT("Level_%s"),
			*FPackageName::GetShortName(FPackageName::ObjectPathToPackageName(Lv.ToString())));
		if (UGameplayStatics::DoesSaveGameExist(Slot, Instance->UserIndex))
			UGameplayStatics::DeleteGameInSlot(Slot, Instance->UserIndex);
	}
	UE_LOG(LogTemp, Warning, TEXT("[Save] ClearSaves done"));
}

void UBaseGameInstance::SaveWorld()
{
	if (Instance)
	{
		Instance->SaveCurrentWorld(Instance->GetWorld());
	}
}

void UBaseGameInstance::SaveCurrentWorld(UWorld* World)
{
	if (!World)
		return;

	// 1) 플레이어 — 레벨 간 유지되므로 고정 슬롯(항상 저장)
	/*{
		UPalSaveGame* PlayerSave = Cast<UPalSaveGame>(UGameplayStatics::CreateSaveGameObject(UPalSaveGame::StaticClass()));
		if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0))
		{
			SerializeActor(Pawn, PlayerSave->PlayerByteData);
		}
		UGameplayStatics::SaveGameToSlot(PlayerSave, PlayerSlotName, UserIndex);
	}*/

	// 2) 빌딩 — 허용 레벨에서만 (보스 맵 등은 스킵)
	if (!IsBuildingLevel(World))
		return;

	UPalSaveGame* LevelSave = Cast<UPalSaveGame>(UGameplayStatics::CreateSaveGameObject(UPalSaveGame::StaticClass()));
	LevelSave->LevelName = World->GetMapName();

	TArray<AActor*> Existing;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (IsValid(*It) && It->Implements<UPalSaveGameObject>())
		{
			FPalActorSaveData Record;
			Record.ActorClass = FSoftClassPath(It->GetClass());
			Record.Transform = It->GetActorTransform();
			SerializeActor(*It, Record.ByteData); // 액터 OnPreSave + SaveGame 필드
			LevelSave->SavedActors.Add(Record);
		}
	}
	UGameplayStatics::SaveGameToSlot(LevelSave, MakeLevelSlotName(World), UserIndex);
	UE_LOG(LogTemp, Warning, TEXT("[Save] %s : Buildings=%d"), *LevelSave->LevelName, LevelSave->SavedActors.Num());
}

void UBaseGameInstance::OnPostLoadMap(UWorld* LoadedWorld)
{
	if (!LoadedWorld)
		return;

	// 로딩 스크린 레벨이면 복원하지 않음
	if (!LoadingLevel.IsNull())
	{
		const FString LoadingName = FPackageName::ObjectPathToPackageName(LoadingLevel.ToString());
		if (LoadedWorld->GetOutermost() && LoadedWorld->GetOutermost()->GetName() == LoadingName)
			return;
	}

	// 폰/액터가 완전히 초기화된 다음 틱에 복원
	const FString LevelSlot = MakeLevelSlotName(LoadedWorld);
	TWeakObjectPtr<UWorld> WeakWorld = LoadedWorld;
	LoadedWorld->GetTimerManager().SetTimerForNextTick([this, WeakWorld, LevelSlot]()
		{
			if (UWorld* World = WeakWorld.Get())
				ApplyLoad(World, LevelSlot);
		});
}

void UBaseGameInstance::ApplyLoad(UWorld* World, const FString& LevelSlot)
{
	if (!World)
		return;

	// 1) 플레이어 복원(고정 슬롯, 존재할 때만)
	if (UGameplayStatics::DoesSaveGameExist(PlayerSlotName, UserIndex))
	{
		if (UPalSaveGame* PlayerSave = Cast<UPalSaveGame>(UGameplayStatics::LoadGameFromSlot(PlayerSlotName, UserIndex)))
		{
			if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0))
			{
				DeserializeActor(Pawn, PlayerSave->PlayerByteData);
			}
		}
	}

	// 2) 빌딩 복원 — 허용 레벨 + 슬롯 존재할 때만
	if (!IsBuildingLevel(World) || !UGameplayStatics::DoesSaveGameExist(LevelSlot, UserIndex))
		return;

	UPalSaveGame* LevelSave = Cast<UPalSaveGame>(UGameplayStatics::LoadGameFromSlot(LevelSlot, UserIndex));
	if (!LevelSave)
		return;

	// 안전 순회: 먼저 수집 후 파괴(Destroy 콜백 재진입 방지)
	TArray<AActor*> Existing;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (IsValid(*It) && It->Implements<UPalSaveGameObject>())
			Existing.Add(*It);
	}
	for (AActor* Building : Existing)
	{
		if (IsValid(Building))
			Building->Destroy();
	}

	for (const FPalActorSaveData& Record : LevelSave->SavedActors)
	{
		UClass* Cls = Record.ActorClass.TryLoadClass<AActor>();
		if (!Cls)
			continue;

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		if (AActor* Spawned = World->SpawnActor<AActor>(Cls, Record.Transform, Params))
			DeserializeActor(Spawned, Record.ByteData);
	}
	UE_LOG(LogTemp, Warning, TEXT("[Load] %s : Buildings=%d"), *LevelSlot, LevelSave->SavedActors.Num());
}

void UBaseGameInstance::SerializeActor(AActor* Actor, TArray<uint8>& OutBytes)
{
	if (!Actor)
		return;

	// 액터 자신의 OnPreSave (빌딩의 가공 필드 등)
	if (IPalSaveGameObject* ActorObj = Cast<IPalSaveGameObject>(Actor))
		ActorObj->OnPreSave();

	FMemoryWriter Writer(OutBytes);
	FObjectAndNameAsStringProxyArchive Ar(Writer, true);
	Ar.ArIsSaveGame = true;

	Actor->Serialize(Ar);                     
	TArray<UActorComponent*> Comps;
	Actor->GetComponents(Comps);
	UE_LOG(LogTemp, Warning, TEXT("[Load] %s : LoadActorComSize =%d"), *Actor->GetName(), Comps.Num());
	Comps.Sort([](const UActorComponent& A, const UActorComponent& B) { return A.GetName() < B.GetName(); });
	for (UActorComponent* Comp : Comps)
	{
		if (IPalSaveGameObject* Obj = Cast<IPalSaveGameObject>(Comp))
		{
			Obj->OnPreSave();
			Comp->Serialize(Ar);
		}
	}
}

void UBaseGameInstance::DeserializeActor(AActor* Actor, const TArray<uint8>& InBytes)
{
	if (!Actor)
		return;
	FMemoryReader Reader(InBytes);
	FObjectAndNameAsStringProxyArchive Ar(Reader, true);
	Ar.ArIsSaveGame = true;

	Actor->Serialize(Ar);
	TArray<UActorComponent*> Comps;
	Actor->GetComponents(Comps);
	Comps.Sort([](const UActorComponent& A, const UActorComponent& B) { return A.GetName() < B.GetName(); });
	for (UActorComponent* Comp : Comps)
	{
		if (Comp && Comp->Implements<UPalSaveGameObject>())
		{
			IPalSaveGameObject* Obj = Cast<IPalSaveGameObject>(Comp);
			Comp->Serialize(Ar);
			Obj->OnLoaded();
		}
	}

	// 액터 자신의 OnLoaded
	if (IPalSaveGameObject* ActorObj = Cast<IPalSaveGameObject>(Actor))
		ActorObj->OnLoaded();
}

bool UBaseGameInstance::LoadFromSlot(UObject* pWorldContext, const FString& SlotName)
{
	if (!pWorldContext)
		return false;
	UWorld* World = pWorldContext->GetWorld();
	if (!World || !UGameplayStatics::DoesSaveGameExist(SlotName, Instance->UserIndex))
		return false;

	UPalSaveGame* Save = Cast<UPalSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, Instance->UserIndex));
	if (!Save)
		return false;

	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0))
	{
		Instance->DeserializeActor(Pawn, Save->PlayerByteData);
	}
	return true;
}

void UBaseGameInstance::SavePlayer(UObject* pWorldContext)
{
	if (!Instance || !pWorldContext)
		return;
	UPalSaveGame* PlayerSave = Cast<UPalSaveGame>(UGameplayStatics::CreateSaveGameObject(UPalSaveGame::StaticClass()));
	APawn* Pawn = Cast<APawn>(pWorldContext);
	if (Pawn)
	{
		SerializeActor(Pawn, PlayerSave->PlayerByteData);
	}
	else if (UWorld* W = pWorldContext->GetWorld())
	{
		Pawn = UGameplayStatics::GetPlayerPawn(W, 0);
		if (Pawn)
			SerializeActor(Pawn, PlayerSave->PlayerByteData);
	}
	UGameplayStatics::SaveGameToSlot(PlayerSave, PlayerSlotName, Instance->UserIndex);
}

void UBaseGameInstance::PalSaveTest(UObject* pWorldContext, const FString SlotName)
{
	if (!Instance || !pWorldContext)
		return;
	UWorld* World = pWorldContext->GetWorld();

	UPalSaveGame* Save = Cast<UPalSaveGame>(UGameplayStatics::CreateSaveGameObject(UPalSaveGame::StaticClass()));
	Save->LevelName = World->GetMapName();

	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0))
	{
		Instance->SerializeActor(Pawn, Save->PlayerByteData);
	}
	const bool bSaved = UGameplayStatics::SaveGameToSlot(Save, SlotName, Instance->UserIndex);

	UPalSaveGame* Loaded = Cast<UPalSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SlotName, Instance->UserIndex));

	UE_LOG(LogTemp, Warning, TEXT("[SaveTest] saved=%d value=%s"), bSaved, Loaded ? *Loaded->LevelName : TEXT("<null>"));
}