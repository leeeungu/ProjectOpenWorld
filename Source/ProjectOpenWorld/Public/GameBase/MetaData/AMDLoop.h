#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMetaData.h"
#include "Animation/AnimInstance.h"
#include "AMDLoop.generated.h"

class UAnimLoopObject;

UCLASS(Abstract)
class PROJECTOPENWORLD_API UAMDLoop : public UAnimMetaData
{
	GENERATED_BODY()
protected:
	TSubclassOf<UAnimLoopObject> InstanceClass{};
public:
	TSubclassOf<UAnimLoopObject> GetInstanceClass() const;
	UAnimLoopObject* CreateInstanceObject(UWorld* WorldInstance) const;
};

UCLASS(Abstract)
class PROJECTOPENWORLD_API UAnimLoopObject : public UObject
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<UAnimInstance> OwnerAniminstance{};
	UPROPERTY()
	TObjectPtr<UAMDLoop> LoopMetaData{};
	bool bLoop{};
public:
	virtual void UpdateLoop(float DeltaTime = 0) {}
	virtual void UpdateBlendOut() {}
	virtual void Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData);
	virtual bool IsLoop() const { return bLoop; }
	virtual void EndLoop() {}
};

