#include "Item/Actor/ItemActor.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Item/DataAsset/ItemPrimaryDataAsset.h"
#include "Item/Widget/ItemInteractionToolTipWidget.h"

AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	//Script/UMGEditor.WidgetBlueprint'/Game/Item/Widget/WBP_ItemInteraction.WBP_ItemInteraction'
	ConstructorHelpers::FClassFinder<UUserWidget> InteractionWidget(TEXT("/Game/Item/Widget/WBP_ItemInteraction.WBP_ItemInteraction_C"));
	if (InteractionWidget.Succeeded())
	{
		ToolTipWidgetClass = InteractionWidget.Class;
	}
}

void AItemActor::BeginPlay()
{
	ToolTipWidget = CreateWidget(GetWorld(), ToolTipWidgetClass);
	if (ToolTipWidget)
	{
		Cast<UItemInteractionToolTipWidget>(ToolTipWidget.Get())->SetItemName(ItemData->GetItemName());
	}
	Super::BeginPlay();
}

void AItemActor::OnBeginDetected_Implementation(APlayerController* pPlayer)
{
	if (ToolTipWidget)
	{
		ToolTipWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void AItemActor::OnEndDetected_Implementation(APlayerController* pPlayer)
{
	if (ToolTipWidget)
	{
		ToolTipWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AItemActor::OnInteractionStart_Implementation(APlayerController* pPlayer)
{
	if (!pPlayer)
		return;
	UE_LOG(LogTemp, Warning, TEXT("NopPlayer"));
	UInventoryComponent* Inventory = pPlayer->GetComponentByClass<UInventoryComponent>();
	if (!Inventory)
		return;
	UE_LOG(LogTemp, Warning, TEXT("NoInventory"));
	Inventory->AddItem(ItemData.Get());
	Destroy();
}






