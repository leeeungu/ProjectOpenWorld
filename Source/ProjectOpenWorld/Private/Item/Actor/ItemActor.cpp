#include "Item/Actor/ItemActor.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Item/DataAsset/ItemPrimaryDataAsset.h"
#include "Item/Widget/ItemInteractionToolTipWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"

AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	//Script/UMGEditor.WidgetBlueprint'/Game/Item/Widget/WBP_ItemInteraction.WBP_ItemInteraction'
	ConstructorHelpers::FClassFinder<UUserWidget> InteractionWidget(TEXT("/Game/Item/Widget/WBP_ItemInteraction.WBP_ItemInteraction_C"));
	if (InteractionWidget.Succeeded())
	{
		ToolTipWidgetClass = InteractionWidget.Class;
	}
	ItemSkeletalMesh = CreateDefaultSubobject< USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(ItemSkeletalMesh);
	ItemSkeletalMesh->SetSimulatePhysics(true);
	ItemSkeletalMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	ItemSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	ItemSkeletalMesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	ItemSkeletalMesh->SetCollisionResponseToChannel(ECC_Vehicle , ECollisionResponse::ECR_Ignore);
	ItemSkeletalMesh->SetCollisionResponseToChannel(ECC_Destructible , ECollisionResponse::ECR_Ignore);
	ItemSkeletalMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	ItemSkeletalMesh->SetLinearDamping(5.f);
	ItemSkeletalMesh->SetAngularDamping(5.f);


	///Script/Engine.StaticMesh'/Game/Fab/Megascans/3D/Bulk_Bag_ukxkbesqa/Low/ukxkbesqa_tier_3.ukxkbesqa_tier_3'
	//Script/Engine.SkeletalMesh'/Game/Pal/Model/Weapon/PalSphere/Mesh/SK_PalSphere.SK_PalSphere'
	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("/Game/Pal/Model/Weapon/PalSphere/Mesh/SK_PalSphere.SK_PalSphere"));
	if(MeshObj.Succeeded())
	{
		ItemSkeletalMesh->SetSkeletalMesh(MeshObj.Object);
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

	//ItemSkeletalMesh->SetLinearDamping(5.f);
	ItemSkeletalMesh->SetAngularDamping(5.f);
	if (ItemWidget && ToolTipWidget)
		ItemWidget->SetWidget(ToolTipWidget.Get());
}

void AItemActor::Init(TObjectPtr<UItemPrimaryDataAsset> IteData, int Count)
{
	this->ItemData = IteData;
	this->itemCount = Count;
}

void AItemActor::OnBeginDetected_Implementation(ACharacter* pOther)
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

void AItemActor::OnEndDetected_Implementation(ACharacter* pOther)
{
	if (ToolTipWidget)
	{
		ToolTipWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AItemActor::OnInteractionStart_Implementation(ACharacter* pOther)
{
	if (!pOther)
		return;
	UInventoryComponent* Inventory = pOther->GetController()->GetComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("AItemActor::OnInteractionStart - NoInventory"));
		return;
	}
	if (Inventory->AddItem(ItemData.Get(), itemCount))
		Destroy();
}






