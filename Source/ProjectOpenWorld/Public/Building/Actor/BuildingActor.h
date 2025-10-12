// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/BaseBuilding.h"
#include "Interaction/InteractionInterface.h"
#include "BuildingActor.generated.h"

class UBuildingStateWidget;

UCLASS()
class PROJECTOPENWORLD_API ABuildingActor : public ABaseBuilding, public IInteractionInterface
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public: // IInteractionInterface
	virtual void OnBeginDetected_Implementation(APlayerController* pPlayer) override;
	virtual void OnEndDetected_Implementation(APlayerController* pPlayer) override;
	virtual void OnInteractionStart_Implementation(APlayerController* pPlayer) override;
	virtual void OnInteraction_Implementation() override;
	virtual void OnInteractionEnd_Implementation(APlayerController* pPlayer) override;
};
