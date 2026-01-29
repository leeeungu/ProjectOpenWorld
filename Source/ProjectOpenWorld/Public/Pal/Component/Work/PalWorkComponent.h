#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
#include "Pal/Data/PalCommandData.h"
#include "PalWorkComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorkCompleteSignature, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorkCancel, AActor*, Target);

UCLASS( ClassGroup=(PalWork),  Abstract)
class PROJECTOPENWORLD_API UPalWorkComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	EPalCommandKind PalCommand{};
	uint8 PalSubCommand{};
	UPROPERTY()
	TObjectPtr<ABaseCreature> OwnerPal{};
	UPROPERTY()
	TObjectPtr<APalAIController> OwnerController{};

public:	
	UPalWorkComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void WorkStart(const FPalCommand& Command) PURE_VIRTUAL(UPalWorkComponent::WorkStart, );
	virtual void WorkEnd(const FPalCommand& Command) PURE_VIRTUAL(UPalWorkComponent::WorkEnd, );
	virtual void WorkCancel() PURE_VIRTUAL(UPalWorkComponent::WorkCancel, );

	UPROPERTY(BlueprintAssignable, Category = "Work")
	FOnWorkCompleteSignature OnWorkComplete{};
	UPROPERTY(BlueprintAssignable, Category = "Work")
	FOnWorkCancel OnWorkCancel{};


	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
