#include "Pal/FunctionLibrary/PalAIBlackboardKeysLibrary.h"

namespace PalAIBB
{
    static const FName TargetActor = TEXT("TargetActor");
    static const FName TargetLocation = TEXT("TargetLocation");
    static const FName MonsterState = TEXT("MonsterState");
    static const FName JobType(TEXT("JobType"));
    static const FName JobTarget(TEXT("JobTarget"));
    static const FName JobLocation(TEXT("JobLocation"));
    static const FName AttackDistance(TEXT("AttackDistance"));
}

FName UPalAIBlackboardKeysLibrary::GetBBTargetActorKey() { return PalAIBB::TargetActor; }
FName UPalAIBlackboardKeysLibrary::GetBBTargetLocationKey() { return PalAIBB::TargetLocation; }
FName UPalAIBlackboardKeysLibrary::GetBBMonsterStateKey() { return PalAIBB::MonsterState; }
FName UPalAIBlackboardKeysLibrary::GetBBJobType() { return PalAIBB::JobType; }
FName UPalAIBlackboardKeysLibrary::GetBBJobTarget() { return PalAIBB::JobTarget; }
FName UPalAIBlackboardKeysLibrary::GetBBJobLocation() { return PalAIBB::JobLocation; }
