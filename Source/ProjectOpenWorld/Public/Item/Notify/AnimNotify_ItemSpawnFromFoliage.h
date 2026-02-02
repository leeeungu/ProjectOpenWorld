#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ItemSpawnFromFoliage.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UAnimNotify_ItemSpawnFromFoliage : public UAnimNotify
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemSpawn", meta = (AnimNotifyBoneName = "true"))
	FName ShearchSocketName{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemSpawn")
	FVector ShearchOffset{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemSpawn")
	float ShearchRadius = 50.0f;


	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
