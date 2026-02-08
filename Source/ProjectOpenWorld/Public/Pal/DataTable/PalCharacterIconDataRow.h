#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalCharacterIconDataRow.generated.h"

USTRUCT()
struct FPalCharacterIconDataRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalCharacterIconDataRow")
	TObjectPtr<UTexture2D> Icon{};
};
