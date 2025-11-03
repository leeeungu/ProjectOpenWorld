#include "Creature/Component/CreatureAction_Building.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "Building/Actor/BuildingActor.h"
#include "Building/Component/BuildingProgress.h"
#include "Creature/Interface/CreatureMessageInterface.h"
#include "Components/StaticMeshComponent.h"


UCreatureAction_Building::UCreatureAction_Building()
{
	Action = ECreatureActionType::Action_Buliding;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

void UCreatureAction_Building::ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject)
{
	// 호출시 마다 TargetBuilding가 변경될 수 있음
	if (TargetBuilding != TargetObject)
	{
		bActionStart = false;
	}
	TargetBuilding = Cast< ABuildingActor>(TargetObject);
	if (!TargetBuilding || !OwnerController || bActionStart)
		return;
	bActionStart = true;
	FAIMoveRequest MoveReq(TargetBuilding.Get());
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetAcceptanceRadius(100.0f);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(true); 
	FNavPathSharedPtr OutPath{};
	OwnerController->MoveTo(MoveReq, &OutPath);
	TargetBuilding->GetBuildingProgress()->onBuildingEnd.AddDynamic(this, &UCreatureAction_Building::BuildEnd);
	if (!OutPath.IsValid())
	{
		FinishMoved(FAIRequestID::InvalidRequest, EPathFollowingResult::Success);
	}
}

void UCreatureAction_Building::ActionEnd_Implementation()
{
	if (!bActionStart || !bBuildingStart)
		return;

	BuildEnd();
}

void UCreatureAction_Building::BeginPlay()
{
	Super::BeginPlay(); 
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UCreatureAction_Building::FinishMoved);
		if (ACharacter* Character = Cast<ACharacter>(OwnerController->GetPawn()))
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
}

void UCreatureAction_Building::FinishMoved(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (!bActionStart || !TargetBuilding)
		return;

	//{
	//	if (MeshComponent)
	//		MeshComponent->SetVisibility(false);
	//	//ICreatureMessageInterface::Execute_ReceiveMessage(GetOwner(), EMessageType::NONE, nullptr, nullptr);
	//	bActionStart = false;
	//	bBuildingStart = false;
	//}

	if (EPathFollowingResult::Type::Success == Result &&  !bBuildingStart && bActionStart && !TargetBuilding->GetBuildingProgress()->IsBuildingEnd())
	{
		TargetBuilding->GetBuildingProgress()->StartBuilding();
		if (MeshComponent)
			MeshComponent->SetVisibility(true);
		bBuildingStart = true;
	}
	else
	{
		if (MeshComponent)
			MeshComponent->SetVisibility(false);
		ICreatureMessageInterface::Execute_ReceiveMessage(GetOwner(), EMessageType::NONE, nullptr, nullptr);
		bActionStart = false;
		bBuildingStart = false;
	}
}

void UCreatureAction_Building::BuildEnd()
{
	if (TargetBuilding && bBuildingStart)
	{
		TargetBuilding->GetBuildingProgress()->StopBuilding();
		MeshComponent->SetVisibility(false);
		bBuildingStart = false;
		bActionStart = false;
		ICreatureMessageInterface::Execute_ReceiveMessage(GetOwner(), EMessageType::NONE, TargetBuilding.Get(), nullptr);
		TargetBuilding->GetBuildingProgress()->onBuildingEnd.RemoveDynamic(this, &UCreatureAction_Building::BuildEnd);
		TargetBuilding = nullptr;
	}
}
