#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "WorldGenerator.generated.h"

class UProceduralMeshComponent;
enum class EUpdateTransformFlags : int32;
enum class ETeleportType : uint8;
class FObjectPreSaveContext;

// 4, 4

USTRUCT(BlueprintType)
struct FHeightMapPngData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	float CellSize = 700.0f; //300

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")

	int xVertexCount = 50; //40
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	int yVertexCount = 50; // 40
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	FIntPoint StartSectionCoordinate = FIntPoint::ZeroValue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	FIntPoint EndSectionCoordinate = FIntPoint::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TArray<float> scales = {0.000001,0.00002,0.0001,0.001};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TArray<float> amplitudes = {20000,10000,500,100};
};

UCLASS()
class PROJECTOPENWORLD_API AWorldGenerator : public AActor
{
	friend class FAsyncWorldGenerater;
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = "1"))
	int xVertexCount = 50;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = "1"))
	int yVertexCount = 50;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	float CellSize = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	int NumOfSectionsX = 2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	int NumOfSectionsY = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Settings")
	int MeshSectionIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	float TileRelplaceableDistance = 3000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TArray<float> scales{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TArray<float> amplitudes{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	FHeightMapPngData HeightMapData{};


	UPROPERTY(BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UProceduralMeshComponent> TerrainMesh{};



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr< UMaterialInterface> TerrainMaterial{};

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere, Category = "Landscape Settings")
	bool GeneratorBusy = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Landscape Settings")
	bool TileDataReady = false;

	UPROPERTY(BlueprintReadOnly, Category = "Landscape Settings")
	TMap<FIntPoint, int> QueuedTiles{};



	int SectionIndexX{};
	int SectionIndexY{};

	//int PlayerSectionIndexX{};
	//int PlayerSectionIndexY{};

	TArray<FVector> SubVertices{};
	TArray<FVector2D> SubUVs{};
	TArray<int32> SubTriangles{};
	TArray<FVector> SubNormals{};
	TArray<FProcMeshTangent> SubTangents{};

	TArray<int32> Triangles{};

private:
#if WITH_EDITOR
	bool bPropertyChaned = false;
#endif
public:	
	AWorldGenerator();
public:
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
protected:
	virtual void BeginPlay() override;

	void OnUpdatePlayerLocation(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	void GenerateTerrain(const int inSectionIndexX, const int inSectionIndexY);
	//void GenerateTerrainInRange(const FIntPoint& StartCoord, const FIntPoint& EndCoord);

	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	void GenerateTerrainAsync(const int inSectionIndexX, const int inSectionIndexY);

	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	void DrawTile();
	//UFUNCTION(BlueprintCallable, Category = "Landscape Generation")

	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	FVector GetPlayerLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	FVector2D GetTileLocation(FIntPoint TileCoordinate) const;

	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	bool GetClosestQueuedTile(FIntPoint& Result ) const;

	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	int GetFurthestTile() const;
	public:
	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	float GetHeight(float inX, float inY) const;
	float PerlinNoiseExtended(float x, float y, float scale, float Amplitude, FVector2D offset = FVector2D::ZeroVector) const;
};


class FAsyncWorldGenerater : public FNonAbandonableTask
{
public:
	FAsyncWorldGenerater(AWorldGenerator* InWorldGenerator)
	{
		WorldGenerator = InWorldGenerator;
	}

	FORCEINLINE	TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncWorldGenerater, STATGROUP_ThreadPoolAsyncTasks);
	}

	void DoWork();

private:
	UPROPERTY()
	TObjectPtr<AWorldGenerator> WorldGenerator{};

};