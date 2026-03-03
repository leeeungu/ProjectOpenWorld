#include "Player/Controller/MainTitlePlayerController.h"
#include "GameBase/Widget/MainTitleWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void AMainTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (MainTitleWidgetClass)
	{
		MainTitleWidget = CreateWidget<UUserWidget>(this, MainTitleWidgetClass);
		if (MainTitleWidget)
		{
			MainTitleWidget->AddToViewport();
			SetShowMouseCursor(true);
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, MainTitleWidget, EMouseLockMode::DoNotLock, true, true);
			UGameplayStatics::SetViewportMouseCaptureMode(GetWorld(), EMouseCaptureMode::NoCapture);
		}
	}
}
