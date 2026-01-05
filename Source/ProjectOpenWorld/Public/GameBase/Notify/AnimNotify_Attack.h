#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_Attack.generated.h"

class UAttackObject;

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
	float AttackRadius = 100.0f;

	//UPROPERTY(Category = Section, Instanced, EditAnywhere)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", Instanced)
	TArray< TObjectPtr<UAttackObject>> AttackEventObject{};

private:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);
};
