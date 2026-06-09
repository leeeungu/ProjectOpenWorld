#include "Item/Object/Fragment/PlayerAnimationDataFragment.h"
#include "Player/Character/BasePlayer.h"
#include "Player/Animation/PlayerAnimInstance.h"

UAnimSequence* UPlayerAnimationSLEDataFragment::GetStartAnim() const
{
    return StartAnim.LoadSynchronous();
}

UAnimSequence* UPlayerAnimationSLEDataFragment::GetLoopAnim() const
{
    return LoopAnim.LoadSynchronous();
}

UAnimSequence* UPlayerAnimationSLEDataFragment::GetEndAnim() const
{
    return EndAnim.LoadSynchronous();
}

bool UPlayerAnimationSLEDataFragment::Equip(const FItemEquipContext& Context) const
{
	ABasePlayer* Player = Cast<ABasePlayer>(Context.User);
	if (!Player)
		return false;
	if (UPlayerAnimInstance* PlayerAnimInstance = Player->GetPlayerAnimInstance())
	{
		if (!PlayerAnimInstance->IsPlayerAnimationSeq())
			return PlayerAnimInstance->SetAnimationSequences(GetStartAnim(), GetLoopAnim(), GetEndAnim());
	}
	return false;
}

bool UPlayerAnimationSLEDataFragment::Unequip(const FItemEquipContext& Context) const
{
	ABasePlayer* Player = Cast<ABasePlayer>(Context.User);
	if (!Player)
		return false;
	if (UPlayerAnimInstance* PlayerAnimInstance = Player->GetPlayerAnimInstance())
	{
		if(!PlayerAnimInstance->IsPlayerAnimationSeq())
			return PlayerAnimInstance->ResetAnimSection();
	}
    return false;
}
