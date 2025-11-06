// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseBuildingAction.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOPENWORLD_API UBaseBuildingAction : public UUserWidget
{
	GENERATED_BODY()

protected:
	UBaseBuildingAction(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void OpenBuildingActionWidget();
	void CloseBuildingActionWidget();
public:
	void BuildingAction();
};
