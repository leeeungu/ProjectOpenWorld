#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttackObject.generated.h"

class AActor;
class USkeletalMeshComponent;
USTRUCT(BlueprintType)
struct FDebugData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	float DebugLifeTime = 1;

	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	FColor DebugColor = FColor::Red;
};

USTRUCT()
struct FAttackEventContext
{
	GENERATED_BODY()
	TObjectPtr<AActor> Owner = nullptr;           // 공격자
	TObjectPtr<AActor> Causer = nullptr;           // 타격 원인(무기/투사체/본인)
	TObjectPtr<USceneComponent> SourceComp = nullptr; // 위치·소켓 기준(Mesh or Collision)
	const FHitResult* Hit{};
};

//UCLASS(Blueprintable, abstract, editinlinenew, hidecategories = Object, collapsecategories, MinimalAPI)
UCLASS(Blueprintable, editinlinenew, Abstract)
class PROJECTOPENWORLD_API UAttackObject : public UObject
{
	GENERATED_BODY()
protected:

protected:
	virtual void AttackEvent(const FAttackEventContext& AttContext) const {}
	virtual void DebugAttackEvent(const FAttackEventContext& AttContext, FVector StartLocation, FVector EndLocation,const FCollisionShape& CollisionShape) const {}
public:
	void ExecuteAttackEvent(const FAttackEventContext& AttContext) const
	{
		AttackEvent(AttContext);
	}
	void ExecuteDebugAttackEvent(const FAttackEventContext& AttContext, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const
	{
		DebugAttackEvent(AttContext, StartLocation, EndLocation, CollisionShape);
	}
};


UCLASS()
class PROJECTOPENWORLD_API UAttackObject_KnockBackDirection : public UAttackObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	float KnockBackForce = 1000.f;
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	FVector KnockBackDirection = FVector::ForwardVector;
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	bool bIsWorldDirection = false;
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	FDebugData DebugData{};

	virtual void AttackEvent(const FAttackEventContext& AttContext) const override;
	virtual void DebugAttackEvent(const FAttackEventContext& AttContext, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const override;
};

UCLASS()
class PROJECTOPENWORLD_API UAttackObject_PlayerStun : public UAttackObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	float StunTime = 1.f;

	virtual void AttackEvent(const FAttackEventContext& AttContext) const override;
};


UCLASS()
class PROJECTOPENWORLD_API UAttackObject_Impulse: public UAttackObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	float MaxImpulseForce = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackEvent", meta = (AnimNotifyBoneName = "true"))
	FName SocketName{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackEvent")
	FVector SocketOffset{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackEvent")
	float AttackRadius = 100.0f;
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	FDebugData DebugData{};

	virtual void AttackEvent(const FAttackEventContext& AttContext) const override;
	virtual void DebugAttackEvent(const FAttackEventContext& AttContext, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const override;
};

UCLASS()
class PROJECTOPENWORLD_API UAttackObject_Attack: public UAttackObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	FDebugData DebugData{};

protected:
	virtual void AttackEvent(const FAttackEventContext& AttContext) const override;
	virtual void DebugAttackEvent(const FAttackEventContext& AttContext, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const override;
};

UCLASS()
class PROJECTOPENWORLD_API UAttackObject_HitReact : public UAttackObject
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	FDebugData DebugData{};

	virtual void AttackEvent(const FAttackEventContext& AttContext) const override;
	virtual void DebugAttackEvent(const FAttackEventContext& AttContext, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const override {};
};

