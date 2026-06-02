#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileAttack.generated.h"

class UProjectileMovementComponent;
class UPrimitiveComponent;
class UAttackObject;
struct FProjectileLaunchParams;


UCLASS(Abstract, BlueprintType, Blueprintable, Category = "Projectile")
class PROJECTOPENWORLD_API AProjectileAttack : public AActor
{
	GENERATED_BODY()

public:
	AProjectileAttack();

	virtual void OnConstruction(const FTransform& Transform) override;
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void Launch(const FProjectileLaunchParams& Params);
protected:
	virtual void BeginPlay() override;

	// [3] 빈 루트. 실제 콜리전은 BP가 이 아래 자식으로 추가하고 GetCollision으로 지정
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<USceneComponent> Root{};

	// 이동(C++ 소유). UpdatedComponent는 콜리전이 BP에서 오므로 BeginPlay에서 연결
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> Movement{};

	UPROPERTY(EditAnywhere, Instanced, Category = "ProjectileAttack", BlueprintReadWrite)
	TArray<TObjectPtr<UAttackObject>> AttackEventObject{};   

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float InitialSpeed = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float Damage = 10.f;

	// [1] 콜리전 "모양"은 BP가 지정 — C++는 프로파일/바인딩/이동만 책임
	UFUNCTION(BlueprintNativeEvent, Category = "Projectile")
	UPrimitiveComponent* GetCollision() const;
	virtual UPrimitiveComponent* GetCollision_Implementation() const;

	// 피격 시 비주얼 반응(Niagara/Mesh) — BP에서 구현
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
	void OnProjectileHit(const FHitResult& Hit);

	// 적중 후 운명 — 기본은 소멸, 관통/바운스는 이 함수만 override
	UFUNCTION(BlueprintNativeEvent, Category = "Projectile")
	void ResolveHit(const FHitResult& Hit);
	virtual void ResolveHit_Implementation(const FHitResult& Hit);

private:
	// 캐시된 콜리전 (BeginPlay에서 1회 resolve → 이후 O(1))
	UPROPERTY(Transient)
	TObjectPtr<UPrimitiveComponent> Collision;

	// [2] PalDamage 프로파일을 콜리전에 적용 (에디터/런타임 공용)
	void ApplyCollisionProfile();


	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};