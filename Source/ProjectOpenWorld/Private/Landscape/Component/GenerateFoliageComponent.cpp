#include "Landscape/Component/GenerateFoliageComponent.h"
#include "Landscape/Component/GeneratorSectionComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

UGenerateFoliageComponent::UGenerateFoliageComponent() :UGenerateWorldComponent{}
{
	PrimaryComponentTick.bCanEverTick = false;
	GrassFoliageInstance = CreateDefaultSubobject< UInstancedStaticMeshComponent>(TEXT("GrassFoliageInstance"));
	GrassFoliageInstance->ClearInstanceSelection();
	//int i = 0;
	//for (const FFoliageData& Data : FoliageData)
	//{
	//	UInstancedStaticMeshComponent* FoliageInstance = Cast<UInstancedStaticMeshComponent>(CreateDefaultSubobject(*FString::Printf(TEXT("FoliageInstance_%d"), i), Data.InstanceMeshClass, UInstancedStaticMeshComponent::StaticClass(), true, true));
	//	FoliageInstances.Add(FoliageInstance);
	//	FoliageInstance->SetStaticMesh(Data.FoliageMesh);
	//	i++;
	//}
}

void UGenerateFoliageComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGenerateFoliageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGenerateFoliageComponent::StartGenerateWorld()
{
	GrassFoliageInstance->ClearInstances();
}

void UGenerateFoliageComponent::UpdateGenerateWorld(int SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	//for (const FVector& Vert : Vertices)
	//{
	//	int32 Rand = (FMath::PerlinNoise2D(FVector2D(Vert.X, Vert.Y)* 0.0001f) + 1) *10000;
	//		Rand = Rand % 255;
	//	if (0 < Rand && Rand <= FoliageType && Vert.Z > 10)
	//	{
	//		if (GrassFoliageInstance)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("UGenerateFoliageComponent Perlin Noise at %s , Rand=%d"), *Vert.ToString(), Rand);
	//			//Section->SetStaticMesh(FoliageData[0].FoliageMesh);
	//			FTransform InstanceTransform{};
	//			InstanceTransform.SetLocation(Vert);
	//			//InstanceTransform.SetRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, Rand % 360)));
	//			float ScaleValue = 0.5f + (Rand % 50) / 100.f;
	//			InstanceTransform.SetScale3D(FVector(ScaleValue));
	//			GrassFoliageInstance->AddInstance(InstanceTransform, true);
	//		}
	//	}
	//}
	//for ( const FVector& Vert : Vertices)
	//{
	//	int32 Rand = FAsyncWorldGenerater::PerlinNoiseExtended(Vert.X, Vert.Y, 0.1f, 255);
	//	if (0< Rand  && Rand <= FoliageType)
	//	{
	//		UInstancedStaticMeshComponent* Section{};
	//		if (!FoliageInstances.IsValidIndex(0))
	//		{
	//			Section = NewObject< UInstancedStaticMeshComponent>(this, FoliageData[0].InstanceMeshClass);
	//			FoliageInstances.Add(Section);
	//		}
	//		else
	//		{
	//			Section = FoliageInstances[0];
	//		}
	//		if (Section && Section->GetNumInstances() < 100)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("UGenerateFoliageComponent Add Foliage at %s"), *Vert.ToString());
	//			Section->SetStaticMesh(FoliageData[0].FoliageMesh);
	//			FTransform InstanceTransform{};
	//			InstanceTransform.SetLocation(Vert);
	//			//InstanceTransform.SetRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, Rand % 360)));
	//			float ScaleValue = 0.5f + (Rand % 50) / 100.f;
	//			InstanceTransform.SetScale3D(FVector(ScaleValue));
	//			Section->AddInstance(InstanceTransform, true);
	//		}
	//	}
	//}
}

void UGenerateFoliageComponent::Initialize(USceneComponent* ParentComponent)
{
	Super::Initialize(ParentComponent);
	//for (UInstancedStaticMeshComponent* Section : FoliageInstances)
	//{
	//	if (Section)
	//	{
	//		Section->SetupAttachment(ParentComponent);
	//	}
	//}
	if (GrassFoliageInstance)
	{
		GrassFoliageInstance->SetupAttachment(ParentComponent);
	}
}
