#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileAttack.generated.h"

class UCapsuleComponent;
class IAttackInterface;

UCLASS(BlueprintType, Blueprintable, Category = "Projectile")
class PROJECTOPENWORLD_API AProjectileAttack : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Projectile")
	TObjectPtr<USceneComponent> Root{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Projectile")
	TObjectPtr<UCapsuleComponent> Collision{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Projectile")
	FVector ProjectileDirection = FVector::ForwardVector;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Projectile", meta = (ClampMin = 0.0f))
	float ProjectileSpeed = 100.0f;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Projectile", meta = (ClampMin = 0.0f))
	float ProjectileLifeTime = 5.0f;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Projectile", meta = (ClampMin = 0.0f))
	float ProjectileDistanceTraveled = 0.0f;
	
	float CurrentDistanceTraveled = 0.0f;
	UPROPERTY()
	TScriptInterface<IAttackInterface>  ProjectileOwnerAttacker{};
public:	
	AProjectileAttack();

	void InitializeProjectile(TScriptInterface<IAttackInterface> OwnerAttacker,
		const FVector& Direction, float Speed, float LifeTime = 5.0f, float DistanceTraveled = 0.0f);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	virtual void Tick(float DeltaTime) override;
};
