#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "PalAnimNotify_FacialEye.generated.h"

// "MouthType": "EPalFacialMouthType::Anger"

UENUM(BlueprintType)
enum class EPalFacialEyeType : uint8
{
	Open,
	HalfClose,
	Close,
	A, // UMETA(Hidden),
	
	B, // UMETA(Hidden),
	Anger,
	Trouble,
	Pain
};

UCLASS()
class PROJECTOPENWORLD_API UPalAnimNotify_FacialEye : public UAnimNotify
{
	GENERATED_BODY()
private:
	FVector GetEyeOffset() const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Facial")
	 EPalFacialEyeType EyeType;
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
