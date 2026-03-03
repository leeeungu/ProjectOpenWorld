#include "GameBase/Widget/MainTitleWidget.h"
#include "Components/Button.h"
#include "GameBase/GameMode/BaseGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameBase/Subsystem/SoundGameInstanceSubsystem.h"


void UMainTitleWidget::OnStartButtonClicked()
{
	if (MainLevel.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("MainLevel is Nullptr"));
		return;
	}
	USoundGameInstanceSubsystem::PlayUISound(ButtonClickSoundType);
	UBaseGameInstance::OpenLevelStatic(this, MainLevel);
}

void UMainTitleWidget::OnExitButtonClicked()
{
	if (!GetWorld())
		return;
	USoundGameInstanceSubsystem::PlayUISound(ButtonClickSoundType);
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Type::Quit, false);
}

void UMainTitleWidget::OnButtonHovered()
{
	USoundGameInstanceSubsystem::PlayUISound(ButtonHoverSoundType);
}

void UMainTitleWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ButtonHoverSoundType = EUISoundType::UIST_UIHover;
	ButtonClickSoundType = EUISoundType::UIST_ButtonClick;
	if (StartButton)
	{
		StartButton->OnClicked.AddUniqueDynamic(this, &UMainTitleWidget::OnStartButtonClicked);
		StartButton->OnHovered.AddUniqueDynamic(this, &UMainTitleWidget::OnButtonHovered);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddUniqueDynamic(this, &UMainTitleWidget::OnExitButtonClicked);
		ExitButton->OnHovered.AddUniqueDynamic(this, &UMainTitleWidget::OnButtonHovered);
	}
}