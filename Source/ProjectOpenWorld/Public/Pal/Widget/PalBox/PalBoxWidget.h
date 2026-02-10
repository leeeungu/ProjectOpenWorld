#pragma once

#include "CoreMinimal.h"
#include "Building/Widget/BaseBuildingAction.h"
#include "Player/Interface/MainWidgetInterface.h"
#include "PalBoxWidget.generated.h"

class ABaseCreature;
class UPalStorageComponent;
class UPalInventoryWidget;
class UPalInfomation;
class UPalBoxSpawnWidget;
class AActor;

UCLASS()
class PROJECTOPENWORLD_API UPalBoxWidget : public UBaseBuildingAction
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<UPalStorageComponent> PalStorageComponent;

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UPalInventoryWidget> PalInventoryWidget{};

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr < UPalInfomation> PalInfoWidget{};

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr < UPalBoxSpawnWidget> PalBoxSpawnWidget{};
	TWeakObjectPtr<ABaseCreature> CurrentSelectedPal{};

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	//void SetCommanderComponent(UPalCommanderComponent* InCommanderComponent);

	UFUNCTION()
	void OnPalInventoryChanged(int nIndex, AActor* Actor);	
	UFUNCTION()
	void OnPalSpawnInventoryChanged(int nIndex, AActor* Actor);	
	UFUNCTION()
	void OnPalSelectedChanged(ABaseCreature* Actor);

	TObjectPtr<ABaseCreature> GetPalInInventory(int Index) const;
	virtual void SetOwnerActor(AActor* NewOwner) override;

	void SwapPalInInventory(int FromIndex, int ToIndex);
	void SpawnSlotFromInventory(int FromIndex, int ToIndex);
	void SwapSpawnInventory(int FromIndex, int ToIndex);
	void DespawnSlotToInventory(int FromIndex, int ToIndex);
	//OnWidgetAdded
protected:
	//DECLARE_EVENT_TwoParams(UGameViewportSubsystem, FWidgetAddedEvent, UWidget*, ULocalPlayer*);
	//FWidgetAddedEvent OnWidgetAdded;
	UFUNCTION()
	void OnPalBoxWidgetAdded(UWidget* Widget, ULocalPlayer* LocalPlayer);
	UFUNCTION()
	void OnPalBoxChangeEvent();



};
