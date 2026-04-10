#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalItemSlotData.generated.h"

USTRUCT(BlueprintType)
struct FPalItemSlotData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemSlotData")
	int32 MaxStackCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemSlotData")
	float Weight = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemSlotData")
	int32 SortID = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemSlotData")
	bool bTrashable = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemSlotData")
	bool bStackable = true;
};

