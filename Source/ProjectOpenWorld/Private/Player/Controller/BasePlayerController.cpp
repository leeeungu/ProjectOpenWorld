#include "Player/Controller/BasePlayerController.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Blueprint/UserWidget.h"

ABasePlayerController::ABasePlayerController() : Super()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	//Script/UMGEditor.WidgetBlueprint'/Game/Inventory/Widget/WBP_TestInventory.WBP_TestInventory'
	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryWidgetBPClass(TEXT("/Game/Inventory/Widget/WBP_TestInventory.WBP_TestInventory_C"));
	if (InventoryWidgetBPClass.Succeeded())
	{
		InventoryWidgetClass = InventoryWidgetBPClass.Class;
	}
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
	}
}

void ABasePlayerController::ToggleInventory()
{
	if (InventoryWidget)
	{
		if (InventoryWidget->IsInViewport())
		{
			InventoryWidget->RemoveFromParent();
		}
		else
		{
			InventoryWidget->AddToViewport();
		}
	}
}

bool ABasePlayerController::bIsInventoryOpen() const
{
	if (InventoryWidget)
	{
		return InventoryWidget->IsInViewport();
	}
	return false;
}
