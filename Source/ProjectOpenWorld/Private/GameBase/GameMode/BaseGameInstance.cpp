#include "GameBase/GameMode/BaseGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "Pal/Interface/PalSaveGameObject.h"

TObjectPtr<UBaseGameInstance> UBaseGameInstance::Instance = nullptr;

UBaseGameInstance::UBaseGameInstance()
{
	Instance = this;
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
	Instance->TargetLevel = newLevel;
	if (Instance->LoadingLevel)
		UGameplayStatics::OpenLevelBySoftObjectPtr(World, Instance->LoadingLevel);
	LoadLevelStatic(pWorldContext);
	//UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(World, World->GetCurrentLevel(), FLatentActionInfo{}, false);
}

void UBaseGameInstance::SetTargetLevel(TSoftObjectPtr<UWorld> newLevel)
{
	if (!Instance)
		return;
	Instance->TargetLevel = newLevel;
}

void UBaseGameInstance::OpenLoadedLevel()
{
	//UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(GetWorld(), GetWorld()->GetCurrentLevel(), FLatentActionInfo{}, false);
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), TargetLevel);
	TargetLevel = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("OpenLoadedLevel %d"), Count);
	Count++;
}

void UBaseGameInstance::ProgressFunction()
{
	UE_LOG(LogTemp, Warning, TEXT("Load %d"), Count);
	Count++;
}

void UBaseGameInstance::PalSaveTest(UObject* pWorldContext, const FString SlotName)
{
	if (!Instance || !pWorldContext)
		return;
	UWorld* World = pWorldContext->GetWorld();

	UPalSaveGame* Save = Cast<UPalSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UPalSaveGame::StaticClass()));
	Save->LevelName = World->GetMapName();

	//TArray<AActor*> Savables;
	//if (UWorld* World = GetWorld())
	//{
	//	for (TActorIterator<AActor> It(World); It; ++It)
	//		Savables.Add(*It);
	//}
	//TArray<uint8> ByteData{};
	//for (AActor* Actor : Savables)
	//{
	//	SerializeActor(Actor, ByteData);
	//}

	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0)) // PlayerState로 바꿔도 동일
	{
		Instance->SerializeActor(Pawn, Save->PlayerByteData);
		Instance->SerializeActor(Pawn->GetController(), Save->PlayerByteData);
	}
	const bool bSaved = UGameplayStatics::SaveGameToSlot(Save, SlotName, Instance->UserIndex);

	UPalSaveGame* Loaded = Cast<UPalSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SlotName, Instance->UserIndex));

	UE_LOG(LogTemp, Warning, TEXT("[SaveTest] saved=%d value=%s"),bSaved, Loaded ? *Loaded->LevelName : TEXT("<null>"));
}

void UBaseGameInstance::SerializeActor(AActor* Actor, TArray<uint8>& OutBytes)
{
	//if (IPalSaveGameObject* S = Cast<IPalSaveGameObject>(Actor))
	//	S->OnPreSave();
	if (!Actor)
		return;
	FMemoryWriter Writer(OutBytes);
	FObjectAndNameAsStringProxyArchive Ar(Writer, true);
	Ar.ArIsSaveGame = true;

	Actor->Serialize(Ar);                       // 액터 자신의 SaveGame 필드
	// 컴포넌트(인벤토리 등)의 SaveGame 필드 — 저장/로드 순서 일치를 위해 이름순 정렬
	TArray<UActorComponent*> Comps;
	Actor->GetComponents(Comps);
	Comps.Sort([](const UActorComponent& A, const UActorComponent& B) { return A.GetName() < B.GetName(); });
	for (UActorComponent* Comp : Comps)
	{
		if (IPalSaveGameObject* Obj = Cast<IPalSaveGameObject>(Comp))
		{
			Obj->OnPreSave();
		}
		if (Comp && Comp->Implements<UPalSaveGameObject>())
		{
			Comp->Serialize(Ar);
		}
	}
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

	// 기존 동적 액터 제거 후 레코드에서 재생성
	/*TArray<AActor*> Existing;
	GatherSavables(Existing);
	for (AActor* Actor : Existing) Actor->Destroy();

	for (const FPalActorSaveData& Data : Save->SavedActors)
	{
		UClass* Cls = Data.ActorClass.TryLoadClass<AActor>();
		if (!Cls) continue;
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (AActor* Spawned = World->SpawnActor<AActor>(Cls, Data.Transform, Params))
			DeserializeActor(Spawned, Data.ByteData);
	}*/

	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0))
	{
		Instance->DeserializeActor(Pawn, Save->PlayerByteData);
		Instance->DeserializeActor(Pawn->GetController(), Save->PlayerByteData);
	}

	return true;
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
			Comp->Serialize(Ar);
		}
		if (IPalSaveGameObject* Obj = Cast<IPalSaveGameObject>(Comp))
		{
			Obj->OnLoaded();
		}
	}

	//if (IPalSaveGameObject* S = Cast<IPalSaveGameObject>(Actor))
	//	S->OnLoaded();
}