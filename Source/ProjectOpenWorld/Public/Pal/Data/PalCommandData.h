#pragma once

#include "CoreMinimal.h"
#include "PalCommandData.generated.h"

UENUM(BlueprintType)
enum class EPalCommandKind : uint8
{
	None,
	Move,
	Attack,
	Interact,
	Work,
};

USTRUCT(BlueprintType)
struct FPalCommand
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPalCommandKind CommandKind{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* pInstigatorActor{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* pTarget{};
};