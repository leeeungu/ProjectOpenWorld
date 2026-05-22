// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Actor/BuildingActor.h"
#include "PalBaseCamp_V2.generated.h"

class UPalWorkableSearchComponent;
class UPalWorkCommander;
class UPalInventory;

UCLASS()
class PROJECTOPENWORLD_API APalBaseCamp_V2 : public ABuildingActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "SearchSphere")
	TObjectPtr<UPalWorkableSearchComponent> PalWorkableSearchComponent{};
	UPROPERTY(VisibleAnywhere, Category = "PalWork")
	TObjectPtr<UPalWorkCommander> PalWorkCommander{};
	UPROPERTY(VisibleAnywhere, Category = "PalWork")
	TObjectPtr<UPalInventory> PalInventory{};
	
public:
	APalBaseCamp_V2();
	UFUNCTION(BlueprintPure)
	UPalWorkCommander* GetPalWorkCommander() const { return PalWorkCommander; }
	UFUNCTION(BlueprintPure)
	UPalInventory* GetPalInventory() const { return PalInventory; }
protected:
	virtual void BeginPlay() override;
};
