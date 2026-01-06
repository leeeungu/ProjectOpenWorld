#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "BasePlayerController.generated.h"

class UInventoryComponent;

UCLASS()
class PROJECTOPENWORLD_API ABasePlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent{};
public:
	ABasePlayerController();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	virtual FGenericTeamId GetGenericTeamId() const { return FGenericTeamId(1); }
};
