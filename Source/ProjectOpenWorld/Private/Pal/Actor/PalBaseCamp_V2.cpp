#include "Pal/Actor/PalBaseCamp_V2.h"
#include "Pal/Component/PalWorkCommander.h"
#include "Pal/Component/PalWorkableSearchComponent.h"
#include "Pal/Component/PalInventory.h"
#include "Pal/Component/ActionWidget/PalActionWidgetComponent.h"
#include "Pal/Widget/PalBox/PalBoxWidget.h"
#include "Pal/Widget/Item/PalItemInventoryWidget.h"
#include "Building/Component/PalBuildingStaticMeshComponent.h"
#include "Player/Character/BasePlayer.h"
#include "Pal/Component/PalStorageComponent.h"
#include "Pal/Component/PalSpawnerComponent.h"

APalBaseCamp_V2::APalBaseCamp_V2() : Super{}
{
	PrimaryActorTick.bCanEverTick = false;
	PalWorkableSearchComponent = CreateDefaultSubobject<UPalWorkableSearchComponent>(TEXT("PalWorkableSearchComponent"));
	PalWorkableSearchComponent->SetupAttachment(GetRootComponent());
	PalWorkCommander = CreateDefaultSubobject<UPalWorkCommander>(TEXT("PalWorkCommander"));
	PalInventory = CreateDefaultSubobject<UPalInventory>(TEXT("PalInventory"));
	//PalStorageComponent = CreateDefaultSubobject<UPalStorageComponent>(TEXT("PalStorageComponent"));

	PalSpawnerComponent = CreateDefaultSubobject<UPalSpawnerComponent>(TEXT("PalSpawnerComponent"));
	PalSpawnerComponent->SetupAttachment(GetRootComponent());
}

void APalBaseCamp_V2::OnDelActionWidget(UUserWidget* ActionWidget)
{
	if (PalInventory)
	{
		if (UPalBoxWidget* PalBoxWidget = Cast< UPalBoxWidget>(BuildActionWidgetCom->GetActionWidget()))
		{
			if (UPalItemInventoryWidget* PalItemInventoryWidget = PalBoxWidget->GetPalItemInventoryWidget())
			{
				PalInventory->OnSlotChanged.RemoveAll(PalItemInventoryWidget);
				PalItemInventoryWidget->OnNativeDestruct.RemoveAll(this);
				PalItemInventoryWidget->OnWidgetOpen.RemoveAll(PalInventory);
				PalItemInventoryWidget->OnSlotSwap.RemoveAll(PalInventory);
				PalItemInventoryWidget->OnSlotUpdate.RemoveAll(PalInventory);
				PalItemInventoryWidget->OnSlotRemove.RemoveAll(PalInventory);
				PalItemInventoryWidget->OnAddItem.Unbind();
			}
		}
	}
}

void APalBaseCamp_V2::BeginPlay()
{
	if (PalWorkableSearchComponent && PalWorkCommander)
	{
		PalWorkableSearchComponent->OnComponentBeginOverlap.AddUniqueDynamic(PalWorkCommander.Get(), &UPalWorkCommander::OnBeginSearch);
		PalWorkableSearchComponent->OnComponentEndOverlap.AddUniqueDynamic(PalWorkCommander.Get(), &UPalWorkCommander::OnEndSearch);
	}
	Super::BeginPlay();
	if (PalSpawnerComponent)
	{
		PalSpawnerComponent->OnContainerUpdated.AddDynamic(this, &APalBaseCamp_V2::SpawnPal);
	}
}

void APalBaseCamp_V2::SpawnPal(int32 Index, AActor* Pre)
{
	if (PalSpawnerComponent)
	{
		PalSpawnerComponent->SpawnPal(Index);
	}

}

void APalBaseCamp_V2::OnInteractionStart_Implementation(ACharacter* pOther)
{
	if (!pOther)
		return;
	if (!GetBuildingProgress()->IsBuildingEnd())
	{
		GetBuildingProgress()->StartBuilding(pOther);
	}
	else if (ABasePlayer* pPlayer = Cast<ABasePlayer>(pOther))
	{
		if (BuildActionWidgetCom)
		{
			if (!BuildActionWidgetCom->GetActionWidget())
			{
				BuildActionWidgetCom->CreateActionWidget();
				if (PalInventory)
				{
					if (UPalBoxWidget* PalBoxWidget = Cast< UPalBoxWidget>(BuildActionWidgetCom->GetActionWidget()))
					{
						PalBoxWidget->SetStorageComponent(pPlayer->GetPalStorageComponent());
						PalBoxWidget->SetPalSpawnerComponent(PalSpawnerComponent);
						if (UPalItemInventoryWidget* PalItemInventoryWidget = PalBoxWidget->GetPalItemInventoryWidget())
						{
							PalInventory->OnSlotChanged.AddUniqueDynamic(
								PalItemInventoryWidget, &UPalItemInventoryWidget::OnUpdateSlot);
							PalItemInventoryWidget->OnNativeDestruct.AddUObject(
								this, &APalBaseCamp_V2::OnDelActionWidget);
							PalItemInventoryWidget->OnWidgetOpen.AddUniqueDynamic(PalInventory, &UPalInventory::OnOpenUI);
							PalItemInventoryWidget->OnSlotSwap.AddUniqueDynamic(PalInventory, &UPalInventory::OnSwapSlots);
							PalItemInventoryWidget->OnSlotUpdate.AddUniqueDynamic(PalInventory, &UPalInventory::OnSlotUpdate);
							PalItemInventoryWidget->OnSlotRemove.AddUniqueDynamic(PalInventory, &UPalInventory::OnRemoveItem);
							PalItemInventoryWidget->OnAddItem.BindUObject(PalInventory, &UPalInventory::AddItem);
						}
					}
				}
				if (!pPlayer->AddToViewPort(BuildActionWidgetCom->GetActionWidget()))
				{
					pPlayer->RemoveFromViewPort(BuildActionWidgetCom->GetActionWidget());
					BuildActionWidgetCom->DeleteActionWidget();
				}
			}
			else
			{
				pPlayer->RemoveFromViewPort(BuildActionWidgetCom->GetActionWidget());
				BuildActionWidgetCom->DeleteActionWidget();
			}
		}
	}
}

void APalBaseCamp_V2::OnInteractionEnd_Implementation(ACharacter* pOther)
{
	if (!pOther)
		return;
	if (!GetBuildingProgress()->IsBuildingEnd())
	{
		GetBuildingProgress()->StopBuilding(pOther);
	}
	//else if (BuildActionWidgetCom && BuildActionWidgetCom->GetActionWidget() && PalInventory)
	//{
	//	if (UPalBoxWidget* PalBoxWidget = Cast< UPalBoxWidget>(BuildActionWidgetCom->GetActionWidget()))
	//	{
	//		if (UPalItemInventoryWidget* PalItemInventoryWidget = PalBoxWidget->GetPalItemInventoryWidget())
	//		{
	//			PalInventory->OnSlotChanged.RemoveAll(PalItemInventoryWidget);
	//			PalItemInventoryWidget->OnWidgetOpen.RemoveAll(PalInventory);
	//			PalItemInventoryWidget->OnSlotSwap.RemoveAll(PalInventory);
	//		}
	//	}
	//	//BuildActionWidgetCom->DeleteActionWidget();
	//}
}


