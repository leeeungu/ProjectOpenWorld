#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SoundGameInstanceSubsystem.generated.h"

class USoundBase;
class UAudioComponent;
class UDataTable;
class AActor;

UENUM(BlueprintType)
enum EUISoundType : uint8
{
	UIST_None UMETA(DisplayName = "None"),
	UIST_ButtonClick UMETA(DisplayName = "ButtonClick"),
	UIST_UIOpen UMETA(DisplayName = "UIOpen"),
	UIST_UIClose UMETA(DisplayName = "UIClose"),
	UIST_UIHover UMETA(DisplayName = "UIHover"),
	UIST_UITabChange UMETA(DisplayName = "UITabChange"),
	UIST_UIBuildPress UMETA(DisplayName = "UIBuildPress"),
	UIST_UIBuildHover UMETA(DisplayName = "UIBuildHover"),
	UIST_UIBuildCancel UMETA(DisplayName = "UIBuildCancel"),
	UIST_UIBuildComplete UMETA(DisplayName = "UIBuildComplete"),
};

USTRUCT(BlueprintType)
struct FUISoundData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundData")
	TEnumAsByte<EUISoundType> SoundType{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundData")
	TObjectPtr<USoundBase> Sound{};
};


UENUM(BlueprintType)
enum EBGMSoundType : uint8
{
	BGMST_None UMETA(DisplayName = "None"),
	BGMST_MainMenu UMETA(DisplayName = "MainMenu"),
	BGMST_InGame UMETA(DisplayName = "InGame"),
	BGMST_Battle UMETA(DisplayName = "Battle"),
	BGMST_BossTower UMETA(DisplayName = "BossTower"),
};

USTRUCT(BlueprintType)
struct FBGMSoundData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundData")
	TEnumAsByte<EBGMSoundType> SoundType{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundData")
	TObjectPtr<USoundBase> Sound{};
};

UENUM(BlueprintType)
enum EEffectSoundType : uint8
{
	EST_None UMETA(DisplayName = "None"),
	EST_PinkUpGrass UMETA(DisplayName = "PickUpGrass"),
	EST_PickUpItem UMETA(DisplayName = "PickUpItem"),
	EST_PinkUpStone UMETA(DisplayName = "PickUpStone"),
	EST_PinkUpWood UMETA(DisplayName = "PickUpWood"),
	EST_BuildingComplete UMETA(DisplayName = "BuildingComplete"),
	EST_CutTree UMETA(DisplayName = "CutTree"),
	EST_BreakRock UMETA(DisplayName = "BreakRock"),

};

USTRUCT(BlueprintType)
struct FEffectSoundData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundData")
	TEnumAsByte<EEffectSoundType> SoundType{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundData")
	TObjectPtr<USoundBase> Sound{};
};

UCLASS()
class PROJECTOPENWORLD_API USoundGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	static USoundGameInstanceSubsystem* Instance;
	TMap< TEnumAsByte<EUISoundType>, const FUISoundData*> UISoundDataMap{};
	TMap< TEnumAsByte<EBGMSoundType>, const FBGMSoundData*> BGMSoundDataMap{};
	TMap< TEnumAsByte<EEffectSoundType>, const FEffectSoundData*> EffectSoundDataMap{};
	UPROPERTY()
	TMap < TEnumAsByte<EBGMSoundType>, UAudioComponent*> BGMAudioComponentMap{}; 
	UPROPERTY()
	TObjectPtr<UDataTable> UISoundDataTable{};
	UPROPERTY()
	TObjectPtr<UDataTable> BGMSoundDataTable{};
	UPROPERTY()
	TObjectPtr<UDataTable> EffectSoundDataTable{};

	EBGMSoundType CurrentMainBGM{ EBGMSoundType::BGMST_None };

	UPROPERTY()
	TObjectPtr<AActor> DamagedActorForBGM{};
public:
	USoundGameInstanceSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UFUNCTION(BlueprintCallable, Category = "SoundGameInstanceSubsystem")
	static void PlayUISound(EUISoundType SoundType);
	UFUNCTION(BlueprintCallable, Category = "SoundGameInstanceSubsystem")
	static void PlayEffectSound(EEffectSoundType SoundType, FVector Location);
	UFUNCTION(BlueprintCallable, Category = "SoundGameInstanceSubsystem")
	static void PlayBGMSound(EBGMSoundType SoundType, float FadeInDuration = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "SoundGameInstanceSubsystem")
	static void PauseBGMSound(EBGMSoundType SoundType);

	UFUNCTION(BlueprintCallable, Category = "SoundGameInstanceSubsystem")
	static void PlayMainBGMSound(EBGMSoundType SoundType, float FadeInDuration = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "SoundGameInstanceSubsystem")
	static void DamageEventBGMSound(AActor* DamagedActor);
private:

	UFUNCTION()
	void OnMonsterDead(AActor* DestroyedActor);

	USoundBase* GetUISound(EUISoundType SoundType);
	USoundBase* GetEffectSound(EEffectSoundType SoundType);
	USoundBase* GetBGMSound(EBGMSoundType SoundType);

	void PlaySound2D(USoundBase* Sound);
	void PlaySoundAtLocation(USoundBase* Sound, FVector Location);
};
