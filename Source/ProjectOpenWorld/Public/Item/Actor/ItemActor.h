#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "Item/Interface/TransportInterface.h"
#include "GameBase/Interface/GenerateWorldInterface.h"
#include "ItemActor.generated.h"

class USkeletalMeshComponent;
class UWidgetComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECTOPENWORLD_API AItemActor : public AActor, public IInteractionInterface, public ITransportInterface, public IGenerateWorldInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<USphereComponent> ItemCollision{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<USkeletalMeshComponent> ItemSkeletalMesh{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UStaticMeshComponent> ItemStaticMesh{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UWidgetComponent> ItemWidget{};
	
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (ExposeOnSpawn = "TRUE"))
	//TObjectPtr<UItemPrimaryDataAsset> ItemData{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (ExposeOnSpawn = "TRUE"))
	FName ItemID = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TSubclassOf<UUserWidget> ToolTipWidgetClass{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TSoftObjectPtr<UUserWidget> ToolTipWidget{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (ExposeOnSpawn = "TRUE"))
	int itemCount = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TObjectPtr<USoundBase> PickUpSound{};
	UPROPERTY()
	TObjectPtr< AActor> Transport{};
	ETransportState TransportState{};
public:	
	AItemActor();
//	virtual void Tick(float DeltaTime) override;

	//UFUNCTION(BlueprintPure, Category = "ItemData")
	//FORCEINLINE UItemPrimaryDataAsset* const GetItemDataAsset() const { return ItemData.Get(); }
protected:
	virtual void BeginPlay() override;

public: // IInteractionInterface
	void Init(FName NewItemID, int Count);

	virtual void OnBeginDetected_Implementation(ACharacter* pOther) override;
	virtual void OnEndDetected_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
	virtual void OnInteraction_Implementation(ACharacter* pOther) override {}
	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override {}


	virtual void OnTransportRegister_Implementation(AActor* Other) override;
	virtual void OnTransportUnRegister_Implementation(AActor* Other) override;
	virtual void OnTransportEnd_Implementation(AActor* Other) override {}
	virtual void OnTransportCancel_Implementation(AActor* Other) override {}
	virtual ETransportState GetTransportState_Implementation() override;

	// GenerateWorldInterface
	virtual void NewGenerateWorldEvent(const FGenerateSectionData& SectionData) override;
	virtual void DelGenerateWorldEvent(const FGenerateSectionData& SectionData) override;

	FORCEINLINE UPrimitiveComponent* GetItemCollision() const;
};
