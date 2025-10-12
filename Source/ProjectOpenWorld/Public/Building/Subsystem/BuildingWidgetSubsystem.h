#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "BuildingWidgetSubsystem.generated.h"

class UBuildingStateWidget;
class UBuildingProgress;

DECLARE_LOG_CATEGORY_EXTERN(LogBuildingSubsystem, Log, All);

UCLASS()
class PROJECTOPENWORLD_API UBuildingWidgetSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
protected:

	UPROPERTY()
	UBuildingStateWidget* buildingStateWidget{};
	UPROPERTY()
	TSubclassOf<UBuildingStateWidget> buildingStateWidgetClass{};
public:
	UBuildingWidgetSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void PlayerControllerChanged(APlayerController* NewPlayerController) override;

	/// BuildingTimeWidget
	void SetBuildingWidgetProperty(UBuildingProgress* ProgressComponent);
	void AddBuildTimeWidget();
	void RemoveBuildTimeWidget();
};

