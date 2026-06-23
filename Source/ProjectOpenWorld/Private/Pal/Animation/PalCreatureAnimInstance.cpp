#include "Pal/Animation/PalCreatureAnimInstance.h"
#include "Pal/Character/PalBaseCreature.h"
#include "Pal/Data/PalJobTypes.h"
#include "Pal/AssetUserData/PalChangeToolMesh.h"

void UPalCreatureAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerPalCreature = Cast<APalBaseCreature>(TryGetPawnOwner());
}

void UPalCreatureAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UPalCreatureAnimInstance::OnChangeWorkCommand(const FPalWorkCommand& Job)
{
	//if (CurrenJobType == Job.JobType)
	//	return;
	CurrenJobType = Job.JobType;
	WorkAnimation = nullptr;
	if(WorkAnim.Contains(CurrenJobType))
		WorkAnimation = *WorkAnim.Find(CurrenJobType);
}

void UPalCreatureAnimInstance::OnStartWork()
{
	if (WorkAnimation)
	{
		bActionStarted = true;
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
	if (bActionStarted)
	{
		bActionStarted = false;
		WorkAnimation = nullptr;
		OnToolMeshChanged.Broadcast(nullptr, NAME_None, FTransform{});
	}
}
