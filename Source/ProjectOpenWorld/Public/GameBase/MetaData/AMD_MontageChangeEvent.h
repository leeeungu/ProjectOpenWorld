#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMetaData.h"
#include "Animation/AnimInstance.h"
#include "AMD_MontageChangeEvent.generated.h"


// 주위 : Metadata는 animation 끼리 전부 공유 하기에 cach용으로 변수 사용을 하면 안됨
UCLASS(Abstract)
class PROJECTOPENWORLD_API UAMD_MontageChangeEvent : public UAnimMetaData
{
	GENERATED_BODY()
public:
	virtual void StartEvent(UAnimInstance* Animinstance) PURE_VIRTUAL(UAMD_MontageChangeEvent::StartEvent, );
	virtual void EndEvent(UAnimInstance* Animinstance) PURE_VIRTUAL(UAMD_MontageChangeEvent::EndEvent, );
};
