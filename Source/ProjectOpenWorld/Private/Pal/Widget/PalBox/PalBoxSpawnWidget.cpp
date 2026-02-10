#include "Pal/Widget/PalBox/PalBoxSpawnWidget.h"
#include "Pal/Widget/PalBox/PalBoxSpawnSlot.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "Creature/Character/BaseCreature.h"

void UPalBoxSpawnWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	for (int i = 0; i < SpawnBoxScrollBox->GetChildrenCount(); i++)
	{
		UPalBoxSpawnSlot* SpawnSlot = Cast<UPalBoxSpawnSlot>(SpawnBoxScrollBox->GetChildAt(i));
		if (SpawnSlot)
		{
			SpawnSlot->SetSlotIndex(i);
		}
	}
}

void UPalBoxSpawnWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UPalBoxSpawnWidget::UpdatePalSpawnInventory(int InventoryIndex, AActor* NewActor)
{
	if (SpawnBoxScrollBox)
	{
		UPalBoxSpawnSlot* SpawnSlot = Cast<UPalBoxSpawnSlot>(SpawnBoxScrollBox->GetChildAt(InventoryIndex));
		if (SpawnSlot)
		{
			SpawnSlot->SetPalCreature(Cast<ABaseCreature>(NewActor));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PalBoxSpawnWidget: SpawnInventoryVerticalBox is not bound"));
	}
}

void UPalBoxSpawnWidget::CreateSpawnSlotWidget(int SlotIndex)
{
	if (SpawnBoxScrollBox && PalBoxSpawnSlotClass)
	{
		SlotIndex = SlotIndex - SpawnBoxScrollBox->GetChildrenCount();
		for (int i = 0; i < SlotIndex; i++)
		{
			UPalBoxSpawnSlot* SpawnSlot = CreateWidget<UPalBoxSpawnSlot>(this, PalBoxSpawnSlotClass);
			if (SpawnBoxScrollBox->AddChild(SpawnSlot))
			{
				UE_LOG(LogTemp, Log, TEXT("PalBoxSpawnWidget: Created SpawnSlot widget at index %d %s"), i + SpawnBoxScrollBox->GetChildrenCount(), *PalBoxSpawnSlotClass->GetName());
				SpawnSlot->SetSlotIndex(i + SpawnBoxScrollBox->GetChildrenCount());
			}
		}
	}
}
