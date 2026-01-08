#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMetaData.h"
#include "Animation/AnimInstance.h"
#include "AMD_MontageChangeEvent.generated.h"

UCLASS(Abstract)
class PROJECTOPENWORLD_API UAMD_MontageChangeEvent : public UAnimMetaData
{
	GENERATED_BODY()
public:
	virtual void StartEvent(UAnimInstance* Animinstance) PURE_VIRTUAL(UAMD_MontageChangeEvent::StartEvent, );
	virtual void EndEvent(UAnimInstance* Animinstance) PURE_VIRTUAL(UAMD_MontageChangeEvent::EndEvent, );
};
