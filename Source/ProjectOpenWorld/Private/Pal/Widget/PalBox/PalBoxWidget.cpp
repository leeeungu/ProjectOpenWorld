#include "Pal/Widget/PalBox/PalBoxWidget.h"
#include "Pal/Widget/PalBox/PalInfomation.h"
#include "Pal/Widget/PalBox/PalInventoryWidget.h"
#include "Pal/Widget/PalBox/PalBoxSpawnWidget.h"
#include "Pal/Component/PalStorageComponent.h"
#include "Pal/Actor/PalBaseCamp.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "Pal/Character/PalBaseCreature.h"
#include "Pal/Widget/PalBox/PalBoxItemTab.h"
#include "Components/WidgetSwitcher.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

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
}

void UPalBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetSelectedPanel(CurSelectedIndex);
}

void UPalBoxWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

FReply UPalBoxWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("UPalBoxWidget : OnKeyDown"));
	if (PalBoxSwither)
	{
		int32 Index = PalBoxSwither->GetActiveWidgetIndex();
		if (InKeyEvent.GetKey() == EKeys::Q && PalBoxSwither)
		{
			if (Index - 1 < 0)
			{
				Index = PalBoxSwither->GetChildrenCount() - 1;
			}
		}
		if (InKeyEvent.GetKey() == EKeys::E && PalBoxSwither)
		{
			if (Index + 1 >= PalBoxSwither->GetChildrenCount())
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

void UPalBoxWidget::SetSelectedPanel(int32 SelectedIndex)
{
	if (PalBoxSwither && SelectedIndex >= 0 && SelectedIndex < PalBoxSwither->GetChildrenCount())
	{
		CurSelectedIndex = SelectedIndex;
		PalBoxSwither->SetActiveWidgetIndex(CurSelectedIndex);
	}
}


void UPalBoxWidget::OnPalSelectedChanged(APalBaseCreature* SelectedPal)
{
	CurrentSelectedPal = SelectedPal;
	if(PalInfoWidget )
		PalInfoWidget->SetPalCreature(CurrentSelectedPal.Get());
}

void UPalBoxWidget::OnPalInventoryChanged(int nIndex, AActor* Actor)
{
	if (PalInventoryWidget)
		PalInventoryWidget->UpdatePalInventory(nIndex, Cast< APalBaseCreature>(Actor));
}

void UPalBoxWidget::OnPalSpawnInventoryChanged(int nIndex, AActor* Actor)
{
	if (PalBoxSpawnWidget)
		PalBoxSpawnWidget->UpdatePalSpawnInventory(nIndex, Cast< APalBaseCreature>(Actor));
}

TObjectPtr<APalBaseCreature>  UPalBoxWidget::GetPalInInventory(int Index) const
{
	return PalStorageComponent ? Cast<APalBaseCreature>(PalStorageComponent->GetStoredPal(Index)) : nullptr;
}

void UPalBoxWidget::SetOwnerActor(AActor* NewOwner)
{
	Super::SetOwnerActor(NewOwner);
	APalBaseCamp* PalBaseCamp = Cast<APalBaseCamp>(NewOwner);
	if (!PalBaseCamp)
	{
		UE_LOG(LogTemp, Warning, TEXT("PalBoxWidget: Owner is not PalBaseCamp"));
		return;
	}
	PalStorageComponent = PalBaseCamp->GetPalStoreComponent();
	if (!PalStorageComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("PalBoxWidget: PalBaseCamp has no CommanderComponent"));
		return;
	}
	PalBoxSpawnWidget->CreateSpawnSlotWidget(PalStorageComponent->GetSpawnInventorySize());
	PalStorageComponent->OnPalStoreChanged.AddUniqueDynamic(this, &UPalBoxWidget::OnPalInventoryChanged);
	PalStorageComponent->OnPalSpawnChanged.AddUniqueDynamic(this, &UPalBoxWidget::OnPalSpawnInventoryChanged);
}

void UPalBoxWidget::SwapPalInInventory(int FromIndex, int ToIndex)
{
	if (PalStorageComponent)
	{
		PalStorageComponent->SwapStoredPals(FromIndex, ToIndex);
	}
}

void UPalBoxWidget::SpawnSlotFromInventory(int FromIndex, int ToIndex)
{
	if (PalStorageComponent)
	{
		PalStorageComponent->SpawnPal(FromIndex, ToIndex);
	}
}

void UPalBoxWidget::SwapSpawnInventory(int FromIndex, int ToIndex)
{
	if (PalStorageComponent)
	{
		PalStorageComponent->SwapSpawnedPals(FromIndex, ToIndex);
	}
}

void UPalBoxWidget::DespawnSlotToInventory(int FromIndex, int ToIndex)
{
	if (PalStorageComponent)
	{
		PalStorageComponent->DeSpawnPal(ToIndex, FromIndex);
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
	if (PalInventoryWidget)
		PalInventoryWidget->SetPalSlot();
}


UPalItemInventoryWidget* UPalBoxWidget::GetPalItemInventoryWidget() const
{
	if (!PalBoxItemTab)
		return nullptr;
	return PalBoxItemTab->GetPalItemInventoryWidget();
}