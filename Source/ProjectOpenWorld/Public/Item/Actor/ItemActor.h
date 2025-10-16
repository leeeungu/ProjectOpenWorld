#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "ItemActor.generated.h"

class UItemPrimaryDataAsset;

UCLASS()
class PROJECTOPENWORLD_API AItemActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (ExposeOnSpawn = "TRUE"))
	TObjectPtr<UItemPrimaryDataAsset> ItemData{};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TSubclassOf<UUserWidget> ToolTipWidgetClass{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TSoftObjectPtr<UUserWidget> ToolTipWidget{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (ExposeOnSpawn = "TRUE"))
	int itemCount = 1;
public:	
	AItemActor();
//	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "ItemData")
	FORCEINLINE UItemPrimaryDataAsset* const GetItemDataAsset() const { return ItemData.Get(); }
protected:
	virtual void BeginPlay() override;

public: // IInteractionInterface
	virtual void OnBeginDetected_Implementation(APlayerController* pPlayer) override;
	virtual void OnEndDetected_Implementation(APlayerController* pPlayer) override;
	virtual void OnInteractionStart_Implementation(APlayerController* pPlayer) override;
	virtual void OnInteraction_Implementation() override {}
	virtual void OnInteractionEnd_Implementation(APlayerController* pPlayer) override {}
};
