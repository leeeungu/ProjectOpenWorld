#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerPreviewAnchor.generated.h"

UCLASS()
class PROJECTOPENWORLD_API APlayerPreviewAnchor : public AActor
{
	GENERATED_BODY()
	
public:	
	APlayerPreviewAnchor();

protected:
	virtual void BeginPlay() override;

public:	

};
