#include "Item/Actor/ItemActor.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Item/DataAsset/ItemPrimaryDataAsset.h"
#include "Item/Widget/ItemInteractionToolTipWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	//Script/UMGEditor.WidgetBlueprint'/Game/Item/Widget/WBP_ItemInteraction.WBP_ItemInteraction'
	ConstructorHelpers::FClassFinder<UUserWidget> InteractionWidget(TEXT("/Game/Item/Widget/WBP_ItemInteraction.WBP_ItemInteraction_C"));
	if (InteractionWidget.Succeeded())
	{
		ToolTipWidgetClass = InteractionWidget.Class;
	}
	ItemMesh = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(ItemMesh);
	///Script/Engine.StaticMesh'/Game/Fab/Megascans/3D/Bulk_Bag_ukxkbesqa/Low/ukxkbesqa_tier_3.ukxkbesqa_tier_3'
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Game/Fab/Megascans/3D/Bulk_Bag_ukxkbesqa/Low/ukxkbesqa_tier_3.ukxkbesqa_tier_3"));
	if(MeshObj.Succeeded())
	{
		ItemMesh->SetStaticMesh(MeshObj.Object);
	}
	ItemWidget = CreateDefaultSubobject< UWidgetComponent>(TEXT("Widget"));
	ItemWidget->SetupAttachment(GetRootComponent());
	ItemWidget->SetWidgetClass(ToolTipWidgetClass);
	ItemWidget->SetWidgetSpace(EWidgetSpace::Screen);
}

void AItemActor::BeginPlay()
{
	ToolTipWidget = CreateWidget(GetWorld(), ToolTipWidgetClass);
	
	Super::BeginPlay();

	if (ItemWidget && ToolTipWidget)
		ItemWidget->SetWidget(ToolTipWidget.Get());
}

void AItemActor::Init(TObjectPtr<UItemPrimaryDataAsset> IteData, int Count)
{
	this->ItemData = IteData;
	this->itemCount = Count;
}

void AItemActor::OnBeginDetected_Implementation(APlayerController* pPlayer)
{
	//ItemData.LoadSynchronous();
	if (ToolTipWidget && ItemData)
	{
		Cast<UItemInteractionToolTipWidget>(ToolTipWidget.Get())->SetItemName(ItemData->GetItemName());
	}
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
	UInventoryComponent* Inventory = pPlayer->GetComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("AItemActor::OnInteractionStart - NoInventory"));
		return;
	}
	if (Inventory->AddItem(ItemData.Get(), itemCount))
		Destroy();
}






