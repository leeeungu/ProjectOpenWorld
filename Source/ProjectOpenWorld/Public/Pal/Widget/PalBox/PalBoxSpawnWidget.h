#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalBoxSpawnWidget.generated.h"

class UScrollBox;
class UPalBoxSpawnSlot;
class UPalSpawnerComponent;

UCLASS()
class PROJECTOPENWORLD_API UPalBoxSpawnWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = "PalBox", meta = (BindWidget))
	TObjectPtr<UScrollBox> SpawnBoxScrollBox{};

	UPROPERTY(EditDefaultsOnly, Category = "PalBox")
	TSubclassOf<UPalBoxSpawnSlot> PalBoxSpawnSlotClass{};
public:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	void UpdatePalSpawnInventory(int InventoryIndex, AActor* NewActor);

	void UpdateSlots(UPalSpawnerComponent* Component);

	void CreateSpawnSlotWidget(int SlotIndex);
};
