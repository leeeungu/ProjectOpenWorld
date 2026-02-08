#pragma once

#include "CoreMinimal.h"
#include "Building/Widget/BaseBuildingAction.h"
#include "Player/Interface/MainWidgetInterface.h"
#include "PalBoxWidget.generated.h"

class ABaseCreature;
class UPalCommanderComponent;
class UPalInventoryWidget;
class UPalInfomation;

UCLASS()
class PROJECTOPENWORLD_API UPalBoxWidget : public UBaseBuildingAction
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<UPalCommanderComponent> CommanderComponent;

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UPalInventoryWidget> PalInventoryWidget{};

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr < UPalInfomation> PalInfoWidget{};

	TWeakObjectPtr<ABaseCreature> CurrentSelectedPal{};

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	//void SetCommanderComponent(UPalCommanderComponent* InCommanderComponent);

	UFUNCTION()
	void OnPalSelectedChanged(ABaseCreature* SelectedPal);	

	void GetSelectedPalArray(TArray<TObjectPtr<ABaseCreature>>& OutSelectedPals) const;
	virtual void SetOwnerActor(AActor* NewOwner) override;
	//OnWidgetAdded
protected:
	//DECLARE_EVENT_TwoParams(UGameViewportSubsystem, FWidgetAddedEvent, UWidget*, ULocalPlayer*);
	//FWidgetAddedEvent OnWidgetAdded;
	UFUNCTION()
	void OnPalBoxWidgetAdded(UWidget* Widget, ULocalPlayer* LocalPlayer);
	UFUNCTION()
	void OnPalBoxChangeEvent();

};
