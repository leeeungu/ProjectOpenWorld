#include "Pal/Widget/PalBox/PalInventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "Pal/Widget/PalBox/PalInventorySlot.h"
#include "Pal/Character/PalBaseCreature.h"
#include "Pal/Widget/PalBox/PalBoxWidget.h"
#include "Pal/Interface/PalSlotSelectedInterface.h"
#include "Pal/Component/PalStorageComponent.h"
#include "Pal/Widget/PalBox/PalBoxDDO.h"

void UPalInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!PalBoxGridPanel || !InventorySlotClass)
		return;
	if (PalBoxGridPanel->GetChildrenCount() != NumRows * NumColumns)
	{
		PalBoxGridPanel->ClearChildren();
		for (int32 IndexRow = 0; IndexRow < NumRows; ++IndexRow)
		{
			for (int32 IndexCol = 0; IndexCol < NumColumns; ++IndexCol)
			{
				UPalInventorySlot* NewSlotWidget = CreateWidget<UPalInventorySlot>(this, InventorySlotClass);
				if (NewSlotWidget)
				{
					NewSlotWidget->SetSlotIndex(IndexRow * NumColumns + IndexCol);
					NewSlotWidget->OnInitialized(this);
					PalBoxGridPanel->AddChildToUniformGrid(NewSlotWidget, IndexRow, IndexCol);
				}
			}
		}
	}
}

void UPalInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetPalSlot();
}

void UPalInventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (PalBoxGridPanel && PalBoxGridPanel->GetChildrenCount() != NumRows * NumColumns)
	{
		PalBoxGridPanel->ClearChildren();
		for (int32 IndexRow = 0; IndexRow < NumRows; ++IndexRow)
		{
			for (int32 IndexCol = 0; IndexCol < NumColumns; ++IndexCol)
			{
				UPalInventorySlot* NewSlotWidget = CreateWidget<UPalInventorySlot>(this, InventorySlotClass);
				if (NewSlotWidget)
				{
					NewSlotWidget->SetSlotIndex(IndexRow * NumColumns + IndexCol);
					NewSlotWidget->OnInitialized(this);
					PalBoxGridPanel->AddChildToUniformGrid(NewSlotWidget, IndexRow, IndexCol);
				}
			}
		}
	}
}

void UPalInventoryWidget::UpdatePalInventory(int InventoryIndex, TObjectPtr<APalBaseCreature> NewPal)
{
	if (!PalStorageComponent.IsValid())
		return;
	int32 IndexRow = InventoryIndex / NumColumns;
	int32 IndexCol = InventoryIndex % NumColumns;

	int32 SlotIndex = IndexRow * NumColumns + IndexCol;
	if (UPalInventorySlot* SlotWidget = Cast<UPalInventorySlot>(PalBoxGridPanel->GetChildAt(SlotIndex)))
	{
		TObjectPtr<APalBaseCreature> SelectedPal = nullptr;
		SelectedPal = Cast< APalBaseCreature>(PalStorageComponent->GetStoredPal(SlotIndex));
		SlotWidget->SetPalCreature(SelectedPal);
	}
}

void UPalInventoryWidget::SetPalSlot()
{
	if (!PalStorageComponent.IsValid())
		return;
	for (int32 IndexRow = 0; IndexRow < NumRows; ++IndexRow)
	{
		for (int32 IndexCol = 0; IndexCol < NumColumns; ++IndexCol)
		{
			int32 SlotIndex = IndexRow * NumColumns + IndexCol;
			if (UPalInventorySlot* SlotWidget = Cast<UPalInventorySlot>(PalBoxGridPanel->GetChildAt(SlotIndex)))
			{
				TObjectPtr<APalBaseCreature> SelectedPal = nullptr;
				SelectedPal = Cast< APalBaseCreature>(PalStorageComponent->GetStoredPal(SlotIndex));
				SlotWidget->OnSlotSelected.AddUniqueDynamic(this, &UPalInventoryWidget::OnSlotSelected);
				SlotWidget->SetPalCreature(SelectedPal);
			}
		}
	}
}

void UPalInventoryWidget::SetStorageComponent(UPalStorageComponent* Storage)
{
	if (PalStorageComponent.IsValid())
	{
		PalStorageComponent->OnPalStoreChanged.RemoveDynamic(this, &UPalInventoryWidget::OnPalInventoryChanged);
	}
	PalStorageComponent = Storage;
	if (PalStorageComponent.IsValid())
	{
		PalStorageComponent->OnPalStoreChanged.AddUniqueDynamic(this, &UPalInventoryWidget::OnPalInventoryChanged);
	}
}

UPalStorageComponent* UPalInventoryWidget::GetPalStorageComponent() const
{
	return PalStorageComponent.Get();
}

void UPalInventoryWidget::OnSlotOnDropEvent(int32 SlotIndex, UPalBoxDDO* PalBoxDDO)
{
	if (PalBoxDDO)
	{
		if (PalStorageComponent.IsValid() && PalBoxDDO->PalStorageComponent == PalStorageComponent && PalBoxDDO->IsFromInventory)
		{
			PalStorageComponent->SwapStoredPals(PalBoxDDO->Index, SlotIndex);
		}
		else
		{
			OnInventoryDropEvent.Broadcast(SlotIndex, PalBoxDDO);
		}
	}
	
}

void UPalInventoryWidget::OnPalInventoryChanged(int nIndex, AActor* Actor)
{
	UpdatePalInventory(nIndex, Cast< APalBaseCreature>(Actor));
}

void UPalInventoryWidget::OnSlotSelected(int32 Index, AActor* SelectedCreature)
{
	for (UWidget* Widget : SelectEvent)
	{
		if (Widget && Widget->Implements<UPalSlotSelectedInterface>())
		{
			IPalSlotSelectedInterface::Execute_OnSlotSelected(Widget, Index, SelectedCreature);
		}
	}
}

#if WITH_EDITOR
void UPalInventoryWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName MemberName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;
	if (MemberName == GET_MEMBER_NAME_CHECKED(UPalInventoryWidget, SelectEvent))
	{
		for (int32 Index = 0; Index < SelectEvent.Num(); ++Index)
		{
			const UWidget* Widget = SelectEvent[Index];
			if (Widget && !Widget->Implements<UPalSlotSelectedInterface>())
			{
				UE_LOG(LogTemp, Error, TEXT("[%s] SelectEvent[%d] '%s'(%s)가 UPalSlotSelectedInterface를 구현하지 않음"),
					*GetName(), Index, *Widget->GetName(), *Widget->GetClass()->GetName());
			}
		}
	}
}
#endif
