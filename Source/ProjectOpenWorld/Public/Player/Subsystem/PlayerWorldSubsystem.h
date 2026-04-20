#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PlayerWorldSubsystem.generated.h"

class APlayerPreviewPawn;

UCLASS()
class PROJECTOPENWORLD_API UPlayerWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TWeakObjectPtr< APlayerPreviewPawn> PlayerPreviewActor{};

public:

	void RegisterPreviewActor(APlayerPreviewPawn* Actor);
	APlayerPreviewPawn* GetPreviewActor() const;

};
