#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "PalBoxDDO.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UPalBoxDDO : public UDragDropOperation
{
	GENERATED_BODY()
public:
	UPalBoxDDO();
	UPalBoxDDO(bool InIsFromInventory, int32 InIndex)
		: IsFromInventory(InIsFromInventory)
		, Index(InIndex)
	{
	}
	UPROPERTY()
	bool IsFromInventory{ false };
	UPROPERTY()
	int32 Index{-1};
};
