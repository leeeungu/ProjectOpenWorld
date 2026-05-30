#include "Player/Widget/PlayerStatusWidget.h"
#include "Player/Widget/PlayerStatusProgress.h"
#include "Player/Widget/PlayerStatusSlot.h"
#include "Player/Character/BasePlayer.h"
#include "GameFramework/PlayerController.h"

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
}

void UPlayerStatusWidget::SetStatusSlot(EStatusSlotType StatusType, float& Value)
{
	TObjectPtr<UPlayerStatusSlot>* arSlot[SlotMax] =
	{
		nullptr,
		&SlotMaxHp,
		&SlotStamina,
		&SlotAttack,
		&SlotDefense,
		&SlotWorkSpeed,
		&SlotMaxWeight,
	};
	if (arSlot[StatusType] && *arSlot[StatusType])
	{
		(*arSlot[StatusType])->SetStatusSlot(&Value);
		(*arSlot[StatusType])->UpdateStatus();
	}
}

void UPlayerStatusWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();
}
