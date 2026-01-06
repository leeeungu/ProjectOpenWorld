#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameBase/Interface/MonsterInteractionInterface.h"
#include "SpawnSolider.generated.h"

class USplineComponent;
class ACharacter;

UCLASS()
class PROJECTOPENWORLD_API ASpawnSolider : public AActor, public IMonsterInteractionInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnSolider")
	TSubclassOf<ACharacter> SoliderClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnSolider")
	uint8 InteractionID{};
	FVector SpawnOffset{ 50,50,50 };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnSolider")
	FRotator SpawnRotator{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnSolider")
	TObjectPtr< USplineComponent> SplineComp{};
	FVector SpawnLocation{};
	FVector SpawnDirection{};
	uint8 SpawnCount{ 0 };
public:	
	ASpawnSolider();

protected:
	virtual void BeginPlay() override;

	ACharacter* SpawnSolider();
public:	
	virtual void Tick(float DeltaTime) override;

	// IMonsterInteractionInterface
	virtual void OnInteractionEvent_Implementation(ACharacter* TargetMonster) override;
	virtual uint8 GetInteractionID_Implementation() const override { return InteractionID; }
};
