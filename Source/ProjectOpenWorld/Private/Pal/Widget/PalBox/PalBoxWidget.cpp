#include "Pal/Widget/PalBox/PalBoxWidget.h"
#include "Pal/Widget/PalBox/PalInfomation.h"
#include "Pal/Widget/PalBox/PalInventoryWidget.h"
#include "Pal/Widget/PalBox/PalBoxSpawnWidget.h"
#include "Pal/Component/PalStorageComponent.h"
#include "Pal/Component/PalSpawnerComponent.h"
#include "Pal/Actor/PalBaseCamp_V2.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "Pal/Character/PalBaseCreature.h"
#include "Pal/Widget/PalBox/PalBoxItemTab.h"
#include "Components/WidgetSwitcher.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Pal/Widget/PalBox/PalBoxDDO.h"
#include "Pal/Widget/PalBox/PalInventorySlot.h"

bool UPalBoxWidget::SetMainWidget()
{
	bool bResult = Super::SetMainWidget();
	//UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetOwningPlayer(), this, EMouseLockMode::DoNotLock, true);
	return bResult;
}

void UPalBoxWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (UGameViewportSubsystem* Subsystem = UGameViewportSubsystem::Get(GetWorld()))
	{
		Subsystem->OnWidgetAdded.AddUFunction(this, FName("OnPalBoxWidgetAdded"));
	}
	if (PalInventoryWidget)
	{
		PalInventoryWidget->OnInventoryDropEvent.AddUniqueDynamic(this, &UPalBoxWidget::OnSlotOnDropEvent);
	}
}

void UPalBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//SetSelectedPanel(CurSelectedIndex);
}

void UPalBoxWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UPalBoxWidget::NativeDestruct()
{
	Super::NativeDestruct();
	PalStorageComponent = nullptr;
	if (PalSpawnerComponent.IsValid())
	{
		PalSpawnerComponent->OnContainerUpdated.RemoveDynamic(this, &UPalBoxWidget::OnUpdateSpawnSlot);
		PalSpawnerComponent = nullptr;
	}
}

FReply UPalBoxWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("UPalBoxWidget : OnKeyDown"));
	if (PalBoxSwither)
	{
		int32 Index = PalBoxSwither->GetActiveWidgetIndex();
		if (InKeyEvent.GetKey() == EKeys::Q && PalBoxSwither)
		{
			Index -= 1;
			if (Index  < 0)
			{
				Index = PalBoxSwither->GetChildrenCount() - 1;
			}
		}
		if (InKeyEvent.GetKey() == EKeys::E && PalBoxSwither)
		{
			Index += 1;
			if (Index >= PalBoxSwither->GetChildrenCount())
			{
				Index = 0;
			}
		}
		if (Index != PalBoxSwither->GetActiveWidgetIndex())
		{
			PalBoxSwither->SetActiveWidgetIndex(Index);
		}
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UPalBoxWidget::OnPalSelectedChanged(APalBaseCreature* SelectedPal)
{
	CurrentSelectedPal = SelectedPal;
	//if(PalInfoWidget )
	//	PalInfoWidget->SetPalCreature(CurrentSelectedPal.Get());
}



void UPalBoxWidget::OnPalSpawnInventoryChanged(int nIndex, AActor* Actor)
{
	if (PalBoxSpawnWidget)
		PalBoxSpawnWidget->UpdatePalSpawnInventory(nIndex, Cast< APalBaseCreature>(Actor));
}

TObjectPtr<APalBaseCreature>  UPalBoxWidget::GetPalInInventory(int Index) const
{
	return PalStorageComponent.IsValid() ? Cast<APalBaseCreature>(PalStorageComponent->GetStoredPal(Index)) : nullptr;
}

void UPalBoxWidget::SetOwnerActor(AActor* NewOwner)
{
	Super::SetOwnerActor(NewOwner);

}

void UPalBoxWidget::SetStorageComponent(UPalStorageComponent* Storage)
{
	PalStorageComponent = Storage;
	if (PalInventoryWidget)
	{
		PalInventoryWidget->SetStorageComponent(PalStorageComponent.Get());
	}
}

void UPalBoxWidget::SetPalSpawnerComponent(UPalSpawnerComponent* Spawner)
{
	if (PalSpawnerComponent.IsValid())
	{
		PalSpawnerComponent->OnContainerUpdated.RemoveDynamic(this, &UPalBoxWidget::OnUpdateSpawnSlot);
	}
	PalSpawnerComponent = Spawner;
	if (PalSpawnerComponent.IsValid())
	{
		PalSpawnerComponent->OnContainerUpdated.AddUniqueDynamic(this, &UPalBoxWidget::OnUpdateSpawnSlot);
	}
}

void UPalBoxWidget::SwapPalInInventory(int FromIndex, int ToIndex)
{
	if (PalStorageComponent.IsValid())
	{
		PalStorageComponent->SwapStoredPals(FromIndex, ToIndex);
	}
}

void UPalBoxWidget::SpawnSlotFromInventory(int FromIndex, int ToIndex)
{
	if (PalSpawnerComponent.IsValid())
	{
		//PalStorageComponent->SpawnPal(FromIndex, ToIndex);
	}
}

void UPalBoxWidget::SwapSpawnInventory(int FromIndex, int ToIndex)
{
	if (PalSpawnerComponent.IsValid())
	{
		PalSpawnerComponent->SwapSpawnedPals(FromIndex, ToIndex);
	}
}

void UPalBoxWidget::DespawnSlotToInventory(int FromIndex, int ToIndex)
{
	if (PalSpawnerComponent.IsValid())
	{
		//PalStorageComponent->DeSpawnPal(ToIndex, FromIndex);
	}
}

void UPalBoxWidget::OnPalBoxWidgetAdded(UWidget* Widget, ULocalPlayer* LocalPlayer)
{
	if (Widget == this)
	{
		CurrentSelectedPal = nullptr;
		OnPalBoxChangeEvent();
	}
}

void UPalBoxWidget::OnPalBoxChangeEvent()
{
	if (PalInfoWidget)
		PalInfoWidget->SetPalCreature(CurrentSelectedPal.Get());
	/*
	if (PalInventoryWidget)
		PalInventoryWidget->SetPalSlot();*/
}


UPalItemInventoryWidget* UPalBoxWidget::GetPalItemInventoryWidget() const
{
	if (!PalBoxItemTab)
		return nullptr;
	return PalBoxItemTab->GetPalItemInventoryWidget();
}

void UPalBoxWidget::StorePal(struct FPalStoreInventoryData NewPal)
{
	if(PalStorageComponent.IsValid())
		PalStorageComponent->StorePal(NewPal);
}
void UPalBoxWidget::RemovePal(int Index)
{
	if (PalStorageComponent.IsValid())
	{
		PalStorageComponent->RemovePal(Index);
	}
}

void UPalBoxWidget::OnUpdateSpawnSlot(int32 Index, AActor* preActor)
{
	if (PalBoxSpawnWidget && PalSpawnerComponent.IsValid())
	{
		PalBoxSpawnWidget->UpdatePalSpawnInventory(Index, PalSpawnerComponent->GetPal(Index));
	}
}

void UPalBoxWidget::OnSlotOnDropEvent(int32 SlotIndex, UPalBoxDDO* PalBoxDDO)
{
	if (!PalBoxDDO)
		return;
	if (!PalBoxDDO->IsFromInventory)
	{
		UPalStorageComponent* OwnerCom = GetPalStorageComponent();
		if (PalBoxDDO->PalSpawnerComponent && OwnerCom)
		{
			AActor* pTo = OwnerCom->GetStoredPal(SlotIndex);
			AActor* pFrom = PalBoxDDO->PalSpawnerComponent->GetPal(PalBoxDDO->Index);
			OwnerCom->RemovePal(SlotIndex);
			PalBoxDDO->PalSpawnerComponent->RemovePal(PalBoxDDO->Index);
			if (pFrom)
			{
				OwnerCom->StorePal({ pFrom,SlotIndex });
			}

			if (pTo)
			{
				PalBoxDDO->PalSpawnerComponent->StorePal(pTo, PalBoxDDO->Index);
			}
		}
	}
}


UPalStorageComponent* UPalBoxWidget::GetPalStorageComponent() const
{
	return PalStorageComponent.Get(); 
}

UPalSpawnerComponent* UPalBoxWidget::GetPalSpawnerComponent() const
{
	return PalSpawnerComponent.Get(); 
}