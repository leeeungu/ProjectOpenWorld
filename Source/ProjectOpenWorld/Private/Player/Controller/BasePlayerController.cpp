#include "Player/Controller/BasePlayerController.h"
#include "Inventory/Component/InventoryComponent.h"

ABasePlayerController::ABasePlayerController() : Super()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	//SetMouseCursorWidget(EMouseCursor::Default, nullptr);
}
