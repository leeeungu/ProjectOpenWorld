#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PalSpawnerComponent.generated.h"

class AActor;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnContainerUpdated, int32, Index, AActor*, Pre);

USTRUCT()
struct FContainerData
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> Pal{};
	UPROPERTY(VisibleAnywhere)
	bool bSpawned{};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalSpawnerComponent : public USceneComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray< FContainerData>  PalSpawned{};
	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 SpawnerSize{};

public:	
	UPalSpawnerComponent();
	FOnContainerUpdated OnContainerUpdated{};
	void SpawnPal(int32 Index);
	void RemovePal(int32 Index);
	bool StorePal(AActor* TargetActor, int32 Index = -1);
	TArray< FContainerData> GetSapwnContainer() const { return PalSpawned; }
	AActor* GetPal(int32 Index) const;
	int32 GetSpawnInventorySize() const { return SpawnerSize; }

	void SwapSpawnedPals(int32 Src, int32 Dst);
protected:
	virtual void BeginPlay() override;
};
