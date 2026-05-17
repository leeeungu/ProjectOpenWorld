#include "Pal/Animation/PalCreatureAnimInstance.h"
#include "Pal/Character/PalBaseCreature.h"
#include "Pal/Component/PalJobComponent.h"


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
	if (JobComponent)
	{
		CurrenJobType = JobComponent->GetCurrentJobType();
		bActionStarted = JobComponent->IsWorking();
	}

	if (CurrenJobType == EPalJobType::Transport)
	{
		MoveSpeed = FMath::Clamp(MoveSpeed, 0.0f, 0.46f);
	}
}
