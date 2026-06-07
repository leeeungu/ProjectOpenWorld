#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttackObject.generated.h"

class AActor;
class USkeletalMeshComponent;
class AStaticMeshActor;

USTRUCT(BlueprintType)
struct FDebugData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	float DebugLifeTime = 1;
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	FColor DebugColor = FColor::Red;
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	bool bFireInEditor = false;
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

USTRUCT()
struct FInitializeData
{
	GENERATED_BODY()
	TObjectPtr<AActor> Owner = nullptr;
	TObjectPtr<UWorld> World = nullptr;
};

USTRUCT()
struct FDebugCollisionData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "AttackEvent")
	TSubclassOf<AActor> DebugActorClass;
	UPROPERTY(EditAnywhere, Category = "AttackEvent")
	TArray<TObjectPtr<AActor>> DebugMeshActors{};
	UPROPERTY(EditAnywhere, Category = "AttackEvent")
	int32 Counts{};
};

//UCLASS(Blueprintable, abstract, editinlinenew, hidecategories = Object, collapsecategories, MinimalAPI)
UCLASS(Blueprintable, editinlinenew, Abstract)
class PROJECTOPENWORLD_API UAttackObject : public UObject
{
	GENERATED_BODY()
protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	FDebugData DebugData{};
#endif
protected:
	virtual void AttackEvent(const FAttackEventContext& AttContext) const {}
#if WITH_EDITOR
	virtual void InitializeEvent(const FInitializeData& Data) {}
	virtual void DebugAttackEvent(const FAttackEventContext& AttContext, FVector StartLocation, FVector EndLocation,const FCollisionShape& CollisionShape) const {}
#endif
public:
	void ExecuteAttackEvent(const FAttackEventContext& AttContext) const
	{
		AttackEvent(AttContext);
	}
#if WITH_EDITOR
	void ExecuteInitialize(const FInitializeData& Data)
	{
		InitializeEvent(Data);
	}

	void ExecuteDebugAttackEvent(const FAttackEventContext& AttContext, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const
	{
		DebugAttackEvent(AttContext, StartLocation, EndLocation, CollisionShape);
	}
#endif
};


UCLASS()
class PROJECTOPENWORLD_API UAttackObject_KnockBackDirection : public UAttackObject
{
	GENERATED_BODY()
private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "AttackEvent")
	FDebugCollisionData DebugCollisionData{};
#endif

protected:
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	float KnockBackForce = 1000.f;
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	FVector KnockBackDirection = FVector::ForwardVector;
	UPROPERTY(EditInstanceOnly, Category = "AttackEvent")
	bool bIsWorldDirection = false;

	virtual void AttackEvent(const FAttackEventContext& AttContext) const override;
#if WITH_EDITOR
	virtual void InitializeEvent(const FInitializeData& Data) override;
	virtual void DebugAttackEvent(const FAttackEventContext& AttContext, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const override;
#endif
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
	virtual void AttackEvent(const FAttackEventContext& AttContext) const override;
#if WITH_EDITOR
	virtual void DebugAttackEvent(const FAttackEventContext& AttContext, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const override;
#endif
};

UCLASS()
class PROJECTOPENWORLD_API UAttackObject_Attack: public UAttackObject
{
	GENERATED_BODY()
protected:
	virtual void AttackEvent(const FAttackEventContext& AttContext) const override;
#if WITH_EDITOR
	virtual void DebugAttackEvent(const FAttackEventContext& AttContext, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const override;
#endif
};

UCLASS()
class PROJECTOPENWORLD_API UAttackObject_HitReact : public UAttackObject
{
	GENERATED_BODY()
protected:
	virtual void AttackEvent(const FAttackEventContext& AttContext) const override;
#if WITH_EDITOR
	virtual void DebugAttackEvent(const FAttackEventContext& AttContext, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const override {};
#endif
};

