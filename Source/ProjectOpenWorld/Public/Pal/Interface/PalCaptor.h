#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PalCaptor.generated.h"

class APalBaseCharacter;

UINTERFACE(MinimalAPI)
class UPalCaptor : public UInterface
{
	GENERATED_BODY()
};

class IPalCaptor
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Pal|Capture")
	void TryCaptureCreature(TSubclassOf<APalBaseCharacter> ActorClass);
};
