#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
#include "Pal/Data/PalCommandData.h"
#include "PalWorkComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorkCompleteSignature, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorkCancel, AActor*, Target);

UCLASS(
	Abstract,                  // SpawnActor / NewObject 불가
	NotBlueprintable,          // BP 자식 클래스 못 만듦
	NotPlaceable,              // 레벨 드래그 배치 못함
	HideDropdown,              // 클래스 드롭다운에서 숨김
	meta = (
		DeprecationMessage = "Use UPalJobComponent instead. This class will be removed."
		)
)
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
	bool bIsWorking{};
	bool bIsWorkEnd{};
	bool bIsArrive{};
public:	
	UPalWorkComponent();

protected:
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	bool IsArrive() const { return bIsArrive; }
	UFUNCTION()
	virtual bool CanWork() { return !bIsWorking; }
	UFUNCTION()
	virtual bool IsWorkEnd() const { return bIsWorkEnd; }
	UFUNCTION()
	virtual void WorkStart(const FPalCommand& Command) PURE_VIRTUAL(UPalWorkComponent::WorkStart, );
	UFUNCTION()
	virtual void WorkEvent(const FPalCommand& Command) PURE_VIRTUAL(UPalWorkComponent::WorkEvent, );
	UFUNCTION()
	virtual void WorkEnd(const FPalCommand& Command) PURE_VIRTUAL(UPalWorkComponent::WorkEnd, );
	UFUNCTION()
	virtual void WorkCancel() PURE_VIRTUAL(UPalWorkComponent::WorkCancel, );

	UPROPERTY(BlueprintAssignable, Category = "Work")
	FOnWorkCompleteSignature OnWorkComplete{};
	UPROPERTY(BlueprintAssignable, Category = "Work")
	FOnWorkCancel OnWorkCancel{};


	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
