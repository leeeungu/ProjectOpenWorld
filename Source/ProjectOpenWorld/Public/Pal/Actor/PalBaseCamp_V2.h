// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Actor/BuildingActor.h"
#include "PalBaseCamp_V2.generated.h"

class UPalWorkableSearchComponent;
class UPalWorkCommander;
class UPalInventory;
class ACharacter;
class UPalSpawnerComponent;

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

	UPROPERTY(VisibleAnywhere, Category = "PalStorage")
	TObjectPtr<UPalSpawnerComponent> PalSpawnerComponent{};
	
public:
	APalBaseCamp_V2();
	UFUNCTION(BlueprintPure)
	FORCEINLINE UPalWorkCommander* GetPalWorkCommander() const { return PalWorkCommander; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE UPalSpawnerComponent* GetPalSpawnerComponent() const { return PalSpawnerComponent; }
	

	UFUNCTION(BlueprintPure)
	UPalInventory* GetPalInventory() const { return PalInventory; }
	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override;

protected:
	UFUNCTION()
	void OnDelActionWidget(UUserWidget* ActionWidget);
	virtual void BeginPlay() override;

	UFUNCTION()
	void SpawnPal(int32 Index, AActor* Pre);
};
