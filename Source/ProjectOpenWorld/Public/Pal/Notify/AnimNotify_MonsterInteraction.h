#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_MonsterInteraction.generated.h"

enum class EMonsterInteractionEvent :uint8;

UCLASS()
class PROJECTOPENWORLD_API UAnimNotify_MonsterInteraction : public UAnimNotify
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MonsterInteraction")
	uint8 InteractionID{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MonsterInteraction")
	EMonsterInteractionEvent InteractionEventType{};


public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)  override;
	
};
