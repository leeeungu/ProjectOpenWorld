#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "BasePlayerController.generated.h"

class UInventoryComponent;
class UUserWidget;
class UPlayerInventoryWidget;
class APlayerPreviewPawn;
class UPlayerPreviewComponent;

UCLASS()
class PROJECTOPENWORLD_API ABasePlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory")
	TObjectPtr <UPlayerPreviewComponent> PreviewComponent{};

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UPlayerInventoryWidget> InventoryWidget{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UPlayerInventoryWidget> InventoryWidgetClass{};

protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	ABasePlayerController();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE UPlayerPreviewComponent* GetPreviewComponent() const { return PreviewComponent; }

	virtual FGenericTeamId GetGenericTeamId() const { return FGenericTeamId(1); }

	UUserWidget* GetInventoryWidget() const;
	bool bIsInventoryOpen() const;

};
