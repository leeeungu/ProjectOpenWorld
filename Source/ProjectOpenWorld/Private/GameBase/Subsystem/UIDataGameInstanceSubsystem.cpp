#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"

void UUIDataGameInstanceSubsystem::PlayButtonClickSound()
{
	PlayUISound(EUISoundType::UIST_ButtonClick);
}

void UUIDataGameInstanceSubsystem::PlayUIOpenSound()
{
	PlayUISound(EUISoundType::UIST_UIOpen);
}

void UUIDataGameInstanceSubsystem::PlayUICloseSound()
{
	PlayUISound(EUISoundType::UIST_UIClose);
}

void UUIDataGameInstanceSubsystem::PlayUIHoverSound()
{
	PlayUISound(EUISoundType::UIST_UIHover);
}

void UUIDataGameInstanceSubsystem::PlayUITabChangeSound()
{
	PlayUISound(EUISoundType::UIST_UITabChange);
}

void UUIDataGameInstanceSubsystem::PlayUIBuildPressSound()
{
	PlayUISound(EUISoundType::UIST_UIBuildPress);
}

void UUIDataGameInstanceSubsystem::PlayUIBuildHoverSound()
{
	PlayUISound(EUISoundType::UIST_UIBuildHover);
}

void UUIDataGameInstanceSubsystem::PlayUIBuildCancelSound()
{
	PlayUISound(EUISoundType::UIST_UIBuildCancel);
}
//
//void UUIDataGameInstanceSubsystem::PlayUIBuildCompleteSound()
//{
//	PlayEffectSound(EEffectSoundType::EST_BuildingComplete, FVector::ZeroVector);
//	//PlayUISound(EUISoundType::UIST_UIBuildComplete);
//}
