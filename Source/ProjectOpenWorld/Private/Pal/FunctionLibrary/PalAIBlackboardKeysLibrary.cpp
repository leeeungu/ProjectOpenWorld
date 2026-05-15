#include "Pal/FunctionLibrary/PalAIBlackboardKeysLibrary.h"

namespace PalAIBB
{
    static const FName TargetActor = TEXT("TargetActor");
    static const FName TargetLocation = TEXT("TargetLocation");
    static const FName MonsterState = TEXT("MonsterState");
}

FName UPalAIBlackboardKeysLibrary::GetBBTargetActorKey() { return PalAIBB::TargetActor; }
FName UPalAIBlackboardKeysLibrary::GetBBTargetLocationKey() { return PalAIBB::TargetLocation; }
FName UPalAIBlackboardKeysLibrary::GetBBMonsterStateKey() { return PalAIBB::MonsterState; }
