#pragma once

#include "CoreMinimal.h"
#include "Building/Widget/BaseBuildingAction.h"
#include "Pal/Interface/TabWidgetInterface.h"
#include "PalBoxWidget.generated.h"

class APalBaseCreature;
class UPalStorageComponent;
class UPalInventoryWidget;
class UPalInfomation;
class UPalBoxSpawnWidget;
class AActor;
class UPalBoxItemTab;
class UPalItemInventoryWidget;
class UWidgetSwitcher;
class UPalSpawnerComponent;

UCLASS()
class PROJECTOPENWORLD_API UPalBoxWidget : public UBaseBuildingAction, public ITabWidgetInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<UPalStorageComponent> PalStorageComponent{};
	TObjectPtr< UPalSpawnerComponent> PalSpawnerComponent{};

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UPalInventoryWidget> PalInventoryWidget{};
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UWidgetSwitcher> PalBoxSwither{};
	
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr < UPalInfomation> PalInfoWidget{};

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr < UPalBoxSpawnWidget> PalBoxSpawnWidget{};
	TWeakObjectPtr<APalBaseCreature> CurrentSelectedPal{};

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr < UPalBoxItemTab> PalBoxItemTab{};
	
	UPROPERTY(EditAnywhere, Category = "PalBox")
	int32 CurSelectedIndex{};
public:
	virtual bool SetMainWidget() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
	virtual void SetSelectedPanel(int32 SelectedIndex) override;
	//void SetCommanderComponent(UPalCommanderComponent* InCommanderComponent);

	UFUNCTION()
	void OnPalInventoryChanged(int nIndex, AActor* Actor);	
	UFUNCTION()
	void OnPalSpawnInventoryChanged(int nIndex, AActor* Actor);	
	UFUNCTION()
	void OnPalSelectedChanged(APalBaseCreature* Actor);

	TObjectPtr<APalBaseCreature> GetPalInInventory(int Index) const;
	virtual void SetOwnerActor(AActor* NewOwner) override;
	void SetStorageComponent(UPalStorageComponent* Storage);
	void SetPalSpawnerComponent(UPalSpawnerComponent* Spawner);

	void SwapPalInInventory(int FromIndex, int ToIndex);
	void SpawnSlotFromInventory(int FromIndex, int ToIndex);
	void SwapSpawnInventory(int FromIndex, int ToIndex);
	void DespawnSlotToInventory(int FromIndex, int ToIndex);


	void StorePal(struct FPalStoreInventoryData NewPal);
	void RemovePal(int Index);

	UPalStorageComponent* GetPalStorageComponent() const { return PalStorageComponent; }
	UPalSpawnerComponent* GetPalSpawnerComponent() const { return PalSpawnerComponent; }
	UPalItemInventoryWidget* GetPalItemInventoryWidget() const;
	//OnWidgetAdded
protected:
	//DECLARE_EVENT_TwoParams(UGameViewportSubsystem, FWidgetAddedEvent, UWidget*, ULocalPlayer*);
	//FWidgetAddedEvent OnWidgetAdded;
	UFUNCTION()
	void OnPalBoxWidgetAdded(UWidget* Widget, ULocalPlayer* LocalPlayer);
	UFUNCTION()
	void OnPalBoxChangeEvent();

	UFUNCTION()
	void OnUpdateSpawnSlot(int32 Index, AActor* preActor);
};
