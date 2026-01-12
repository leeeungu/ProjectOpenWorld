#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalEditorItemIconTableRow.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct PROJECTOPENWORLD_API FPalEditorItemIconTableRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	// The icon representing the item in the editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UTexture2D> Icon;
};