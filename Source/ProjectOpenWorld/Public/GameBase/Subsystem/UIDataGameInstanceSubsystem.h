#pragma once

#include "CoreMinimal.h"
#include "GameBase/Subsystem/SoundGameInstanceSubsystem.h"
#include "UIDataGameInstanceSubsystem.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UUIDataGameInstanceSubsystem : public USoundGameInstanceSubsystem
{
	GENERATED_BODY()

public:
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
};
