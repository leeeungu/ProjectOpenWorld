#include "Item/Actor/ItemActor.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Item/DataAsset/ItemPrimaryDataAsset.h"
#include "Item/Widget/ItemInteractionToolTipWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Player/Character/BasePlayer.h"
#include "Interaction/Component/InteractionComponent.h"

AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	ItemCollision = CreateDefaultSubobject< USphereComponent>(TEXT("ItemCollision"));
	SetRootComponent(ItemCollision);
	ItemCollision->SetSimulatePhysics(true);
	ItemCollision->SetCollisionProfileName(TEXT("NoCollision"));
	ItemCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);;
	ItemCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	ItemCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	ItemCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ItemCollision->SetLinearDamping(5.f);
	ItemCollision->SetAngularDamping(5.f);

	ItemMesh = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("SkeletalMesh"));
	ItemMesh->SetupAttachment(GetRootComponent());
	ItemMesh->SetCollisionProfileName(TEXT("NoCollision"));
	//Script/Engine.StaticMesh'/Game/Pal/Model/Weapon/PalSphere/Mesh/SM_PalSphere.SM_PalSphere'
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Game/Pal/Model/Weapon/PalSphere/Mesh/SM_PalSphere.SM_PalSphere"));
	if(MeshObj.Succeeded())
	{
		ItemMesh->SetStaticMesh(MeshObj.Object);
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
	ItemMesh->SetAngularDamping(5.f);
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
	if (Inventory->AddItem(ItemData.Get(), itemCount))
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
	ItemMesh->SetSimulatePhysics(false);
	AttachToComponent(pTarget->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Socket_Transport"));
}

void AItemActor::OnTransportUnRegister_Implementation(AActor* Other)
{
	if (!Transport || !Transport.Get())
		return;
	TransportState = ETransportState::NotTransport;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemMesh->SetSimulatePhysics(true);
	Transport = nullptr;
}

ETransportState AItemActor::GetTransportState_Implementation()
{
	return TransportState;
}






