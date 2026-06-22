#include "Player/Controller/BasePlayerController.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "Player/Widget/PlayerInventoryWidget.h"
#include "Player/Component/PlayerPreviewComponent.h"
#include "Player/Character/BasePlayer.h"

void ABasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (PreviewComponent)
	{
		PreviewComponent->SetSourcePlayer(Cast<ABasePlayer>(InPawn));
	}
}

ABasePlayerController::ABasePlayerController() : Super()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	//Script/UMGEditor.WidgetBlueprint'/Game/Inventory/Widget/WBP_TestInventory.WBP_TestInventory'
	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryWidgetBPClass(TEXT("/Game/Inventory/Widget/WBP_TestInventory.WBP_TestInventory_C"));
	if (InventoryWidgetBPClass.Succeeded())
	{
		InventoryWidgetClass = InventoryWidgetBPClass.Class;
	}

	PreviewComponent = CreateDefaultSubobject<UPlayerPreviewComponent>(TEXT("PreviewComponent"));
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UPlayerInventoryWidget>(this, InventoryWidgetClass);
		if (InventoryWidget)
		{
			ABasePlayer* PlayerCharacter = Cast< ABasePlayer>(GetPawn());
			if (PlayerCharacter)
			{
				InventoryWidget->SetStatWidget(PlayerCharacter->GetPlayerStatComponent());
				InventoryWidget->SetStorageComponent(PlayerCharacter->GetPalStorageComponent());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ABasePlayerController::beginplay NoPwan"));
			}
		}
	}
}

//void ABasePlayerController::ToggleInventory()
//{
//	if (InventoryWidget)
//	{
//		if (InventoryWidget->IsInViewport())
//		{
//			InventoryWidget->RemoveFromParent();
//		}
//		else
//		{
//			InventoryWidget->AddToViewport();
//		}
//	}
//}

UUserWidget* ABasePlayerController::GetInventoryWidget() const
{ 
	return InventoryWidget;
}

bool ABasePlayerController::bIsInventoryOpen() const
{
	if (InventoryWidget)
	{
		return InventoryWidget->IsInViewport();
	}
	return false;
}

