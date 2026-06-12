#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PalSlotSelectedInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPalSlotSelectedInterface : public UInterface
{
	GENERATED_BODY()
};

class IPalSlotSelectedInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category="SlotSelected")
	void OnSlotSelected(int32 Index, AActor* SelectedCreature);
};
