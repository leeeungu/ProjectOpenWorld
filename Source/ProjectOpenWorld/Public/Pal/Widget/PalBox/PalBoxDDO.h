#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "PalBoxDDO.generated.h"

class UPalSpawnerComponent;
class UPalStorageComponent;

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
	TObjectPtr<UPalStorageComponent> PalStorageComponent{};
	TObjectPtr< UPalSpawnerComponent> PalSpawnerComponent{};
	UPROPERTY()
	bool IsFromInventory{ false };
	UPROPERTY()
	int32 Index{-1};
};
