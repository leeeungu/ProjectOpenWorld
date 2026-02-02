#include "Item/Actor/ItemActor.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Item/Widget/ItemInteractionToolTipWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Player/Character/BasePlayer.h"
#include "Interaction/Component/InteractionComponent.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"

AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	//ItemCollision = CreateDefaultSubobject< USphereComponent>(TEXT("ItemCollision"));
	ItemSkeletalMesh = CreateDefaultSubobject< USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SetRootComponent(ItemSkeletalMesh);
	ItemSkeletalMesh->SetSimulatePhysics(true);
	ItemSkeletalMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemSkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);;
	ItemSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	ItemSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	ItemSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ItemSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap);
	ItemSkeletalMesh->SetGenerateOverlapEvents(true);
	ItemSkeletalMesh->SetLinearDamping(5.f);
	ItemSkeletalMesh->SetAngularDamping(5.f);

	//Script/Engine.StaticMesh'/Game/Pal/Model/Weapon/PalSphere/Mesh/SM_PalSphere.SM_PalSphere'
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("/Game/Pal/Model/Weapon/PalSphere/Mesh/SK_PalSphere.SK_PalSphere"));
	if(MeshObj.Succeeded())
	{
		ItemSkeletalMesh->SetSkeletalMesh(MeshObj.Object);
	}
	ItemWidget = CreateDefaultSubobject< UWidgetComponent>(TEXT("Widget"));
	ItemWidget->SetupAttachment(GetRootComponent());
	//Script/UMGEditor.WidgetBlueprint'/Game/Item/Widget/WBP_ItemInteraction.WBP_ItemInteraction'
	static ConstructorHelpers::FClassFinder<UUserWidget> InteractionWidget(TEXT("/Game/Item/Widget/WBP_ItemInteraction.WBP_ItemInteraction_C"));
	if (InteractionWidget.Succeeded())
	{
		ToolTipWidgetClass = InteractionWidget.Class;
	}
	ItemWidget->SetCollisionProfileName(TEXT("NoCollision"));
	ItemWidget->SetWidgetClass(ToolTipWidgetClass);
	ItemWidget->SetWidgetSpace(EWidgetSpace::Screen);
}

void AItemActor::BeginPlay()
{
	ToolTipWidget = CreateWidget(GetWorld(), ToolTipWidgetClass);
	
	Super::BeginPlay();

	//ItemSkeletalMesh->SetLinearDamping(5.f);
	ItemSkeletalMesh->SetAngularDamping(10.f);
	if (ItemWidget && ToolTipWidget)
		ItemWidget->SetWidget(ToolTipWidget.Get());
}


void AItemActor::Init(FName NewItemID, int Count)
{
	ItemID = NewItemID;
	itemCount = Count;
	if (!UItemDataSubsystem::IsValidInstance())
		return;
	const FPalStaticItemDataStruct* ItemDataStruct{};
	UItemDataSubsystem::GetPalStaticItemDataPtr(ItemID, ItemDataStruct);
	if (!ItemDataStruct)
		return;
	//ItemData = ItemDataStruct->ItemPrimaryDataAsset;
	if (ItemSkeletalMesh)
	{
		//ItemSkeletalMesh->SetSkeletalMesh(ItemData->GetItemSkeletalMesh());
	}
		
}

void AItemActor::OnBeginDetected_Implementation(ACharacter* pOther)
{
	if (ToolTipWidget )
	{
		Cast<UItemInteractionToolTipWidget>(ToolTipWidget.Get())->SetItemName(ItemID);
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
	ABasePlayer* Player = Cast<ABasePlayer>(pOther);
	if (Player)
	{
		Player->GetInteractionComponent()->OnInteractionCompleted();
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
	ABasePlayer* Player = Cast<ABasePlayer>(pOther);
	if (Player)
	{
		Player->GetInteractionComponent()->OnInteractionCompleted();
	}
	if (Inventory->AddItem(ItemID, itemCount))
	{
		Destroy();
	}
}


void AItemActor::OnTransportRegister_Implementation(AActor* Other)
{
	if (Transport && Transport.Get() )
		return;
	ACharacter* pTarget = Cast<ACharacter>(Other);
	if (!pTarget)
		return;

	TransportState = ETransportState::Transport;
	Transport = Other;
	ItemSkeletalMesh->SetSimulatePhysics(false);
	AttachToComponent(pTarget->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Socket_Transport"));
}

void AItemActor::OnTransportUnRegister_Implementation(AActor* Other)
{
	if (!Transport || !Transport.Get())
		return;
	TransportState = ETransportState::NotTransport;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemSkeletalMesh->SetSimulatePhysics(true);
	Transport = nullptr;
}

ETransportState AItemActor::GetTransportState_Implementation()
{
	return TransportState;
}


void AItemActor::NewGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	//UE_LOG(LogTemp, Error, TEXT("AItemActor::NewGenerateWorldEvent TODO"));
	ItemSkeletalMesh->SetSimulatePhysics(true);

}

void AItemActor::DelGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	//UE_LOG(LogTemp, Error, TEXT("AItemActor::DelGenerateWorldEvent TODO"));
	ItemSkeletalMesh->SetSimulatePhysics(false);
	ItemSkeletalMesh->BodyInstance.ClearForces();
}

UPrimitiveComponent* AItemActor::GetItemCollision() const
{
	return ItemSkeletalMesh.Get();
}






