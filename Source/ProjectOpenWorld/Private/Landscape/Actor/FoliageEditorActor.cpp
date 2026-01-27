#include "Landscape/Actor/FoliageEditorActor.h"
#include "Landscape/Component/GenerateFoliageComponent.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "FoliageInstancedStaticMeshComponent.h"

AFoliageEditorActor::AFoliageEditorActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
}

void AFoliageEditorActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (FoliageDataTable)
	{
		FoliageTypes.Empty(false);
		TArray<FName> FoliageTypeNames = FoliageDataTable->GetRowNames();
		for (int i = 0; i < FoliageTypeNames.Num(); i++)
		{
			FoliageTypes.Add(FoliageTypeNames[i], FoliageDataTable->FindRow<FFoliageDataTable>(FoliageTypeNames[i], TEXT("")));
		}
	}

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AFoliageEditorActor, InstanceRowName))
	{
		if(!FoliageTypes.Find(InstanceRowName))
			InstanceRowName = NAME_None;
		else
		{
			if (bGenerateFoliage)
			{
				ClearFoliageInstance();
				CreateFoliageInstance();
			}
		}
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AFoliageEditorActor, bGenerateFoliage))
	{
		if (bGenerateFoliage)
		{
			ClearFoliageInstance();
			CreateFoliageInstance();
		}
		else
		{
			ClearFoliageInstance();
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
		
void AFoliageEditorActor::SetFoliageMeshComponent(TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageMesh, TObjectPtr< UFoliageInstancedStaticMeshComponent> MeshComp)
{
	if (FoliageMesh && MeshComp)
	{
		MeshComp->SetStaticMesh(FoliageMesh->GetStaticMesh());
		MeshComp->SetCastShadow(FoliageMesh->CastShadow);
		MeshComp->bReceivesDecals = FoliageMesh->bReceivesDecals;
		MeshComp->SetCullDistances(FoliageMesh->CullDistance.Min, FoliageMesh->CullDistance.Max);
		MeshComp->SetCollisionProfileName(FoliageMesh->BodyInstance.GetCollisionProfileName());
		MeshComp->SetVisibleInRayTracing(FoliageMesh->bVisibleInRayTracing);
	}
}

void AFoliageEditorActor::CreateFoliageInstance()
{
	if (FoliageTypes.Find(InstanceRowName))
	{
		const FFoliageDataTable* SelectedFoliageType = FoliageTypes[InstanceRowName];
		if (SelectedFoliageType)
		{
			for (const FFoliageInstanceData& Foliage : SelectedFoliageType->InstanceData)
			{
				TObjectPtr<UFoliageInstancedStaticMeshComponent>& Component = FoliageComponents.FindOrAdd(Foliage.FoliageType, nullptr);
				if (!Component)
				{
					Component = NewObject<UFoliageInstancedStaticMeshComponent>(this);
					Component->RegisterComponent();
					Component->SetStaticMesh(Foliage.FoliageType->Mesh);
					Component->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
					SetFoliageMeshComponent(Foliage.FoliageType, Component);
				}
				Component->AddInstance(FTransform(Foliage.InstanceOffset));
			}
		}
	}
}

void AFoliageEditorActor::ClearFoliageInstance()
{
	for (auto& Foliage : FoliageComponents)
	{
		if (Foliage.Value)
		{
			Foliage.Value->ClearInstances();
		}
	}
	FoliageComponents.Empty(false);
}

void AFoliageEditorActor::BeginPlay()
{
	Super::BeginPlay();
}