#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
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
	void OnBeginDetected(APlayerController* pPlayer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnEndDetected(APlayerController* pPlayer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionStart(APlayerController* pPlayer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteraction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionEnd(APlayerController* pPlayer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionCanceled();
};
//public: // IInteractionInterface
//	virtual void OnBeginDetected_Implementation(APlayerController* pPlayer) override {}
//	virtual void OnEndDetected_Implementation(APlayerController* pPlayer) override {}
//	virtual void OnInteractionStart_Implementation(APlayerController* pPlayer) override {}
//	virtual void OnInteraction_Implementation() override {}
//	virtual void OnInteractionEnd_Implementation(APlayerController* pPlayer) override {}
