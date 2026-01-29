#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalWorkComponent.generated.h"

class ABaseCreature;
class APalAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorkCompleteSignature, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorkCancel, AActor*, Target);

UCLASS( ClassGroup=(PalWork),  Abstract)
class PROJECTOPENWORLD_API UPalWorkComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<ABaseCreature> OwnerPal{};
	UPROPERTY()
	TObjectPtr<APalAIController> OwnerController{};

public:	
	UPalWorkComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void WorkStart(AActor* Target) PURE_VIRTUAL(UPalWorkComponent::WorkStart, );
	virtual void WorkEnd(AActor* Target) PURE_VIRTUAL(UPalWorkComponent::WorkEnd, );
	virtual void WorkCancel() PURE_VIRTUAL(UPalWorkComponent::WorkCancel, );

	UPROPERTY(BlueprintAssignable, Category = "Work")
	FOnWorkCompleteSignature OnWorkComplete{};
	UPROPERTY(BlueprintAssignable, Category = "Work")
	FOnWorkCancel OnWorkCancel{};


	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
