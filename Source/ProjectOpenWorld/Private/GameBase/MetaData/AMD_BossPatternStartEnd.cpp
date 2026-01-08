#include "GameBase/MetaData/AMD_BossPatternStartEnd.h"
#include "Pal/Character/BossMonster.h"
#include "Pal/Component/PalPatternComponent.h"

//if (MeshComp)
//{
//	ABossMonster* Boss = Cast<ABossMonster>(MeshComp->GetOwner());
//	if (Boss)
//	{
//		if (PatternState == EPalPatternEventType::PatternEnd)
//			Boss->GetPatternComponent()->EndPattern(Index);
//		else if (PatternState == EPalPatternEventType::PatternStart)
////			Boss->GetPatternComponent()->StartPattern(Index);
//	}
////}

void UAMD_BossPatternStartEnd::StartEvent(UAnimInstance* Animinstance)
{
	ABossMonster* Boss = Cast<ABossMonster>(Animinstance->TryGetPawnOwner());
	if (!Boss)
		return;
	InitializeIndex();
	Boss->GetPatternComponent()->StartPattern(Index);
}

void UAMD_BossPatternStartEnd::EndEvent(UAnimInstance* Animinstance)
{
	ABossMonster* Boss = Cast<ABossMonster>(Animinstance->TryGetPawnOwner());
	if (!Boss)
		return;
	InitializeIndex();
	UE_LOG(LogTemp, Warning, TEXT("UAMD_BossPatternStartEnd::EndEvent Index : %d"), Index);//
	Boss->GetPatternComponent()->EndPattern(Index);
}

void UAMD_BossPatternStartEnd_Anubis::InitializeIndex()
{
	Index = static_cast<uint8>(PatternType);
}
