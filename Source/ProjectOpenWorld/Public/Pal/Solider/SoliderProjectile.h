#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoliderProjectile.generated.h"

class UNiagaraComponent;
class UStaticMeshComponent;
class UCapsuleComponent;

UCLASS()
class PROJECTOPENWORLD_API ASoliderProjectile : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float MoveSpeed = 100.0f;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	//FVector MoveDirection{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> MeshComp{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UCapsuleComponent> AttackCollision{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr< UNiagaraComponent> NiagaraComp{};

public:	
	ASoliderProjectile();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	void ChangeDirection(const FVector& NewDirection);
};
