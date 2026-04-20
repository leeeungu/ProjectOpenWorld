#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "PlayerEquipVisualComponent.generated.h"

class UItemDataAsset;

UCLASS(ClassGroup = (PlayerEquip), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UPlayerEquipVisualComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
protected:

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "PlayerEquip")
	TObjectPtr< UItemDataAsset> EquipDataAsset{};
#endif

	virtual void BeginPlay() override;
public:
	UPlayerEquipVisualComponent();
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
