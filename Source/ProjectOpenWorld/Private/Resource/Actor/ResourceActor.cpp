#include "Resource/Actor/ResourceActor.h"
#include "Item/Actor/ItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"

AResourceActor::AResourceActor()
{
	PrimaryActorTick.bCanEverTick = true;
	ResourceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(ResourceMesh);
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
	FVector Location = GetActorLocation();
	AItemActor* Item = Cast< AItemActor>(GetWorld()->SpawnActor(AItemActor::StaticClass(), &Location, 0, Param));
	int idx = rand() % ItemData.Num();
	Item->Init(ItemData[idx], ItemData.Num() + 2);
	ExtractCount--;
	if (ExtractCount <= 0)
	{
		for(TWeakObjectPtr<UObject> Objects : InteractionList)
		{
			if (ACharacter* Character = Cast<ACharacter>(Objects.Get()))
			{
				OnInteractionEnd_Implementation(Character);
			}
		}
		
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
	if (InteractionList.Find(pOther))
	{
		SpawnRandomItem();
		//UE_LOG(LogTemp, Log, TEXT("AResourceActor :: UpdateResource by %s"), *pOther->GetName());
	}
}

void AResourceActor::OnBeginDetected_Implementation(ACharacter* pOther)
{
}

void AResourceActor::OnEndDetected_Implementation(ACharacter* pOther)
{
}

void AResourceActor::OnInteractionStart_Implementation(ACharacter* pOther)
{
	InteractionList.Add(pOther);
}

void AResourceActor::OnInteraction_Implementation(ACharacter* pOther)
{
}

void AResourceActor::OnInteractionEnd_Implementation(ACharacter* pOther)
{
	InteractionList.Remove(pOther);
}

void AResourceActor::OnInteractionCanceled_Implementation(ACharacter* pOther)
{
}


