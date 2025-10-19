#include "Player/Controller/BasePlayerController.h"
#include "Inventory/Component/InventoryComponent.h"

ABasePlayerController::ABasePlayerController()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}
