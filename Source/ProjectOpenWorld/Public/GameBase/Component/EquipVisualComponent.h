#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "EquipVisualComponent.generated.h"

class UItemDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UEquipVisualComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
public:	
	UEquipVisualComponent();

protected:
	virtual void BeginPlay() override;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "PlayerEquip")
	TObjectPtr< UItemDataAsset> EquipDataAsset{};
#endif
public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
