#include "Player/Widget/PlayerStatusWidget.h"
#include "Player/Widget/PlayerStatusProgress.h"
#include "Player/Widget/PlayerStatusSlot.h"
#include "Player/Character/BasePlayer.h"
#include "GameFramework/PlayerController.h"

UPlayerStatusWidget::UPlayerStatusWidget(const FObjectInitializer& ObjectInitializer) :
	UUserWidget(ObjectInitializer)
{
}

void UPlayerStatusWidget::SetStatusProgress(EStatusProgressType StatusType, float& Value, float& MaxValue)
{
	TObjectPtr<UPlayerStatusProgress>* arSlot[ProgressMax] =
	{
		nullptr,
		&ProgressHp,
		&ProgressShield,
		&ProgressHealth,
	};
	if (arSlot[StatusType] && *arSlot[StatusType])
	{
	//	(*arSlot[StatusType])->SetStatusProgress(&Value, &MaxValue);
		(*arSlot[StatusType])->UpdateStatus();
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
	if (!GetOwningPlayer())
		return;
	ABasePlayer* Player =  Cast<ABasePlayer>(GetOwningPlayer()->GetPawn());
	if (!Player)
		return;

	TPair<EStatusSlotType, EStatusType> arSlot[6] =
	{
		{EStatusSlotType::MaxHp, EStatusType::MaxHp},
		{EStatusSlotType::Stamina, EStatusType::Stamina},
		{EStatusSlotType::Attack, EStatusType::Attack},
		{EStatusSlotType::Defense, EStatusType::Defense},
		{EStatusSlotType::WorkSpeed, EStatusType::WorkSpeed},
		{EStatusSlotType::MaxWeight, EStatusType::MaxWeight}
	};
	for (TPair<EStatusSlotType, EStatusType>& Type : arSlot)
	{
		SetStatusSlot(Type.Key, *(Player->GetStatusRef(Type.Value)));
	}

	TTuple<EStatusProgressType, EStatusType, EStatusType > arProgress[3] =
	{
		{EStatusProgressType::Hp, EStatusType::Hp, EStatusType::MaxHp},
		{EStatusProgressType::Shield, EStatusType::Shield, EStatusType::MaxShield},
		{EStatusProgressType::Health, EStatusType::Health, EStatusType::MaxHealth}
	};
	for (TTuple<EStatusProgressType, EStatusType, EStatusType >& Type : arProgress)
	{
		SetStatusProgress(Type.Get<0>(), *(Player->GetStatusRef(Type.Get<1>())), *(Player->GetStatusRef(Type.Get<2>())));
	}
}
