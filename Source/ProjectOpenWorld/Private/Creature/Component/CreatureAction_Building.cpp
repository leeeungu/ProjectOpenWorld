#include "Creature/Component/CreatureAction_Building.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "Building/Actor/BuildingActor.h"
#include "Building/Component/BuildingProgress.h"
#include "Creature/Interface/CreatureMessageInterface.h"
#include "Components/StaticMeshComponent.h"
#include "Creature/Character/BaseCreature.h"


UCreatureAction_Building::UCreatureAction_Building()
{
	Action = ECreatureActionType::Action_Buliding;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

bool UCreatureAction_Building::ActionStart_Implementation(AActor* SendActor, AActor* TargetActor)
{
	// 호출시 마다 TargetBuilding가 변경될 수 있음
	if (TargetBuilding != TargetActor)
	{
		bActionStart = false;
	}
	TargetBuilding = Cast< ABuildingActor>(TargetActor);
	if (!TargetBuilding || bActionStart || TargetBuilding->GetBuildingProgress()->IsBuildingEnd())
		return false;

	bActionStart = true;
	TargetBuilding->GetBuildingProgress()->onBuildingEnd.AddDynamic(this, &UCreatureAction_Building::EndBuilding);
	TargetBuilding->GetBuildingProgress()->StartBuilding();
	if (MeshComponent)
		MeshComponent->SetVisibility(true);
	return true;
}

bool UCreatureAction_Building::ActionEnd_Implementation()
{
	if (bActionStart == false )
		return false;
	EndBuilding();
	return true;
}

void UCreatureAction_Building::BeginPlay()
{
	Super::BeginPlay();
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (MeshComponent)
		{
			MeshComponent->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("SWeapon_R"));
			MeshComponent->SetStaticMesh(BuildToolMesh);
			MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
			MeshComponent->SetVisibility(false);
		}
	}
}

void UCreatureAction_Building::EndBuilding()
{
	if (bActionStart == false)
		return;

	bActionStart = false;
	if (TargetBuilding)
	{
		TargetBuilding->GetBuildingProgress()->onBuildingEnd.RemoveDynamic(this, &UCreatureAction_Building::EndBuilding);
		TargetBuilding->GetBuildingProgress()->StopBuilding();
		TargetBuilding = nullptr;
	}
	if (MeshComponent)
		MeshComponent->SetVisibility(false);
	if (ABaseCreature* pOwner = Cast< ABaseCreature>(GetOwner()))
	{
		pOwner->ResetAction();
	}
}