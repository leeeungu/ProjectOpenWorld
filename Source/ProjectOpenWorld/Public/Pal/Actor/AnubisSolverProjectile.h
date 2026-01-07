#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "AnubisSolverProjectile.generated.h"

UCLASS()
class PROJECTOPENWORLD_API AAnubisSolverProjectile : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anubis Solver Projectile")
	float MoveSpeed{ 200.0f };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anubis Solver Projectile")
	FVector MoveDirection{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anubis Solver Projectile")
	float RotateSpeed{ 720.0f };



public:	
	AAnubisSolverProjectile();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void OnBeginDetected_Implementation(ACharacter* pOther) override {}
	virtual void OnEndDetected_Implementation(ACharacter* pOther) override {}
	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
	virtual void OnInteraction_Implementation(ACharacter* pOther) override {}
	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionCanceled_Implementation(ACharacter* pOther) override {}
};
