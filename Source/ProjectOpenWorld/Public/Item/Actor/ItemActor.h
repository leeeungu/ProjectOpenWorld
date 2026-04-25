#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "Item/Interface/TransportInterface.h"
#include "GameBase/Interface/GenerateWorldInterface.h"
#include "ItemActor.generated.h"

class UBaseItem;
class UUserWidget;
class USkeletalMeshComponent;
class UWidgetComponent;
class USphereComponent;
class UStaticMeshComponent;
enum EEffectSoundType : uint8;

UCLASS()
class PROJECTOPENWORLD_API AItemActor : public AActor, public IInteractionInterface, public ITransportInterface, public IGenerateWorldInterface{
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced, Category = "ItemData")
	TObjectPtr<UBaseItem> ItemInstance{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TSubclassOf<UUserWidget> ToolTipWidgetClass{};

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> ToolTipWidget{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TEnumAsByte<EEffectSoundType> PickUpSound{};

	UPROPERTY()
	TObjectPtr<AActor> Transport{};

	ETransportState TransportState{ ETransportState::NotTransport };

public:
	AItemActor();

protected:
	virtual void BeginPlay() override;
	void RefreshVisualFromItem();

public:
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool InitFromItem(UBaseItem* InItem);

	// 임시 호환용. 전부 갈아탄 뒤 제거 권장
	UFUNCTION(BlueprintCallable, Category = "Item")
	void Init(FName NewItemID, int32 Count);

	// 인벤토리 전달용 복제본 생성
	UFUNCTION(BlueprintCallable, Category = "Item")
	UBaseItem* DuplicateItemFor(UObject* NewOuter) const;

	UFUNCTION(BlueprintPure, Category = "Item")
	FORCEINLINE UBaseItem* GetItemInstance() const { return ItemInstance; }

	UFUNCTION(BlueprintPure, Category = "Item")
	FName GetItemID() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	int32 GetItemCount() const;

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

	virtual void NewGenerateWorldEvent(const FGenerateSectionData& SectionData) override;
	virtual void DelGenerateWorldEvent(const FGenerateSectionData& SectionData) override;

	UPrimitiveComponent* GetItemCollision() const;

private: 
	void SetMeshAsset(class UItemVisibleDataFragment* VisibleDataFragment);
	void SetStaticMeshAsset(class UStaticMesh* StaticMesh);
	void SetSkeletalMeshAsset(class USkeletalMesh* SkeletalMesh);
};