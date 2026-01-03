#include "GameBase/MetaData/AMDLoop.h"

TSubclassOf<UAnimLoopObject> UAMDLoop::GetInstanceClass() const
{
	return InstanceClass;
}

UAnimLoopObject* UAMDLoop::CreateInstanceObject(UWorld* WorldInstance) const
{
	if (!WorldInstance )
		return nullptr;
	return NewObject< UAnimLoopObject>(WorldInstance, InstanceClass);
}


void UAnimLoopObject::Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData)
{
	OwnerAniminstance = Animinstance;
	LoopMetaData = MetaData;
	bLoop = false;
}

