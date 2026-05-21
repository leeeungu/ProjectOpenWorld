#include "Item/Actor/ItemActor.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Item/Widget/ItemInteractionToolTipWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Player/Character/BasePlayer.h"
#include "Interaction/Component/InteractionComponent.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/FunctionLibrary/ItemFunctionLibrary.h"
#include "GameBase/Subsystem/SoundGameInstanceSubsystem.h"
#include "Item/Object/BaseItem.h"
#include "Item/DataAsset/ItemDataAsset.h"
#include "Item/Object/Fragment/ItemVisibleDataFragment.h"
#include "Pal/Interface/PalWorkerInterface.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"

AItemActor::AItemActor() : Super()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ItemCollision"));
	SetRootComponent(ItemCollision);
	ItemCollision->SetCollisionProfileName(TEXT("NoCollision"));
	ItemCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemCollision->SetCollisionObjectType(ECC_WorldDynamic);
	ItemCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ItemCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ItemCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ItemCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);
	ItemCollision->SetGenerateOverlapEvents(true);
	ItemCollision->SetLinearDamping(5.f);
	ItemCollision->SetAngularDamping(5.f);
	ItemCollision->SetSimulatePhysics(true);

	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	ItemSkeletalMesh->SetupAttachment(GetRootComponent());
	ItemSkeletalMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ItemSkeletalMesh->SetRelativeLocation(FVector(0.f, 0.f, -20.f));

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComponent"));
	ItemStaticMesh->SetupAttachment(GetRootComponent());
	ItemStaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ItemStaticMesh->SetRelativeLocation(FVector(0.f, 0.f, -20.f));

	ItemWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	ItemWidget->SetupAttachment(GetRootComponent());
	ItemWidget->SetCollisionProfileName(TEXT("NoCollision"));
	ItemWidget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> InteractionWidget(
		TEXT("/Game/Item/Widget/WBP_ItemInteraction.WBP_ItemInteraction_C"));
	if (InteractionWidget.Succeeded())
	{
		ToolTipWidgetClass = InteractionWidget.Class;
	}

	PickUpSound = EEffectSoundType::EST_PickUpItem;

	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("/Game/Pal/Model/Prop/Resource/CoinGold/Mesh/SM_CoinGold.SM_CoinGold"));
	if (StaticMeshAsset.Succeeded())
	{
		SetStaticMeshAsset(StaticMeshAsset.Object);
	}
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	if (ToolTipWidgetClass)
	{
		ToolTipWidget = CreateWidget<UUserWidget>(GetWorld(), ToolTipWidgetClass);
		if (ItemWidget && ToolTipWidget)
		{
			ItemWidget->SetWidget(ToolTipWidget);
		}
	}
	if(ItemInstance == nullptr)
		Init(TEXT("Money"), 1);
}

bool AItemActor::InitFromItem(UBaseItem* InItem)
{
	if (!InItem)
		return false;

	ItemInstance = DuplicateObject<UBaseItem>(InItem, this);
	if (!ItemInstance)
		return false;

	RefreshVisualFromItem();
	return true;
}

void AItemActor::Init(FName NewItemID, int32 Count)
{
	// 임시 호환용
	UBaseItem* NewItem = UItemFunctionLibrary::CreateBaseItem(NewItemID, Count, this);
	//NewObject<UBaseItem>(this);
	if (!NewItem)
		return;

	NewItem->SetItemID(NewItemID);
	NewItem->SetItemCount(Count);

	InitFromItem(NewItem);
}

UBaseItem* AItemActor::DuplicateItemFor(UObject* NewOuter) const
{
	if (!ItemInstance || !NewOuter)
		return nullptr;

	return DuplicateObject<UBaseItem>(ItemInstance, NewOuter);
}

FName AItemActor::GetItemID() const
{
	return ItemInstance ? ItemInstance->GetItemID() : NAME_None;
}

int32 AItemActor::GetItemCount() const
{
	return ItemInstance ? ItemInstance->GetItemCount() : 0;
}

void AItemActor::RefreshVisualFromItem()
{
	if (!ItemInstance || !UItemDataSubsystem::IsValidInstance())
		return;

	UItemDataAsset* ItemDataAsset = UItemDataSubsystem::GetPalItemDataAssetByName(ItemInstance->GetItemID());
	if (!ItemDataAsset)
		return;
	UItemVisibleDataFragment* VisibleDataFragment = Cast< UItemVisibleDataFragment>(ItemDataAsset->GetItemDataFragmentOfClass(UItemVisibleDataFragment::StaticClass()));
	SetMeshAsset(VisibleDataFragment);
}

void AItemActor::OnBeginDetected_Implementation(ACharacter* pOther)
{
	if (UItemInteractionToolTipWidget* ToolTip = Cast<UItemInteractionToolTipWidget>(ToolTipWidget))
	{
		UE_LOG(LogTemp, Log, TEXT("AItemActor::OnBeginDetected - Detected by %s"), *pOther->GetName());
		ToolTip->SetItemName(GetItemID());
		ToolTip->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void AItemActor::OnEndDetected_Implementation(ACharacter* pOther)
{
	if (ToolTipWidget)
	{
		ToolTipWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if (ABasePlayer* Player = Cast<ABasePlayer>(pOther))
	{
		Player->GetInteractionComponent()->OnInteractionCompleted();
	}
}

void AItemActor::OnInteractionStart_Implementation(ACharacter* pOther)
{
	if (!pOther || !ItemInstance || !pOther->GetController())
		return;
	
	UInventoryComponent* Inventory = pOther->GetController()->GetComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("AItemActor::OnInteractionStart - NoInventory"));
		return;
	}

	if (ABasePlayer* Player = Cast<ABasePlayer>(pOther))
	{
		Player->GetInteractionComponent()->OnInteractionCompleted();
	}
	Inventory->AddItem(DuplicateItemFor(Inventory));
	// if (Inventory->AddItemObject(DuplicateItemFor(Inventory)))

	USoundGameInstanceSubsystem::PlayEffectSound(PickUpSound, GetActorLocation());
	Destroy();
}

void AItemActor::OnTransportRegister_Implementation(AActor* Other)
{
	if (Transport)
		return;

	ACharacter* pTarget = Cast<ACharacter>(Other);
	if (!pTarget)
		return;

	TransportState = ETransportState::Transport;
	Transport = Other;

	ItemCollision->SetSimulatePhysics(false);
	AttachToComponent(pTarget->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Socket_Transport"));
}

void AItemActor::OnTransportUnRegister_Implementation(AActor* Other)
{
	if (!Transport)
		return;

	TransportState = ETransportState::NotTransport;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemCollision->SetSimulatePhysics(true);
	Transport = nullptr;
}

ETransportState AItemActor::GetTransportState_Implementation()
{
	return TransportState;
}

void AItemActor::NewGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	ItemCollision->SetSimulatePhysics(true);
}

void AItemActor::DelGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	ItemCollision->SetSimulatePhysics(false);
	ItemCollision->BodyInstance.ClearForces();
}

UPrimitiveComponent* AItemActor::GetItemCollision() const
{
	return ItemCollision;
}

void AItemActor::ResiterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)
{
	if(WorkerClass)
		WorkerClass->StartWorking();
}

void AItemActor::UnregisterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)
{
}

bool AItemActor::IsWorkable() const
{
	return 	TransportState == ETransportState::NotTransport;
}

FPalWorkCommand AItemActor::GetWorkCommand(AActor* InstigatorActor, AActor* Target) const
{
	UE_LOG(LogTemp, Warning, TEXT("Instigator : %s , target : %s"), *InstigatorActor->GetName(), *Target->GetName());
	return UPalCommandFunctionLibrary::WorkTransport(InstigatorActor, Target, InstigatorActor);
}

void AItemActor::SetMeshAsset(UItemVisibleDataFragment* VisibleDataFragment)
{
	if (!VisibleDataFragment)
		return;
	if (EItemVisibleMeshType::SkeletalMesh == VisibleDataFragment->GetVisibleMeshType())
	{
		SetSkeletalMeshAsset(VisibleDataFragment->GetSkeletalMesh());
	}
	else
	{
		SetStaticMeshAsset(VisibleDataFragment->GetStaticMesh());
	}
}

void AItemActor::SetStaticMeshAsset(UStaticMesh* StaticMesh)
{
	if (!ItemStaticMesh || !StaticMesh)
		return;
	ItemStaticMesh->SetStaticMesh(StaticMesh);
	ItemStaticMesh->SetVisibility(true);
	ItemSkeletalMesh->SetVisibility(false);
}

void AItemActor::SetSkeletalMeshAsset(class USkeletalMesh* SkeletalMesh)
{
	if (!ItemSkeletalMesh || !SkeletalMesh)
		return;
	ItemSkeletalMesh->SetSkeletalMesh(SkeletalMesh);
	ItemSkeletalMesh->SetVisibility(true);
	ItemStaticMesh->SetVisibility(false);
}
