#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/Interface/MainWidgetInterface.h"
#include "BaseBuildingAction.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UBaseBuildingAction : public UUserWidget, public IMainWidgetInterface
{
	GENERATED_BODY()

protected:
	UBaseBuildingAction(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
public:
	// IMainWidgetInterface interface
	virtual bool SetMainWidget() override;
	virtual void UnSetMainWidget() override;

	virtual void SetOwnerActor(AActor* NewOwner) {}


};
