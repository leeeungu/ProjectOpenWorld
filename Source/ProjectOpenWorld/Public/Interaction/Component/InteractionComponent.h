// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class ACharacter;
class APlayerController;
class IInteractionInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	TSoftObjectPtr<APlayerController> PlayerController{};
	TSoftObjectPtr<ACharacter> PlayerCharacter{};
	TScriptInterface<IInteractionInterface> InteractionTarget{};
	TSoftObjectPtr<APlayerCameraManager> CameraManager{};
	
	bool bIsInteraction{};
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float DetectionDistance = 500.0f;

public:	
	UInteractionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnInteractionStart();
	void OnInteractionTriggered();
	void OnInteractionCompleted();
	void OnActorCancel();
protected:
	virtual void BeginPlay() override;
};
