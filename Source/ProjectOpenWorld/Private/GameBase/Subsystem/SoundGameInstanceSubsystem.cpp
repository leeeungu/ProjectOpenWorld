#include "GameBase/Subsystem/SoundGameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Pal/Character/BossMonster.h"
#include "Pal/Character/BasePalMonster.h"

USoundGameInstanceSubsystem* USoundGameInstanceSubsystem::Instance{};

USoundGameInstanceSubsystem::USoundGameInstanceSubsystem() : Super()
{
	//Script/Engine.DataTable'/Game/Pal/DataTable/DT_UISoundData.DT_UISoundData'
	static ConstructorHelpers::FObjectFinder<UDataTable> UISoundDataTableObject(TEXT("DataTable'/Game/Pal/DataTable/DT_UISoundData.DT_UISoundData'"));
	if (UISoundDataTableObject.Succeeded())
	{
		UISoundDataTable = UISoundDataTableObject.Object;
		TArray<FUISoundData*> UISoundDataArray;
		UISoundDataTable->GetAllRows<FUISoundData>(TEXT(""), UISoundDataArray);
		UISoundDataMap.Empty();
		for (const FUISoundData* Data : UISoundDataArray)
		{
			UISoundDataMap.Add(Data->SoundType, Data);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load UISoundDataTable."));
	}

	//Script/Engine.DataTable'/Game/Pal/DataTable/DT_BGMSoundData.DT_BGMSoundData'
	static ConstructorHelpers::FObjectFinder<UDataTable> BGMSoundDataTableObject(TEXT("DataTable'/Game/Pal/DataTable/DT_BGMSoundData.DT_BGMSoundData'"));
	if (BGMSoundDataTableObject.Succeeded())
	{
		BGMSoundDataTable = BGMSoundDataTableObject.Object;
		TArray<FBGMSoundData*> BGMSoundDataArray;
		BGMSoundDataTable->GetAllRows<FBGMSoundData>(TEXT(""), BGMSoundDataArray);
		BGMSoundDataMap.Empty();
		for (const FBGMSoundData* Data : BGMSoundDataArray)
		{
			BGMSoundDataMap.Add(Data->SoundType, Data);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load BGMSoundDataTable."));
	}

	///Script/Engine.DataTable'/Game/Pal/DataTable/DT_EffectSoundData.DT_EffectSoundData'
	static ConstructorHelpers::FObjectFinder<UDataTable> EffectSoundDataTableObject(TEXT("DataTable'/Game/Pal/DataTable/DT_EffectSoundData.DT_EffectSoundData'"));
	if (EffectSoundDataTableObject.Succeeded())
	{
		EffectSoundDataTable = EffectSoundDataTableObject.Object;
		TArray<FEffectSoundData*> EffectSoundDataArray;
		EffectSoundDataTable->GetAllRows<FEffectSoundData>(TEXT(""), EffectSoundDataArray);
		EffectSoundDataMap.Empty();
		for (const FEffectSoundData* Data : EffectSoundDataArray)
		{
			EffectSoundDataMap.Add(Data->SoundType, Data);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load EffectSoundDataTable."));
	}
}

void USoundGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Instance = this;
}

void USoundGameInstanceSubsystem::PlayUISound(EUISoundType SoundType)
{
	if (!Instance)
		return;
	Instance->PlaySound2D(Instance->GetUISound(SoundType));
}

void USoundGameInstanceSubsystem::PlayEffectSound(EEffectSoundType SoundType, FVector Location)
{
	if (!Instance)
		return;
	Instance->PlaySoundAtLocation(Instance->GetEffectSound(SoundType), Location);
}

void USoundGameInstanceSubsystem::PlayBGMSound(EBGMSoundType SoundType, float FadeInDuration)
{
	if (!Instance)
		return;
	UAudioComponent*& StoredAudioComponent = Instance->BGMAudioComponentMap.FindOrAdd(SoundType, nullptr);
	if (StoredAudioComponent == nullptr)
	{
		StoredAudioComponent = UGameplayStatics::SpawnSound2D(Instance->GetWorld(), Instance->GetBGMSound(SoundType));
		StoredAudioComponent->FadeIn(FadeInDuration, 1);
	}
	else
	{
		StoredAudioComponent->SetSound(Instance->GetBGMSound(SoundType));
		StoredAudioComponent->FadeIn(FadeInDuration	, 1);
	}
}

void USoundGameInstanceSubsystem::PauseBGMSound(EBGMSoundType SoundType)
{
	if (!Instance)
		return;
	UAudioComponent*& StoredAudioComponent = Instance->BGMAudioComponentMap.FindOrAdd(SoundType, nullptr);
	if (StoredAudioComponent != nullptr)
	{
		StoredAudioComponent->FadeOut(5.0f,0);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Sound is null, cannot stop BGM sound."));
	}
}

void USoundGameInstanceSubsystem::PlayMainBGMSound(EBGMSoundType SoundType, float FadeInDuration)
{
	if (!Instance)
		return;
	if (Instance->CurrentMainBGM != SoundType)
	{
		if (Instance->CurrentMainBGM != EBGMSoundType::BGMST_None)
		{
			PauseBGMSound(Instance->CurrentMainBGM);
			Instance->CurrentMainBGM = SoundType;
		}
		PlayBGMSound(SoundType, FadeInDuration);
	}
}

void USoundGameInstanceSubsystem::DamageEventBGMSound(AActor* DamagedActor)
{
	if (!Instance || !DamagedActor)
		return;
	EBGMSoundType SoundTypeToPlay = Instance->CurrentMainBGM;

	ABasePalMonster* PalMonster = Cast<ABasePalMonster>(DamagedActor);
	ABossMonster* BossMonster = Cast<ABossMonster>(DamagedActor);
	if (BossMonster)
	{
		SoundTypeToPlay = EBGMSoundType::BGMST_BossTower;
	}
	else if (PalMonster)
	{
		SoundTypeToPlay = EBGMSoundType::BGMST_Battle;
	}
	if (Instance->DamagedActorForBGM)
	{
		Instance->DamagedActorForBGM->OnDestroyed.RemoveDynamic(Instance, &USoundGameInstanceSubsystem::OnMonsterDead);
	}
	if (DamagedActor)
	{
		Instance->DamagedActorForBGM = DamagedActor;
		DamagedActor->OnDestroyed.AddUniqueDynamic(Instance, &USoundGameInstanceSubsystem::OnMonsterDead);
	}
	PlayMainBGMSound(SoundTypeToPlay);
}


void USoundGameInstanceSubsystem::OnMonsterDead(AActor* DestroyedActor)
{
	if (!Instance)
		return;
	if (DestroyedActor == Instance->DamagedActorForBGM)
	{
		Instance->DamagedActorForBGM = nullptr;
		PlayMainBGMSound(EBGMSoundType::BGMST_InGame);
	}
}

USoundBase* USoundGameInstanceSubsystem::GetUISound(EUISoundType SoundType)
{
	const FUISoundData** FoundData = Instance->UISoundDataMap.Find(SoundType);
	if (FoundData &&& *FoundData)
	{
		return (*FoundData)->Sound;
	}
	return nullptr;
}

USoundBase* USoundGameInstanceSubsystem::GetEffectSound(EEffectSoundType SoundType)
{
	const FEffectSoundData** FoundData = Instance->EffectSoundDataMap.Find(SoundType);
	if (FoundData && *FoundData)
	{
		return (*FoundData)->Sound;
	}
	return nullptr;
}

USoundBase* USoundGameInstanceSubsystem::GetBGMSound(EBGMSoundType SoundType)
{
	const FBGMSoundData** FoundData = Instance->BGMSoundDataMap.Find(SoundType);
	if (FoundData && *FoundData)
	{
		return (*FoundData)->Sound;
	}
	return nullptr;
}

void USoundGameInstanceSubsystem::PlaySound2D(USoundBase* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(Instance->GetWorld(), Sound);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Sound is null, cannot play sound."));
	}
}

void USoundGameInstanceSubsystem::PlaySoundAtLocation(USoundBase* Sound, FVector Location)
{
	if(Sound)
		UGameplayStatics::PlaySoundAtLocation(Instance->GetWorld(), Sound, Location);
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Sound is null, cannot play sound at location."));
	}
}
