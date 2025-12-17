#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pal/Interface/CommanderManageable.h"
#include "BaseBuilding.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UMaterials;
class UBuildingProgress;
class UNavModifierComponent;
class UBuildingActionWidgetComponent;

UCLASS(BlueprintType, Blueprintable, ClassGroup = Architecture)
class PROJECTOPENWORLD_API ABaseBuilding : public AActor, public ICommanderManageable
{
	GENERATED_BODY()
protected:

	// 해당 기능을 묶어서 처리면 좋을듯?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Architecture", BlueprintGetter = "GetBuildingMeshComponent")
	TObjectPtr < UStaticMeshComponent> buildingMeshComponent{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Architecture", BlueprintGetter = "GetBuildingProgress")
	TObjectPtr < UBuildingProgress> buildingProgressComponent{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Architecture")
	TObjectPtr<UBuildingActionWidgetComponent> BuildActionWidget{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Architecture")
	TObjectPtr<UNavModifierComponent> NavModifier{};

public:		
	ABaseBuilding();

	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
private:
	FPalCommand Command{};

public:
	virtual EPalCommandKind GetCommandKind_Implementation() override;
	virtual uint8 GetSubCommandType_Implementation() override;
	virtual FPalCommand GetCommand_Implementation() override;

	void UpdateModifier();
protected:
	virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintPure, Category = "Building")
	FORCEINLINE UStaticMeshComponent* GetBuildingMeshComponent() const { return buildingMeshComponent; }
	UFUNCTION(BlueprintPure, Category = "Building")
	FORCEINLINE UBuildingProgress* GetBuildingProgress() const { return buildingProgressComponent; }
};
