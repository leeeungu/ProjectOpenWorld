#include "Building/Subsystem/BuildingWidgetSubsystem.h"
#include "Building/Widget/BuildingStateWidget.h"
#include "Building/Component/BuildingProgress.h"


DEFINE_LOG_CATEGORY(LogBuildingSubsystem);

UBuildingWidgetSubsystem::UBuildingWidgetSubsystem() : ULocalPlayerSubsystem{}
{
	//Script/UMGEditor.WidgetBlueprint'/Game/Building/Widget/WBP_BuildState.WBP_BuildState'
	static ConstructorHelpers::FClassFinder<UBuildingStateWidget> widgetClass(TEXT("/Game/Building/Widget/WBP_BuildTimer.WBP_BuildTimer_C"));
	if (widgetClass.Succeeded())
	{
		buildingStateWidgetClass = widgetClass.Class;
	}
}

void UBuildingWidgetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	ULocalPlayerSubsystem::Initialize(Collection);
	if (!buildingStateWidget && buildingStateWidgetClass)
	{
		//UE_LOG(LogBuildingSubsystem, Log, TEXT("Create BuildingStateWidget"));
		buildingStateWidget = Cast< UBuildingStateWidget>(CreateWidget(GetWorld(), buildingStateWidgetClass));
	}
}

void UBuildingWidgetSubsystem::Deinitialize()
{
	ULocalPlayerSubsystem::Deinitialize();
}

void UBuildingWidgetSubsystem::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	ULocalPlayerSubsystem::PlayerControllerChanged(NewPlayerController);
	if (!buildingStateWidget && buildingStateWidgetClass)
	{
		buildingStateWidget = Cast< UBuildingStateWidget>(CreateWidget(NewPlayerController, buildingStateWidgetClass));
	}
}

void UBuildingWidgetSubsystem::SetBuildingWidgetProperty(UBuildingProgress* ProgressComponent)
{
	if (buildingStateWidget && ProgressComponent)
	{
		buildingStateWidget->SetBuildPercent(ProgressComponent->GetBuildPercent());
		buildingStateWidget->SetBuildTime(ProgressComponent->GetBuildTime());
		buildingStateWidget->SetBuildSpeed(ProgressComponent->GetBuildSpeed());
		if (buildProgress)
		{
			buildProgress->onBuildingEnd.RemoveDynamic(buildingStateWidget, &UBuildingStateWidget::OnBuildingEnd);
		}
		buildProgress = ProgressComponent;
		ProgressComponent->onBuildingEnd.AddUniqueDynamic(buildingStateWidget, &UBuildingStateWidget::OnBuildingEnd);
	}
}

void UBuildingWidgetSubsystem::AddBuildTimeWidget()
{
	if (buildingStateWidget && !buildingStateWidget->IsInViewport())
	{
		buildingStateWidget->AddToViewport();
	}
}

void UBuildingWidgetSubsystem::RemoveBuildTimeWidget()
{
	if (buildingStateWidget)
	{
		buildingStateWidget->RemoveFromParent();
	}
}
