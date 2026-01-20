#include "Resource/Actor/ResourceActor.h"
#include "Item/Actor/ItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"
#include "Resource/Interface/ResourceInterface.h"

AResourceActor::AResourceActor()
{
	PrimaryActorTick.bCanEverTick = true;
	ResourceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(ResourceMesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Game/Pal/Model/Prop/MapMesh/Mesh/SK_Rock_A.SK_Rock_A"));
	if (MeshObj.Succeeded())
	{
		ResourceMesh->SetStaticMesh(MeshObj.Object);
	}
	ResourceMesh->SetCollisionProfileName(TEXT("BlockAll"));
	ResourceMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap);
	//Script/Engine.StaticMesh'/Game/Pal/Model/Prop/MapMesh/Mesh/SK_Rock_A.SK_Rock_A'
	
}

void AResourceActor::BeginPlay()
{
	Super::BeginPlay();
}

void AResourceActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (onExtractEnd.IsBound())
		onExtractEnd.Broadcast();
}

void AResourceActor::SpawnRandomItem()
{
	if (ItemData.Num() == 0)
		return;
	FActorSpawnParameters Param{};
	Param.Owner = this;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FVector Location = GetActorLocation() + FVector (0,0,ResourceMesh->GetStaticMesh()->GetBoundingBox().GetSize().Z);
	if (!SpawnClass)
		SpawnClass = AItemActor::StaticClass();
	AItemActor* Item = Cast< AItemActor>(GetWorld()->SpawnActor(SpawnClass, &Location, 0, Param));
	if (!Item)
		return;
	FVector Force = FVector(0, 0, 1);
	Force.X = FMath::RandRange(-0.5f,0.5f);
	Force.Y = FMath::RandRange(-0.5f,0.5f);
	Item->GetItemCollision()->AddImpulse(Force * 1000);
	int idx = rand() % ItemData.Num();
	Item->Init(ItemData[idx], ItemData.Num() + 2);
	ExtractCount--;
	if (ExtractCount <= 0)
	{
		TArray< TWeakObjectPtr<UObject>> TempArray = InteractionList.Array();
		for(TWeakObjectPtr<UObject>& Objects : TempArray)
		{
			if (Objects.IsValid())
			{
				if (ACharacter* Character = Cast<ACharacter>(Objects.Get()))
				{
					IResourceInterface::Execute_EndResource(Character, this);
				}
			}
		}
		InteractionList.Empty();
		Destroy();
	}
}

EPalCommandKind AResourceActor::GetCommandKind_Implementation()
{
	return EPalCommandKind::Work;
}

uint8 AResourceActor::GetSubCommandType_Implementation()
{
	return static_cast<uint8>(ESubWorkType::Mining);
}

FPalCommand AResourceActor::GetCommand_Implementation()
{
	return UPalCommandFunctionLibrary::CommandMining(this, this);
}

bool AResourceActor::IsCommandFinished_Implementation()
{
	return ExtractCount <= 0;
}

void AResourceActor::UpdateResource(ACharacter* pOther)
{
	if (pOther  && InteractionList.Find(pOther))
	{
		SpawnRandomItem();
		//UE_LOG(LogTemp, Log, TEXT("AResourceActor :: UpdateResource by %s"), *pOther->GetName());
	}
}

void AResourceActor::OnBeginDetected_Implementation(ACharacter* pOther)
{
	UE_LOG(LogTemp, Log, TEXT("AResourceActor:beginDetected"));
}

void AResourceActor::OnEndDetected_Implementation(ACharacter* pOther)
{
}

void AResourceActor::OnInteractionStart_Implementation(ACharacter* pOther)
{
	if (!pOther || !pOther->Implements<UResourceInterface>())
		return;
	UE_LOG(LogTemp, Log, TEXT("AResourceActor:interactionStart"));
	InteractionList.Add(pOther);
	IResourceInterface::Execute_StartResource(pOther, this);
}

void AResourceActor::OnInteraction_Implementation(ACharacter* pOther)
{
}

void AResourceActor::OnInteractionEnd_Implementation(ACharacter* pOther)
{
	if (!pOther || !pOther->Implements<UResourceInterface>())
		return;
	InteractionList.Remove(pOther);
	IResourceInterface::Execute_StopResource(pOther, this);
}

void AResourceActor::OnInteractionCanceled_Implementation(ACharacter* pOther)
{
	if (!pOther || !pOther->Implements<UResourceInterface>())
		return;
	InteractionList.Remove(pOther);
	IResourceInterface::Execute_StopResource(pOther, this);
}

void AResourceActor::NewGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	UE_LOG(LogTemp, Error, TEXT("AResourceActor::NewGenerateWorldEvent TODO"));
}

void AResourceActor::DelGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	UE_LOG(LogTemp, Error, TEXT("AResourceActor::DelGenerateWorldEvent TODO"));
}


