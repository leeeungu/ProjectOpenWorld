// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "GeneratorSectionComponent.generated.h"

class UProceduralMeshComponent;
enum class EUpdateTransformFlags : int32;
enum class ETeleportType : uint8;
class FObjectPreSaveContext;
class UHierarchicalInstancedStaticMeshComponent;
class UInstancedStaticMeshComponent;
class UGenerateWorldComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnSectionTerrainGenerate, FIntPoint, SectionID, const TArray<FVector>&, Vertices, const TArray<FVector2D>&, UVs, const TArray<int32>&, Triangles, const TArray<FVector>&, Normals, const TArray<FProcMeshTangent >&, SumTangents);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSectionStartEvent, bool , Editor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSectionEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UGeneratorSectionComponent : public UActorComponent
{
	friend class FAsyncWorldGenerater;
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = "1"))
	int xVertexCount = 20;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = "1"))
	int yVertexCount = 20;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	float CellSize = 12000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = "2", ClampMax = "10"))
	int SectionRadiusCount = 3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = "1", ClampMax = "10"))
	int SectionUpdateTickCount = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TArray<float> scales{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TArray<float> amplitudes{};

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	bool bPropertyChaned = false;
#endif

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	int PlayerSectionIndexX{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	int PlayerSectionIndexY{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Landscape Settings")
	bool GeneratorBusy = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Landscape Settings")
	bool TileDataReady = false;

	TMap<FIntPoint, TArray<FVector>> SumVertices{};
	TMap<FIntPoint, TArray<FVector2D>> SumUVs{};
	TMap<FIntPoint, TArray<int32>> SumTriangles{};
	TMap<FIntPoint, TArray<FVector>> SumNormals{};
	TMap<FIntPoint, TArray<FProcMeshTangent>> SumTangents{};
	//TArray<TPair<FIntPoint, int>> SectionIndexArray{};
	TSet<FIntPoint> SectionMap{};
	TArray<FIntPoint> UpdateSectionArray{};
	int nDeleteSectionCount{};


	int MaxSection{};
	int CurrentIndex{};
	bool bDelayUpdate{};
public:	
	UGeneratorSectionComponent();

	FOnSectionStartEvent OnGenerateStart{};
	FOnSectionTerrainGenerate OnNewSection{};
	FOnSectionTerrainGenerate OnDeleteSection{};
	FOnSectionEvent OnGenerateFinished{};
public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	virtual void BeginPlay() override;

	void OnUpdatePlayerLocation(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);

	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	void GenerateTerrainAsync();

	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	FVector GetPlayerLocation() const;


	void StartGenerateTerrain(bool bEditor = false);
	void UpdateTerrain();
	void EndGenerateTerrain();

public:	
	FIntPoint GetSectionIndex(FVector Location) const;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void BindGenerteComponent(UGenerateWorldComponent* InGenerateWorldComponent);
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
	FAsyncWorldGenerater(UGeneratorSectionComponent* InWorldGenerator)
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
	static float PerlinNoiseExtended(float x, float y, float scale, float Amplitude, FVector2D offset = FVector2D::ZeroVector) ;

private:
	UPROPERTY()
	TObjectPtr<UGeneratorSectionComponent> WorldGenerator{};
	FIntPoint PlayerSectionIndex{};
	TArray<int32> Triangles{};
};