#include "Pal/Widget/PalBox/PalBoxWidget.h"
#include "Pal/Widget/PalBox/PalInfomation.h"
#include "Pal/Widget/PalBox/PalInventoryWidget.h"
#include "Pal/Widget/PalBox/PalBoxSpawnWidget.h"
#include "Pal/Component/PalStorageComponent.h"
#include "Pal/Actor/PalBaseCamp.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "Creature/Character/BaseCreature.h"

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
}

void UPalBoxWidget::OnPalSelectedChanged(ABaseCreature* SelectedPal)
{
	CurrentSelectedPal = SelectedPal;
	if(PalInfoWidget )
		PalInfoWidget->SetPalCreature(CurrentSelectedPal.Get());
}

void UPalBoxWidget::OnPalInventoryChanged(int nIndex, AActor* Actor)
{
	if (PalInventoryWidget)
		PalInventoryWidget->UpdatePalInventory(nIndex, Cast< ABaseCreature>(Actor));
}

void UPalBoxWidget::OnPalSpawnInventoryChanged(int nIndex, AActor* Actor)
{
	if (PalBoxSpawnWidget)
		PalBoxSpawnWidget->UpdatePalSpawnInventory(nIndex, Cast< ABaseCreature>(Actor));
}

TObjectPtr<ABaseCreature>  UPalBoxWidget::GetPalInInventory(int Index) const
{
	return PalStorageComponent ? Cast<ABaseCreature>(PalStorageComponent->GetStoredPal(Index)) : nullptr;
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
