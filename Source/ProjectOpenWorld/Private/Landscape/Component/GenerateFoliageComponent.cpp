#include "Landscape/Component/GenerateFoliageComponent.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "FoliageInstancedStaticMeshComponent.h"

UGenerateFoliageComponent::UGenerateFoliageComponent() :UGenerateWorldComponent{}
{
	PrimaryComponentTick.bCanEverTick = true;
	//GrassFoliageInstance = CreateDefaultSubobject< UInstancedStaticMeshComponent>(TEXT("GrassFoliageInstance"));
	FoliageInstances.Init(nullptr, 40);
}

void UGenerateFoliageComponent::BeginPlay()
{
	Super::BeginPlay();
	for (int FoliageID{}; FoliageID < FoliageTypes.Num(); FoliageID++)
	{
		if (FoliageInstances.IsValidIndex(FoliageID))
		{
			UFoliageInstancedStaticMeshComponent* Foliage = Cast< UFoliageInstancedStaticMeshComponent>(GetOwner()->AddComponentByClass(UFoliageInstancedStaticMeshComponent::StaticClass(), true, FTransform{}, false));
			Foliage->SetupAttachment(GetOwner()->GetRootComponent());
			Foliage->SetStaticMesh(FoliageTypes[FoliageID]->GetStaticMesh());
			FoliageInstances[FoliageID] = Foliage;
			//FoliageTypeToInstanceIndex.AddDefaulted();
		}
	}
	arrFoliageSectionIndex.Init({}, FoliageTypes.Num());
}

void UGenerateFoliageComponent::UpdateFoliageInstances()
{
	int Max = FMath::Min(FoliageMaxCount, CurrentUpdateTick + UpdateTickCount);
	for (CurrentUpdateTick; CurrentUpdateTick < Max&& arInstancesTranform.IsValidIndex(CurrentUpdateTick); CurrentUpdateTick++)
	{
		int32 FoliageID = arInstancesTranform[CurrentUpdateTick].FoliageID;
		TObjectPtr<UFoliageInstancedStaticMeshComponent>& FoliageInstance = FoliageInstances[FoliageID];
	//	FoliageSectionData& SectionData = FoliageTypeToInstanceIndex[FoliageID];
		if (FoliageInstance)
		{
			//TMap<int32, TSet<int32>>& FoliageTypeToInstanceIndex = SectionIDToFoliageTypeToInstanceIndex.FindOrAdd(arInstancesTranform[CurrentUpdateTick].SectionID);
			//TSet<int32>& IndexSet = FoliageTypeToInstanceIndex.FindOrAdd(FoliageID);
			//if (IndexSet.Find())
			//{
			//	if (TSet<int32>* IndexSet = FoliageTypeToInstanceIndex->Find(FoliageID))
			//	{
			//
			//	}
			//	FoliageTypeToInstanceIndex.
			//	/*if()
			//
			//	IndexSet.Add(CurrentUpdateTick);*/
			
			//FoliageInstance->section
				FoliageInstance->AddInstance(arInstancesTranform[CurrentUpdateTick].InstanceTransform, true);
			//else // if (UpdateData.InstanceIndex[UpdateData.Index]) != arInstancesTranform[CurrentUpdateTick].SectionID)
			//{
			//	FoliageInstance->UpdateInstanceTransform(CurrentUpdateTick, arInstancesTranform[CurrentUpdateTick].InstanceTransform, true, true);
			//}
		}
	}
	bFoliageUpdated = CurrentUpdateTick < FoliageMaxCount;
	SetComponentTickEnabled(bFoliageUpdated);
}

void UGenerateFoliageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bFoliageUpdated)
		UpdateFoliageInstances();
}

void UGenerateFoliageComponent::StartGenerateWorld()
{
	CurrentUpdateTick = 0;
	bFoliageUpdated = false;
	SetComponentTickEnabled(false);
	arInstancesTranform.Empty();
	DeleteArray.Empty();
	/*for (int FoliageID{}; FoliageID < FoliageTypes.Num(); FoliageID++)
	{
		if (FoliageInstances[FoliageID])
		{
			FoliageInstances[FoliageID]->ClearInstances();
		}
	}*/
}

void UGenerateFoliageComponent::NewGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	if (FoliageTypes.IsEmpty())
		return;
	if (FoliageMaxCount <= arInstancesTranform.Num())
		return;
	bFoliageUpdated = true;
	SetComponentTickEnabled(true);
	//int32& Index = SectionIDToFoliageTypeToInstanceIndex.FindOrAdd(SectionID, -1);
	//if (Index < 0)
	//{
	//	Index = nSectionIndex++;
	//}
	int Index{};
	for (FVector Vert : Vertices)
	{
		int32 FoliageID = FMath::Rand() % FoliageTypes.Num();
		const TObjectPtr<UFoliageType_InstancedStaticMesh>& FoliageTypeMesh = FoliageTypes[FoliageID];
		if (FoliageTypeMesh)
		{
			Vert.Z += FMath::RandRange(FoliageTypeMesh->ZOffset.Min, FoliageTypeMesh->ZOffset.Max);
			if (Vert.Z > 10.0f)
			{
				FFoliageData Data{};
				Data.InstanceTransform.SetLocation(Vert);
				//Normals[Index]
				FRotator Rot{};// = Normals[Index].Rotation();
				Rot.Yaw = FMath::RandRange(-360, 360);
				Data.InstanceTransform.SetRotation(Rot.Quaternion());
				Data.InstanceTransform.SetScale3D(FVector(FoliageTypeMesh->GetRandomScale()));
				Data.FoliageID = FoliageID;
				Data.SectionID = SectionID;
				arInstancesTranform.Add(Data);
			}
		}
		Index++;
	}
}

void UGenerateFoliageComponent::DelGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	DeleteArray.Add(SectionID);
	//if (TMap<int32, TSet<int32>>* FoliageTypeToInstanceIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionID))
	//{
	//	if (TSet<int32>& IndexSet = FoliageTypeToInstanceIndex.FindOrAdd(FoliageID))
	//	{
	//		IndexSet.Add(CurrentUpdateTick); */
	//	}
	//}
}
//
//void UGenerateFoliageComponent::UpdateGenerateWorld(FIntPoint SectionID, int SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
//{
//	
//}

void UGenerateFoliageComponent::FinishGenerateWorld()
{
}

void UGenerateFoliageComponent::Initialize(USceneComponent* ParentComponent)
{
	Super::Initialize(ParentComponent);
}
