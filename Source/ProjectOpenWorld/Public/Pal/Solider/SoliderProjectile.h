#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "SoliderProjectile.generated.h"

class UNiagaraComponent;
class UStaticMeshComponent;
class UCapsuleComponent;

UCLASS()
class PROJECTOPENWORLD_API ASoliderProjectile : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float MoveSpeed = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float InteractionDistance = 300.0f;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	//FVector MoveDirection{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> MeshComp{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UCapsuleComponent> AttackCollision{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr< UNiagaraComponent> NiagaraComp{};


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> PreViewMeshComp{};
public:	
	ASoliderProjectile();

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void ProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

public:	
	virtual void Tick(float DeltaTime) override;
	void ChangeDirection(const FVector& NewDirection);


public: // IInteractionInterface
	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
};
