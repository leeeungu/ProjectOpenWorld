#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AnimActionPlayType.generated.h"

UENUM(BlueprintType)
enum class EAnimActionPlayType : uint8
{
	None UMETA(DisplayName = "None"),
	OneShot UMETA(DisplayName = "One Shot"),
	StartLoopEnd UMETA(DisplayName = "Start Loop End"),
	//Combo UMETA(DisplayName = "Combo"),
	//ChargeRelease UMETA(DisplayName = "Charge Release"),
};

UENUM(BlueprintType)
enum class EAnimationSectionType : uint8
{
	None UMETA(DisplayName = "None"),
	Start UMETA(DisplayName = "Start"),
	Loop UMETA(DisplayName = "Loop"),
	End UMETA(DisplayName = "End"),
};
