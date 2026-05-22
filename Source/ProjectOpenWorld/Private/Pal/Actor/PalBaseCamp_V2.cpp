#include "Pal/Actor/PalBaseCamp_V2.h"
#include "Pal/Component/PalWorkCommander.h"
#include "Pal/Component/PalWorkableSearchComponent.h"
#include "Pal/Component/PalInventory.h"
#include "Building/Component/BuildingActionWidgetComponent.h"
#include "Pal/Widget/PalBox/PalBoxWidget.h"
#include "Pal/Widget/Item/PalItemInventoryWidget.h"

APalBaseCamp_V2::APalBaseCamp_V2() : Super{}
{
	PrimaryActorTick.bCanEverTick = false;
	PalWorkableSearchComponent = CreateDefaultSubobject<UPalWorkableSearchComponent>(TEXT("PalWorkableSearchComponent"));
	PalWorkableSearchComponent->SetupAttachment(GetRootComponent());
	PalWorkCommander = CreateDefaultSubobject<UPalWorkCommander>(TEXT("PalWorkCommander"));
	PalInventory = CreateDefaultSubobject<UPalInventory>(TEXT("PalInventory"));
}

void APalBaseCamp_V2::BeginPlay()
{
	if (PalWorkableSearchComponent && PalWorkCommander)
	{
		PalWorkableSearchComponent->OnComponentBeginOverlap.AddUniqueDynamic(PalWorkCommander.Get(), &UPalWorkCommander::OnBeginSearch);
		PalWorkableSearchComponent->OnComponentEndOverlap.AddUniqueDynamic(PalWorkCommander.Get(), &UPalWorkCommander::OnEndSearch);
	}
	Super::BeginPlay();
	if (BuildActionWidget && PalInventory)
	{
		if (UPalBoxWidget* PalBoxWidget = Cast< UPalBoxWidget>(BuildActionWidget->GetWidget()))
		{
			if (UPalItemInventoryWidget* PalItemInventoryWidget = PalBoxWidget->GetPalItemInventoryWidget())
			{
				PalItemInventoryWidget->OnWidgetOpen.AddUniqueDynamic(PalInventory, &UPalInventory::OnOpenUI);
				PalItemInventoryWidget->OnSlotSwap.AddUniqueDynamic(PalInventory, &UPalInventory::OnSwapSlots);
				PalInventory->OnSlotChanged.AddUniqueDynamic(PalItemInventoryWidget, &UPalItemInventoryWidget::OnUpdateSlot);
			}
		}
	}
}

