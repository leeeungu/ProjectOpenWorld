#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MontageQueueInterface.generated.h"

class UAnimInstance;
class UAnimMontage;
class UBaseAnimInstance;

UINTERFACE(MinimalAPI)
class UMontageQueueInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IMontageQueueInterface
{
	GENERATED_BODY()
public:
	virtual UAnimMontage* GetMontage() const { return nullptr; }
	virtual void MontageStartEvent(UBaseAnimInstance* BaseAnim, UAnimMontage* Montage) {}
	virtual void MontageBlendingEvent(UBaseAnimInstance* BaseAnim, UAnimMontage* Montage, bool bInterrupted) {}
};
