#include "Item/Object/Fragment/PlayerAnimationDataFragment.h"

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
