// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Component/InteractionComponent.h"
#include "PlayerInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPlayerInteractionComponent : public UInteractionComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TSoftObjectPtr<APlayerCameraManager> CameraManager{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float DetectionDistance = 500.0f;

	bool bCanAiming = false;
public:	
	UPlayerInteractionComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
public:
	FORCEINLINE void SetCanAiming(bool bEnable) { bCanAiming = bEnable; }
	FORCEINLINE bool IsCanAiming() const { return bCanAiming; }
};
