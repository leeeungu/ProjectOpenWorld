#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalAIMonsterState.generated.h"

UENUM(BlueprintType)
enum class EPalAIMonsterState : uint8
{
	None        UMETA(DisplayName = "None"),
	Idle        UMETA(DisplayName = "Idle"),        // 대기/유휴
	Patrol      UMETA(DisplayName = "Patrol"),      // 순찰·배회
	Alert       UMETA(DisplayName = "Alert"),       // 뭔가 감지, 확인 단계
	Chase       UMETA(DisplayName = "Chase"),       // 타깃 추격
	Combat      UMETA(DisplayName = "Combat"),      // 교전 (공격 사거리 내)
	Search      UMETA(DisplayName = "Search"),      // 잃어버린 타깃 탐색
	Flee        UMETA(DisplayName = "Flee"),        // 도주
	Staggered   UMETA(DisplayName = "Staggered"),   // 피격 경직/CC
	Interact    UMETA(DisplayName = "Interact"),    // 먹기/잠/채집 등 비전투 상호작용
	Dead        UMETA(DisplayName = "Dead"),
};
//UENUM(BlueprintType)
//enum class EPalAIMonsterState : uint8
//{
//    None        UMETA(DisplayName = "None"),
//    Idle        UMETA(DisplayName = "Idle"),        // 대기/유휴
//    Patrol      UMETA(DisplayName = "Patrol"),      // 순찰·배회
//    Alert       UMETA(DisplayName = "Alert"),       // 뭔가 감지, 확인 단계
//    Chase       UMETA(DisplayName = "Chase"),       // 타깃 추격
//    Combat      UMETA(DisplayName = "Combat"),      // 교전 (공격 사거리 내)
//    Search      UMETA(DisplayName = "Search"),      // 잃어버린 타깃 탐색
//    Flee        UMETA(DisplayName = "Flee"),        // 도주
//    Staggered   UMETA(DisplayName = "Staggered"),   // 피격 경직/CC
//    Interact    UMETA(DisplayName = "Interact"),    // 먹기/잠/채집 등 비전투 상호작용
//    Dead        UMETA(DisplayName = "Dead"),
//};