// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameBase/Notify/AnimNotify_Attack.h"
#include "AnimNotify_SphereAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOPENWORLD_API UAnimNotify_SphereAttack : public UAnimNotify_Attack
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (AnimNotifyBoneName = "true"))
	FName SocketName{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector StartOffset{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector EndOffset{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float AttackRadius = 100.0f;

	virtual FCollisionShape GetAttackCollisionShape() const override;
	virtual FVector GetStartLocation(USkeletalMeshComponent* MeshComp) const override;
	virtual FVector GetEndLocation(USkeletalMeshComponent* MeshComp) const override;
};
