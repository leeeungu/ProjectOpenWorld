// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/InteractionInterface.h"
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
	UPROPERTY(VisibleAnywhere)
	TScriptInterface<IInteractionInterface> InteractionTarget{};
	
	bool bIsInteraction{};
protected:
	virtual void BeginPlay() override;

	
public:	
	UFUNCTION()
	void ResetInteractionTarget(AActor* DestroyedActor);
	UInteractionComponent();

	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetInteractionTarget(TScriptInterface<IInteractionInterface> NewTarget);
	void OnInteractionStart();
	void OnInteractionTriggered();
	void OnInteractionCompleted();
	void OnActorCancel();

	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetTargetActor() const;
};
