#include "GameBase/Widget/PlayerGameOver.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UPlayerGameOver::OnRestartButtonClicked()
{
	if (GetOwningPlayerPawn())
		GetOwningPlayerPawn()->Restart();
}

void UPlayerGameOver::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (RestartButton)
	{
		RestartButton->OnClicked.AddUniqueDynamic(this, &UPlayerGameOver::OnRestartButtonClicked);
	}
}

void UPlayerGameOver::NativeConstruct()
{
	Super::NativeConstruct();
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, this, EMouseLockMode::DoNotLock, true);
		//PlayerController->()
	}
}

void UPlayerGameOver::NativeDestruct()
{
	Super::NativeDestruct();
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(false);
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController, true);
	}
}

bool UPlayerGameOver::SetMainWidget()
{
	AddToViewport();
	return true;
}

void UPlayerGameOver::UnSetMainWidget()
{
	RemoveFromParent();
}