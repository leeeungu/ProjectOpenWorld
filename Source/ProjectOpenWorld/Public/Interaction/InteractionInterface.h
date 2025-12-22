#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Character.h"
#include "InteractionInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IInteractionInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnBeginDetected(ACharacter* pOther);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnEndDetected(ACharacter* pOther);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionStart(ACharacter* pOther);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteraction(ACharacter* pOther);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionEnd(ACharacter* pOther);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionCanceled(ACharacter* pOther);
};
//public: // IInteractionInterface
//	virtual void OnBeginDetected_Implementation(ACharacter* pOther) override {}
//	virtual void OnEndDetected_Implementation(ACharacter* pOther) override {}
//	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override {}
//	virtual void OnInteraction_Implementation(ACharacter* pOther) override {}
//	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override {}
