#include "Pal/Animation/PalCreatureAnimInstance.h"
#include "Pal/Character/PalBaseCreature.h"
#include "Pal/Component/PalJobComponent.h"
#include "Pal/Data/PalJobTypes.h"
#include "Pal/AssetUserData/PalChangeToolMesh.h"

void UPalCreatureAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerPalCreature = Cast<APalBaseCreature>(TryGetPawnOwner());
	if (!OwnerPalCreature)
		return;
	JobComponent = OwnerPalCreature->GetJobComponent();
}

void UPalCreatureAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!OwnerPalCreature || !JobComponent)
	{
		return;
	}

	if (CurrenJobType == EPalJobType::Transport)
	{
		MoveSpeed = FMath::Clamp(MoveSpeed, 0.0f, 0.46f);
	}
}

void UPalCreatureAnimInstance::OnChangeWorkCommand(const FPalWorkCommand& Job)
{
	if (!JobComponent)
		return;
	CurrenJobType = JobComponent->GetCurrentJobType();
	switch (CurrenJobType)
	{
	case EPalJobType::Architecture:
	{
		WorkAnimation = ArchitectureAnim;
		return;
	}
	case EPalJobType::Mining:
	{
		WorkAnimation = MiningAnim;
		return;
	}
	case EPalJobType::Lumbering:
	{
		break;
	}
	case EPalJobType::Transport:
	{
		WorkAnimation = TransportAnim;
		return;
	}
	default:
		break;
	}
	WorkAnimation = nullptr;
}

void UPalCreatureAnimInstance::OnStartWork()
{
	bActionStarted = true;
	if (WorkAnimation)
	{
		UPalChangeToolMesh* ToolMesh = Cast< UPalChangeToolMesh>(WorkAnimation->GetAssetUserDataOfClass(UPalChangeToolMesh::StaticClass()));
		if (!ToolMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("AnimInstance Has Not UPalChangeToolMesh : %s"), *GetName());
			return;
		}
		OnToolMeshChanged.Broadcast(ToolMesh->GetToolMesh(), ToolMesh->GetToolSocketName(), ToolMesh->GetToolSocketTransform());
	}
}

void UPalCreatureAnimInstance::OnEndWork()
{
	bActionStarted = false;
	WorkAnimation = nullptr;
	OnToolMeshChanged.Broadcast(nullptr, NAME_None, FTransform{});
}
