#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Player/Interface/PlayerInputInterface.h"
#include "PlayerEquipComponent.generated.h"

enum class EWeapone : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPlayerEquipComponent : public USkeletalMeshComponent, public IPlayerInputInterface
{
	GENERATED_BODY()
private:
	EWeapone CurrentWeapone{};
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerEquip")
	TMap<EWeapone, FString> EquipSocket{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerEquip")
	TMap<EWeapone, TObjectPtr<USkeletalMesh>> EquipMesh{};

	TObjectPtr< USkeletalMeshComponent> PlayerMesh{};
public:	
	UPlayerEquipComponent();

protected:
	virtual void BeginPlay() override;

public:	
	void SetWeapone(EWeapone NewWeapone) { CurrentWeapone = NewWeapone; }

	// 	IPlayerInputInterface implementation
	virtual void StartEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;
	virtual void TriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;
	virtual void CompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;

	void SetEquipMesh(USkeletalMesh* NewMesh);
	void SetUnequipMesh(USkeletalMesh* OldMesh);
};
