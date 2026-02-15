#pragma once

#include "CoreMinimal.h"
#include "GameBase/MetaData/AMDLoop.h"
#include "AMD_LaunchCharacterEvent.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UAMD_LaunchCharacterEvent : public UAMDLoop
{
	GENERATED_BODY()
protected:

public:
	//UAMD_LaunchCharacterEvent(const FObjectInitializer& ObjectInitializer);

};
//UCLASS(Abstract)
//class PROJECTOPENWORLD_API UAnimLoopObject_Launch : public UAnimLoopObject
//{
//	GENERATED_BODY()
//protected:
//	UPROPERTY()
//	TObjectPtr<UAnimInstance> OwnerAniminstance{};
//	UPROPERTY()
//	TObjectPtr<UAMDLoop> LoopMetaData{};
//	bool bLoop{};
//public:
//	virtual void UpdateLoop(float DeltaTime = 0) {}
//	virtual void UpdateBlendOut() {}
//	virtual void Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData);
//	virtual bool IsLoop() const { return bLoop; }
//	virtual void EndLoop() {}
//};
//
