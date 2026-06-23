#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pal/Interface/CommanderManageable.h"
#include "Pal/Interface/PalWorkable.h"
#include "GameBase/Interface/GenerateWorldInterface.h"
#include "Pal/Interface/PalSaveGameObject.h"
#include "BaseBuilding.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UMaterials;
class UBuildingProgress;
class UNavModifierComponent;
class UPalBuildingStaticMeshComponent;
class UPalActionWidgetComponent;
class UPalWorkerInterface;

UCLASS(BlueprintType, Blueprintable, ClassGroup = Architecture)
class PROJECTOPENWORLD_API ABaseBuilding : public AActor, public IGenerateWorldInterface, public IPalWorkable, public IPalSaveGameObject
	// public ICommanderManageable, 
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Architecture")
	TObjectPtr<UPalBuildingStaticMeshComponent> PalBuildingStaticMeshComponent{};
	// 해당 기능을 묶어서 처리면 좋을듯?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Architecture")
	TObjectPtr<UPalActionWidgetComponent> BuildActionWidgetCom{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Architecture")
	TObjectPtr<UNavModifierComponent> NavModifier{};
	FVector WorkLocation{};   // 트레이스 실패 시 폴백
public:		
	ABaseBuilding();

	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginDestroy() override;
//private:
//	FPalCommand Command{};
//
//public:
//	virtual EPalCommandKind GetCommandKind_Implementation() override;
//	virtual uint8 GetSubCommandType_Implementation() override;
//	virtual FPalCommand GetCommand_Implementation() override;
//	virtual bool IsCommandFinished_Implementation() override;

public:
	virtual void OnPreSave() override {}
	virtual void OnLoaded() override {}

	// GenerateWorldInterface
	virtual void NewGenerateWorldEvent(const FGenerateSectionData& SectionData) override;
	virtual void DelGenerateWorldEvent(const FGenerateSectionData& SectionData) override;

	void UpdateModifier();
	void NoCollision();


	// IPalWorkableInterface
	virtual void ResiterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass) override;
	virtual void UnregisterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass) override;
	virtual bool IsWorkable() const override;
	virtual EPalJobType GetWorkJobType() const { return EPalJobType::Architecture; }
	virtual FPalWorkCommand GetWorkCommand(AActor* InstigatorActor, AActor* Target) const override;
protected:
	virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintPure, Category = "Building")
	UStaticMeshComponent* GetBuildingMeshComponent() const;
	UFUNCTION(BlueprintPure, Category = "Building")
	FORCEINLINE UPalBuildingStaticMeshComponent* GetBuildingProgress() const { return PalBuildingStaticMeshComponent; }
};
