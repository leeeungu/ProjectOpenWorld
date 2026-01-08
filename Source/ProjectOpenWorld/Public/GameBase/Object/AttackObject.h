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

//UCLASS(Blueprintable, abstract, editinlinenew, hidecategories = Object, collapsecategories, MinimalAPI)
UCLASS(Blueprintable, editinlinenew, Abstract)
class PROJECTOPENWORLD_API UAttackObject : public UObject
{
	GENERATED_BODY()
protected:

protected:
	virtual void AttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const {}
	virtual void DebugAttackEvent(USkeletalMeshComponent* CauserMesh, FVector AttackLocation, float HitRadius) const {}
public:
	void ExecuteAttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const
	{
		AttackEvent(CauserMesh, HitData);
	}
	void ExecuteDebugAttackEvent(USkeletalMeshComponent* CauserMesh, FVector AttackLocation, float HitRadius) const
	{
		DebugAttackEvent(CauserMesh, AttackLocation, HitRadius);
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

	virtual void AttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const override;
	virtual void DebugAttackEvent(USkeletalMeshComponent* CauserMesh, FVector AttackLocation, float HitRadius) const override;
};

UCLASS()
class PROJECTOPENWORLD_API UAttackObject_PlayerStun : public UAttackObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	float StunTime = 1.f;

	virtual void AttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const override;
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

	virtual void AttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const override;
	virtual void DebugAttackEvent(USkeletalMeshComponent* CauserMesh, FVector AttackLocation, float HitRadius) const override;
};

UCLASS()
class PROJECTOPENWORLD_API UAttackObject_Attack: public UAttackObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	FDebugData DebugData{};

protected:
	virtual void AttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const override;
	virtual void DebugAttackEvent(USkeletalMeshComponent* CauserMesh, FVector AttackLocation, float HitRadius) const override;
};
