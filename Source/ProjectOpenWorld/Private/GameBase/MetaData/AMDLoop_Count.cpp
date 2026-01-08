#include "GameBase/MetaData/AMDLoop_Count.h"

void UAnimLoopObject_Count::Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData)
{
	Super::Initialize(Animinstance, MetaData);
	LoopCount = 0;
	bLoop = false;
	//if (UAMDLoop* LoopData = MetaData->FindMetaDataByClass<UAMDLoop>())
	if (UAMDLoop_Count* Data = Cast< UAMDLoop_Count>(LoopMetaData))
	{
		if (Data->GetLoopCount() - 1 > 0)
		{
			LoopCount = Data->GetLoopCount() - 1;
			bLoop = true;
		}
	}
}

void UAnimLoopObject_Count::UpdateBlendOut()
{
	bLoop = false;
	if (LoopCount > 0)
	{
		LoopCount--;
		bLoop = true;
		return;
	}
	LoopCount = 0;
}

UAMDLoop_Count::UAMDLoop_Count(const FObjectInitializer& ObjectInitializer) : 
	UAMDLoop(ObjectInitializer)
{
	InstanceClass = UAnimLoopObject_Count::StaticClass();
}