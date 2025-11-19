#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SnapRule.generated.h"

USTRUCT(BlueprintType)
struct FSnapRule : public  FTableRowBase
{
    GENERATED_BODY()

    // 월드에 있는 메시 ptr
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* ParentMesh = nullptr;

    // ParentMesh pivot 기준 로컬 기준점 (붙이는 점)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform ParentAnchorLocal{};

    // ChildMesh pivot 기준 로컬 기준점 (붙는 점)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform ChildAnchorLocal{};

    // 나중을 위한 플래그/그룹 (지금은 안 써도 됨)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Flags = 0;
};