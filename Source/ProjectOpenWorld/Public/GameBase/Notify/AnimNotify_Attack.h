#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_Attack.generated.h"

class UAttackObject;

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EAttackObjectUtill : uint8
{
	None = 0 UMETA(Hidden),
	Attack = 1 ,
	KnockBackDirection = 2 ,
	PlayerStun = 4,
	Impulse = 8,
};


UCLASS()
class PROJECTOPENWORLD_API UAnimNotify_Attack : public UAnimNotify
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (AnimNotifyBoneName = "true"))
	FName SocketName{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector SocketOffset{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector EndOffset{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float AttackRadius = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	bool bSweepAttack = false;

	//UPROPERTY(Category = Section, Instanced, EditAnywhere)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", Instanced)
	TArray< TObjectPtr<UAttackObject>> AttackEventObject{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor", meta = (Bitmask, BitmaskEnum = "/Script/ProjectOpenWorld.EAttackObjectUtill"))
	int32 ObjectUtillMask = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor")
	bool bInstanceObject = false;
public:
#if WITH_EDITOR
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent) override;
#endif
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	TObjectPtr<UAttackObject> GetAttackObjectByUtill(EAttackObjectUtill Utill) const;
private:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);
};
