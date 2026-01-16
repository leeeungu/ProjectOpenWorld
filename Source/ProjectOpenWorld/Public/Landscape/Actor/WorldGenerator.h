#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "WorldGenerator.generated.h"

class UProceduralMeshComponent;
enum class EUpdateTransformFlags : int32;
enum class ETeleportType : uint8;
class FObjectPreSaveContext;
class UHierarchicalInstancedStaticMeshComponent;
class UInstancedStaticMeshComponent;

UCLASS()
class PROJECTOPENWORLD_API AWorldGenerator : public AActor
{
	friend class FAsyncWorldGenerater;
	friend class UQuadtreeWorldSubsystem;
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = "1"))
	int xVertexCount = 50;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = "1"))
	int yVertexCount = 50;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	float CellSize = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = "2", ClampMax = "10"))
	int SectionRadiusCount = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Settings")
	int MeshSectionIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = "1", ClampMax = "100"))
	int SectionCreateTickCount{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = "1", ClampMax = "100"))
	int TickFoliageCount{};

	int CurrentFoliageCount{};
	int FolliageSectionIndex{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TArray<float> scales{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TArray<float> amplitudes{};

	int MaxSection{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UProceduralMeshComponent> TerrainMesh{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UInstancedStaticMeshComponent> FoligeMesh{};
	
	UPROPERTY(VisibleAnywhere, Category = "Landscape Settings")
	TObjectPtr<UProceduralMeshComponent> GenerateTerrain{};
	int CurrentMeshIndex{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr< UMaterialInterface> TerrainMaterial{};

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere, Category = "Landscape Settings")
	bool GeneratorBusy = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Landscape Settings")
	bool TileDataReady = false;	

	int PlayerSectionIndexX{};
	int PlayerSectionIndexY{};
	TArray<TArray<FVector>> SumVertices{};
	TArray<TArray<FVector2D>> SumUVs{};
	TArray<TArray<int32>> SumTriangles{};
	TArray<TArray<FVector>> SumNormals{};
	TArray<TArray<FProcMeshTangent>> SumTangents{};

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

	void SetCurrentMesh();
	void OnUpdatePlayerLocation(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);
	void SetGeneratorMesh();
	void UpdateTerrain();
public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	void GenerateTerrainAsync();
	void GenerateTerrain_Editor(int inSectionIndexX, int inSectionIndexY);

	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	FVector GetPlayerLocation() const;

	FIntPoint GetSectionIndex(FVector Location);
public:
};


class FAsyncWorldGenerater : public FNonAbandonableTask
{
	struct TrraninNode
	{
		int SectionIndexX{};
		int SectionIndexY{};
		TrraninNode(int inSectionIndexX, int inSectionIndexY)
			:SectionIndexX(inSectionIndexX), SectionIndexY(inSectionIndexY)
		{
		}
		TrraninNode() = default;
		bool operator==(const TrraninNode& Other) const
		{
			return SectionIndexX == Other.SectionIndexX && SectionIndexY == Other.SectionIndexY;
		}
		bool operator<(const TrraninNode& Other) const
		{
			if (SectionIndexX != Other.SectionIndexX)
			{
				return SectionIndexX < Other.SectionIndexX;
			}
			return SectionIndexY < Other.SectionIndexY;
		}
	};
public:
	FAsyncWorldGenerater(AWorldGenerator* InWorldGenerator)
	{
		WorldGenerator = InWorldGenerator;
		InitGenerater();
	}

	FORCEINLINE	TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncWorldGenerater, STATGROUP_ThreadPoolAsyncTasks);
	}

	void DoWork();

	void InitGenerater();
	void GenerateTerrainTile(const int inSectionIndexX, const int inSectionIndexY);
	float GetHeight(float inX, float inY) const;
	float PerlinNoiseExtended(float x, float y, float scale, float Amplitude, FVector2D offset = FVector2D::ZeroVector) const;

private:
	int MeshSectionIndex{};
private:
	UPROPERTY()
	TObjectPtr<AWorldGenerator> WorldGenerator{};
	FIntPoint PlayerSectionIndex{};
	TArray<int32> Triangles{};
};