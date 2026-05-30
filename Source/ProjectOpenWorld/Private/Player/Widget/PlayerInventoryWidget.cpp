#include "Player/Widget/PlayerInventoryWidget.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Player/Controller/BasePlayerController.h"
#include "Inventory/Widget/EquipSlotWidget.h"
#include "Inventory/Widget/InventorySlotBase.h"
#include "Player/Widget/PlayerStatusWidget.h"

bool UPlayerInventoryWidget::SetMainWidget()
{
	AddToViewport();
	GetOwningPlayer()->SetShowMouseCursor(true);
	UUIDataGameInstanceSubsystem::PlayUIOpenSound();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayer(), this, EMouseLockMode::DoNotLock, true, true);
	UGameplayStatics::SetViewportMouseCaptureMode(GetOwningPlayer()->GetWorld(), EMouseCaptureMode::NoCapture);
	return IsInViewport();
}

void UPlayerInventoryWidget::UnSetMainWidget() 
{
	RemoveFromParent();
	GetOwningPlayer()->SetShowMouseCursor(false);
	UUIDataGameInstanceSubsystem::PlayUICloseSound();
	UGameplayStatics::SetViewportMouseCaptureMode(GetOwningPlayer()->GetWorld(), EMouseCaptureMode::CaptureDuringMouseDown);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer(), true);
	UWidgetBlueprintLibrary::CancelDragDrop();
}

void UPlayerInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ABasePlayerController* playerController = Cast<ABasePlayerController>(GetOwningPlayer());
	if (UInventoryComponent* inventoryComponent = playerController ? playerController->GetInventoryComponent() : nullptr)
	{
		WeaponeSlot->SetSlotPtr(inventoryComponent->GetEquipSlot(EItemSlotType::Weapon));
		PickAxeSlot->SetSlotPtr(inventoryComponent->GetEquipSlot(EItemSlotType::PickAxe));
		AxeSlot->SetSlotPtr(inventoryComponent->GetEquipSlot(EItemSlotType::Axe));
		HeadEquipSlot->SetSlotPtr(inventoryComponent->GetEquipSlot(EItemSlotType::HeadEquip));
		BodyEquipSlot->SetSlotPtr(inventoryComponent->GetEquipSlot(EItemSlotType::BodyEquip));
	}
}

void UPlayerInventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ABasePlayerController* playerController = Cast<ABasePlayerController>(GetOwningPlayer());
	if (UInventoryComponent* inventoryComponent = playerController ? playerController->GetComponentByClass<UInventoryComponent>() : nullptr)
	{
		inventoryComponent->onUpdateInventory.AddUniqueDynamic(this, &UPlayerInventoryWidget::UpdatEquipSlot);
	}
}

void UPlayerInventoryWidget::UpdatEquipSlot()
{
	ABasePlayerController* playerController = Cast<ABasePlayerController>(GetOwningPlayer());
	if (UInventoryComponent* inventoryComponent = playerController ? playerController->GetInventoryComponent() : nullptr)
	{
		WeaponeSlot->SetSlotPtr(inventoryComponent->GetEquipSlot(EItemSlotType::Weapon));
		PickAxeSlot->SetSlotPtr(inventoryComponent->GetEquipSlot(EItemSlotType::PickAxe));
		AxeSlot->SetSlotPtr(inventoryComponent->GetEquipSlot(EItemSlotType::Axe));
		HeadEquipSlot->SetSlotPtr(inventoryComponent->GetEquipSlot(EItemSlotType::HeadEquip));
		BodyEquipSlot->SetSlotPtr(inventoryComponent->GetEquipSlot(EItemSlotType::BodyEquip));
	}
}


void UPlayerInventoryWidget::SetStatWidget(UStatComponent* StatCom)
{
	if (PlayerStatusWidget)
	{
		PlayerStatusWidget->SetStatWidget(StatCom);
	}
}