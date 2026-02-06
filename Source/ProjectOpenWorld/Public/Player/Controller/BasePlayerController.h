#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "BasePlayerController.generated.h"

class UInventoryComponent;
class UUserWidget;

UCLASS()
class PROJECTOPENWORLD_API ABasePlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent{};


	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UUserWidget> InventoryWidget{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UUserWidget> InventoryWidgetClass{};

public:
	ABasePlayerController();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	virtual FGenericTeamId GetGenericTeamId() const { return FGenericTeamId(1); }

	void ToggleInventory();
	bool bIsInventoryOpen() const;
};
