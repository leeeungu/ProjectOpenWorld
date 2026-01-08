#pragma once

#include "CoreMinimal.h"
#include "Pal/Notify/AnimNotify_PatternEvent.h"
#include "Pal/Object/PatternObj_Anubis.h"
#include "AnimNotify_PatternEvent_Anubis.generated.h"

UCLASS(Abstract)
class PROJECTOPENWORLD_API UAnimNotify_PatternEvent_Anubis : public UAnimNotify_PatternEvent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "PatternEvent")
	EAnubisPatternType  AnubisPatternEventType{};
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
