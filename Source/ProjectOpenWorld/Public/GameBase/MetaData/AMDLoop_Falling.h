#pragma once

#include "CoreMinimal.h"
#include "GameBase/MetaData/AMDLoop.h"
#include "AMDLoop_Falling.generated.h"

class UCharacterMovementComponent;

UCLASS()
class PROJECTOPENWORLD_API UAMDLoop_Falling : public UAMDLoop
{
	GENERATED_BODY()
protected:
public:
	UAMDLoop_Falling(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class PROJECTOPENWORLD_API UAnimLoopObject_Falling: public UAnimLoopObject
{
	GENERATED_BODY()
private:
	TObjectPtr< UCharacterMovementComponent> Movement{};	
public:
	virtual void Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData) override;
	virtual bool IsLoop() const override;
};

