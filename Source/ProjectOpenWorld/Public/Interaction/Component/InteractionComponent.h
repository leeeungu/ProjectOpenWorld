// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/InteractionInterface.h"
#include "Player/Interface/MainWidgetInterface.h"
#include "InteractionComponent.generated.h"

class ACharacter;
class APlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TSoftObjectPtr<ACharacter> OwnerCharacter{};
private:
	UPROPERTY(VisibleAnywhere)
	TScriptInterface<IInteractionInterface> InteractionTarget{};
	
	bool bIsInteraction{};
	bool bIsDetect{};
protected:
	virtual void BeginPlay() override;

	
	void SetTarget(TScriptInterface<IInteractionInterface> NewTarget);
public:	
	UFUNCTION()
	void ResetInteractionTarget(AActor* DestroyedActor);
	UInteractionComponent();

	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetInteractionTarget(TScriptInterface<IInteractionInterface> NewTarget);
	void OnInteractionStart();
	void OnInteractionTriggered();
	void OnInteractionCompleted();
	void OnDetectEnd();
	void OnDetectBegin();

	// 잘못 만든 함수 삭제 필요
	void OnActorCancel();

	bool IsInteracting() const { return bIsInteraction; }
	bool IsSetTarget() const;
	bool CheckSameTarget(TScriptInterface<IInteractionInterface> Other) const;

	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetTargetActor() const;
};
