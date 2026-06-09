#include "Player/Widget/PlayerStatusWidget.h"
#include "Player/Widget/PlayerStatusProgress.h"
#include "Player/Widget/PlayerStatusSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UPlayerStatusWidget::SetStatWidget(UStatComponent* StatCom)
{
	if (ProgressHp)
	{
		ProgressHp->SetStatWidget(StatCom);
	}
	if (ProgressHealth)
	{
		ProgressHealth->SetStatWidget(StatCom);
	}

	TArray<UWidget*> Array = StatusView->GetAllChildren();
	if (!SlotArray.IsEmpty() && Array.Num() == SlotArray.Num())
		return;
	SlotArray.Empty();
	SlotArray.Reserve(Array.Num());
	for (UWidget* Widget : Array)
	{
		if (UPlayerStatusSlot* StatSlot = Cast< UPlayerStatusSlot>(Widget))
		{
			SlotArray.Add(StatSlot);
			StatSlot->SetStatWidget(StatCom);
		}
	}
}

void UPlayerStatusWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UPlayerStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}
