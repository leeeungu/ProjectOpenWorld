#include "Building/Component/BuildingActionWidgetComponent.h"
#include "Building/Widget/BaseBuildingAction.h"

UBuildingActionWidgetComponent::UBuildingActionWidgetComponent() : 
	UWidgetComponent{}
{
	//Script/UMGEditor.WidgetBlueprint'/Game/Building/Widget/BuildingAction/WorkBench/WBP_WorkingBench.WBP_WorkingBench'
	//ConstructorHelpers::FClassFinder<UUserWidget> ActionClass(TEXT("/Game/Building/Widget/BuildingAction/WorkBench/WBP_WorkingBench.WBP_WorkingBench_C"));
	//if (ActionClass.Succeeded())
	//{
	//	SetWidgetClass(ActionClass.Class);
	//}
	SetVisibility(false);
}
