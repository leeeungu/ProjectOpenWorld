#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

UUIDataGameInstanceSubsystem*  UUIDataGameInstanceSubsystem::Instance{};


UUIDataGameInstanceSubsystem::UUIDataGameInstanceSubsystem() : Super()
{
	//Script/Engine.SoundCue'/Game/Pal/Sound/Events/SE/UI/Item/SC_Click_sound_14__SFX__Cue.SC_Click_sound_14__SFX__Cue'
	static ConstructorHelpers::FObjectFinder<USoundBase> ButtonClickSoundObject(TEXT("SoundCue'/Game/Pal/Sound/Events/SE/UI/Item/SC_Click_sound_14__SFX__Cue.SC_Click_sound_14__SFX__Cue'"));
	if (ButtonClickSoundObject.Succeeded())
	{
		ButtonClickSound = ButtonClickSoundObject.Object;
	}
	//Script/Engine.SoundCue'/Game/Pal/Sound/Events/SE/UI/Common/SC_Mainmenu_Close__SFX__Cue.SC_Mainmenu_Close__SFX__Cue'
	
	static ConstructorHelpers::FObjectFinder<USoundBase> UICloseObject(TEXT("SoundCue'/Game/Pal/Sound/Events/SE/UI/Common/SC_Mainmenu_Close__SFX__Cue.SC_Mainmenu_Close__SFX__Cue'"));
	if (UICloseObject.Succeeded())
	{
		UIClose = UICloseObject.Object;
	}

	//Script/Engine.SoundCue'/Game/Pal/Sound/Events/SE/UI/Common/SC_Mainmenu_Open_v3__SFX__Cue.SC_Mainmenu_Open_v3__SFX__Cue'
	static ConstructorHelpers::FObjectFinder<USoundBase> UIOpenObject(TEXT("SoundCue'/Game/Pal/Sound/Events/SE/UI/Common/SC_Mainmenu_Open_v3__SFX__Cue.SC_Mainmenu_Open_v3__SFX__Cue'"));
	if (UIOpenObject.Succeeded())
	{
		UIOpen = UIOpenObject.Object;
	}

	//Script/Engine.SoundCue'/Game/Pal/Sound/Events/SE/UI/Common/SC_UIHover.SC_UIHover'
	static ConstructorHelpers::FObjectFinder<USoundBase> UIHoverObject(TEXT("SoundCue'/Game/Pal/Sound/Events/SE/UI/Common/SC_UIHover.SC_UIHover'"));
	if (UIHoverObject.Succeeded())
	{
		UIHover = UIHoverObject.Object;
	}

	//Script/Engine.SoundCue'/Game/Pal/Sound/Events/SE/UI/Common/SC_Mainmenu_TabChange__SFX__Cue.SC_Mainmenu_TabChange__SFX__Cue'
	static ConstructorHelpers::FObjectFinder<USoundBase> UITabChangeObject(TEXT("SoundCue'/Game/Pal/Sound/Events/SE/UI/Common/SC_Mainmenu_TabChange__SFX__Cue.SC_Mainmenu_TabChange__SFX__Cue'"));
	if (UITabChangeObject.Succeeded())
	{
		UITabChange = UITabChangeObject.Object;
	}

	// Script / Engine.SoundCue'/Game/Pal/Sound/Events/SE/UI/Build/SC_Build_Hover__SFX__Cue.SC_Build_Hover__SFX__Cue'
	static ConstructorHelpers::FObjectFinder<USoundBase> UIBuildHoverObject(TEXT("SoundCue'/Game/Pal/Sound/Events/SE/UI/Build/SC_Build_Hover__SFX__Cue.SC_Build_Hover__SFX__Cue'"));
	if (UIBuildHoverObject.Succeeded())
	{
		UIBuildHover = UIBuildHoverObject.Object;
	}
	// Script / Engine.SoundCue'/Game/Pal/Sound/Events/SE/UI/Build/SC_Build_Press__SFX__Cue.SC_Build_Press__SFX__Cue'
	static ConstructorHelpers::FObjectFinder<USoundBase> UIBuildPressObject(TEXT("SoundCue'/Game/Pal/Sound/Events/SE/UI/Build/SC_Build_Press__SFX__Cue.SC_Build_Press__SFX__Cue'"));
	if (UIBuildPressObject.Succeeded())
	{
		UIBuildPress = UIBuildPressObject.Object;
	}
	
	// Script / Engine.SoundCue'/Game/Pal/Sound/Events/SE/UI/Build/SC_Object_Demolition_particle__SFX__Cue.SC_Object_Demolition_particle__SFX__Cue'
	static ConstructorHelpers::FObjectFinder<USoundBase> UIBuildCancelObject(TEXT("SoundCue'/Game/Pal/Sound/Events/SE/UI/Build/SC_Object_Demolition_particle__SFX__Cue.SC_Object_Demolition_particle__SFX__Cue'"));
	if (UIBuildCancelObject.Succeeded())
	{
		UIBuildCancel = UIBuildCancelObject.Object;
	}

	///Script/Engine.SoundCue'/Game/Pal/Sound/Events/SE/Player/Architecture/SC_Build_Complete_Cue.SC_Build_Complete_Cue'
	static ConstructorHelpers::FObjectFinder<USoundBase> UIBuildCompleteObject(TEXT("SoundCue'/Game/Pal/Sound/Events/SE/Player/Architecture/SC_Build_Complete_Cue.SC_Build_Complete_Cue'"));
	if (UIBuildCompleteObject.Succeeded())
	{
		UIBuildComplete = UIBuildCompleteObject.Object;
	}
}

void UUIDataGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Instance = this;
}

void UUIDataGameInstanceSubsystem::PlayButtonClickSound()
{
	if (!Instance || !Instance->ButtonClickSound)
		return;
	UGameplayStatics::PlaySound2D(Instance->GetWorld(), Instance->ButtonClickSound);
}

void UUIDataGameInstanceSubsystem::PlayUIOpenSound()
{
	if (!Instance || !Instance->UIOpen)
		return;
	UGameplayStatics::PlaySound2D(Instance->GetWorld(), Instance->UIOpen);
}

void UUIDataGameInstanceSubsystem::PlayUICloseSound()
{
	if (!Instance || !Instance->UIClose)
		return;
	UGameplayStatics::PlaySound2D(Instance->GetWorld(), Instance->UIClose);
}

void UUIDataGameInstanceSubsystem::PlayUIHoverSound()
{
	if (!Instance || !Instance->UIHover)
		return;
	UGameplayStatics::PlaySound2D(Instance->GetWorld(), Instance->UIHover);
}

void UUIDataGameInstanceSubsystem::PlayUITabChangeSound()
{
	if (!Instance || !Instance->UITabChange)
		return;
	UGameplayStatics::PlaySound2D(Instance->GetWorld(), Instance->UITabChange);
}

void UUIDataGameInstanceSubsystem::PlayUIBuildPressSound()
{
	if (!Instance || !Instance->UIBuildPress)
		return;
	UGameplayStatics::PlaySound2D(Instance->GetWorld(), Instance->UIBuildPress);
}

void UUIDataGameInstanceSubsystem::PlayUIBuildHoverSound()
{
	if (!Instance || !Instance->UIBuildHover)
		return;
	UGameplayStatics::PlaySound2D(Instance->GetWorld(), Instance->UIBuildHover);
}

void UUIDataGameInstanceSubsystem::PlayUIBuildCancelSound()
{
	if (!Instance || !Instance->UIBuildCancel)
		return;
	UGameplayStatics::PlaySound2D(Instance->GetWorld(), Instance->UIBuildCancel);
}

void UUIDataGameInstanceSubsystem::PlayUIBuildCompleteSound()
{
	if (!Instance || !Instance->UIBuildComplete)
		return;
	UGameplayStatics::PlaySound2D(Instance->GetWorld(), Instance->UIBuildComplete);
}
