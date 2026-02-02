// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FoliageEditorActor.generated.h"

struct FFoliageDataTable;
class UFoliageInstancedStaticMeshComponent;
class UFoliageType_InstancedStaticMesh;

USTRUCT(BlueprintType)
struct FFoliageEditorData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Foliage")
	TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageType{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Foliage")
	TArray<FTransform> InstanceData{};

};

UCLASS()
class PROJECTOPENWORLD_API AFoliageEditorActor : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UDataTable> FoliageDataTable{}; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = 0))
	int32 FoliageCount{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = 0))
	FName InstanceRowName{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	bool bGenerateFoliage{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	bool bAddToDataTable{};
	
	UPROPERTY(EditInstanceOnly, Category = "Landscape Settings")
	TMap<TObjectPtr<UFoliageType_InstancedStaticMesh> , TObjectPtr<UFoliageInstancedStaticMeshComponent>> FoliageComponents{};
	TArray<	TObjectPtr<UFoliageInstancedStaticMeshComponent>> FoliageMeshData{};
	TMap<FName, FFoliageDataTable*> FoliageTypes{};
	int nCount{};
public:	
	AFoliageEditorActor();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
protected:
	virtual void BeginPlay() override;

	void SetFoliageMeshComponent(TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageMesh, TObjectPtr< UFoliageInstancedStaticMeshComponent> MeshComp);
	void CreateFoliageInstance();
	void ClearFoliageInstance();
};
