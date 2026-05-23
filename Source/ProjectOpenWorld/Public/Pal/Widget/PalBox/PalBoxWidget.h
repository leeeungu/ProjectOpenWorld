#pragma once

#include "CoreMinimal.h"
#include "Building/Widget/BaseBuildingAction.h"
#include "Pal/Interface/TabWidgetInterface.h"
#include "PalBoxWidget.generated.h"

class ABaseCreature;
class UPalStorageComponent;
class UPalInventoryWidget;
class UPalInfomation;
class UPalBoxSpawnWidget;
class AActor;
class UPalBoxItemTab;
class UPalItemInventoryWidget;
class UWidgetSwitcher;

UCLASS()
class PROJECTOPENWORLD_API UPalBoxWidget : public UBaseBuildingAction, public ITabWidgetInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<UPalStorageComponent> PalStorageComponent;

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UPalInventoryWidget> PalInventoryWidget{};
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UWidgetSwitcher> PalBoxSwither{};
	
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr < UPalInfomation> PalInfoWidget{};

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr < UPalBoxSpawnWidget> PalBoxSpawnWidget{};
	TWeakObjectPtr<ABaseCreature> CurrentSelectedPal{};

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr < UPalBoxItemTab> PalBoxItemTab{};
	
	UPROPERTY(EditAnywhere, Category = "PalBox")
	int32 CurSelectedIndex{};
public:
	virtual bool SetMainWidget() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
	virtual void SetSelectedPanel(int32 SelectedIndex) override;
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

	UPalItemInventoryWidget* GetPalItemInventoryWidget() const;
	//OnWidgetAdded
protected:
	//DECLARE_EVENT_TwoParams(UGameViewportSubsystem, FWidgetAddedEvent, UWidget*, ULocalPlayer*);
	//FWidgetAddedEvent OnWidgetAdded;
	UFUNCTION()
	void OnPalBoxWidgetAdded(UWidget* Widget, ULocalPlayer* LocalPlayer);
	UFUNCTION()
	void OnPalBoxChangeEvent();



};
