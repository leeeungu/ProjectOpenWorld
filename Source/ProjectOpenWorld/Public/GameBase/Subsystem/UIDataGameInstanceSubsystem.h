#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIDataGameInstanceSubsystem.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UUIDataGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
protected:
	static UUIDataGameInstanceSubsystem* Instance;
	UPROPERTY()
	TObjectPtr<USoundBase> ButtonClickSound{};
	UPROPERTY()
	TObjectPtr<USoundBase> UIOpen{};
	UPROPERTY()
	TObjectPtr<USoundBase> UIClose{};
	UPROPERTY()
	TObjectPtr<USoundBase> UIHover{};
	UPROPERTY()
	TObjectPtr<USoundBase> UITabChange{};
	UPROPERTY()
	TObjectPtr<USoundBase> UIBuildPress{};
	UPROPERTY()
	TObjectPtr<USoundBase> UIBuildHover{};
	UPROPERTY()
	TObjectPtr<USoundBase> UIBuildCancel{};
	UPROPERTY()
	TObjectPtr<USoundBase> UIBuildComplete{};

public:
	UUIDataGameInstanceSubsystem();


	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UFUNCTION(BlueprintCallable, Category = "UIDataGameInstanceSubsystem")
	static void PlayButtonClickSound();
	UFUNCTION(BlueprintCallable, Category = "UIDataGameInstanceSubsystem")
	static void  PlayUIOpenSound();
	UFUNCTION(BlueprintCallable , Category = "UIDataGameInstanceSubsystem")
	static void PlayUICloseSound();
	UFUNCTION(BlueprintCallable , Category = "UIDataGameInstanceSubsystem")
	static void PlayUIHoverSound();
	UFUNCTION(BlueprintCallable , Category = "UIDataGameInstanceSubsystem")
	static void PlayUITabChangeSound();
	UFUNCTION(BlueprintCallable, Category = "UIDataGameInstanceSubsystem")
	static void PlayUIBuildPressSound();
	UFUNCTION(BlueprintCallable, Category = "UIDataGameInstanceSubsystem")
	static void PlayUIBuildHoverSound();
	UFUNCTION(BlueprintCallable, Category = "UIDataGameInstanceSubsystem")
	static void PlayUIBuildCancelSound();
	UFUNCTION(BlueprintCallable, Category = "UIDataGameInstanceSubsystem")
	static void PlayUIBuildCompleteSound();

};
