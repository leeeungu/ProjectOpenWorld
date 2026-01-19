#include "Landscape/Component/GenerateFoliageComponent.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "FoliageInstancedStaticMeshComponent.h"

UGenerateFoliageComponent::UGenerateFoliageComponent() :UGenerateWorldComponent{}
{
	PrimaryComponentTick.bCanEverTick = true;
	if (FoliageDataTable)
	{
		FoliageDataTable->GetAllRows< FFoliageDataTable>(TEXT(""), FoliageTypes);
	}
}

void UGenerateFoliageComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGenerateFoliageComponent::UpdateFoliageInstances()
{
	if (arInstancesTranform.IsEmpty())
	{
		bFoliageUpdated = false;
		return;
	}
	FSectionData* SectionIndex = SectionIDToFoliageTypeToInstanceIndex.Find(arInstancesTranform.Last().SectionID);
	FFoliageData FoliageData = arInstancesTranform.Last();
	int32 CurrentInstanceIndex = FoliageData.InstanceIndex;
	FoliageData.SectionID;
	const FFoliageDataTable* FoliageDataTableStruct = FoliageTypes[CurrentInstanceIndex];
	const TArray<FFoliageInstanceData>& InstanceData = FoliageDataTableStruct->InstanceData;

	int Max = UpdateTickCount;
	for (InstanceIndex; InstanceIndex < InstanceData.Num() && Max > 0; InstanceIndex++)
	{
		const FFoliageInstanceData& Instance = InstanceData[InstanceIndex];
		TObjectPtr<UFoliageInstancedStaticMeshComponent>& FoliageInstance = FoliageTypeToInstanceIndex.FindOrAdd(Instance.FoliageType, nullptr);
		if (!FoliageInstance || !FoliageInstance.Get())
		{
			FoliageInstance = Cast<UFoliageInstancedStaticMeshComponent>(GetOwner()->AddComponentByClass(UFoliageInstancedStaticMeshComponent::StaticClass(), true, FTransform{}, false));
			FoliageInstance->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			FoliageInstance->SetStaticMesh(Instance.FoliageType->GetStaticMesh());
			RemoveInstanceIndex.Add(FoliageInstance, {});
		}
		TSet<int32>* RemoveIndexSet = RemoveInstanceIndex.Find(FoliageInstance);
		if (FoliageInstance && RemoveIndexSet)
		{
			FTransform NewTransform = FoliageData.InstanceTransform;
			FVector Location = NewTransform.GetLocation();
			Location += Instance.InstanceOffset;
			FHitResult HitResult;
			if (GetWorld()->LineTraceSingleByChannel(
				OUT HitResult,
				FVector(Location.X, Location.Y, Location.Z - 12000.0f),
				FVector(Location.X, Location.Y, Location.Z + 12000.0f),
				ECollisionChannel::ECC_Visibility,
				FCollisionQueryParams::DefaultQueryParam
			))
			{
				Location.Z = HitResult.Location.Z;
			}
			else if (!EditorModeGenerate)
			{
				continue;
			}
			Location.Z += FMath::RandRange(Instance.FoliageType->ZOffset.Min, Instance.FoliageType->ZOffset.Max);
			if (Location.Z < FoliageDataTableStruct->HeightRange.Min || Location.Z > FoliageDataTableStruct->HeightRange.Max)
			{
				continue;
			}
			NewTransform.SetLocation(Location);
			FRotator Rot = NewTransform.Rotator();
			NewTransform.SetScale3D(FVector(Instance.FoliageType->GetRandomScale()));
			int32 Index{};
			if (RemoveIndexSet->IsEmpty())
			{
				Index = FoliageInstance->AddInstance(NewTransform, true);
				nInstanceCount++;
				//UE_LOG(LogTemp, Warning, TEXT("Add Foliage Instance Total Count:%d"), nInstanceCount);
			}
			else
			{
				Index = *RemoveIndexSet->begin();
				RemoveIndexSet->Remove(Index);
				FoliageInstance->UpdateInstanceTransform(Index, NewTransform, true, true, true);
			}
			auto& NewIndexSet = SectionIndex->NewInstanceIndex.FindOrAdd(FoliageInstance, {});
			NewIndexSet.Add(Index);
			Max--;
		}
	}
	if (InstanceIndex >= InstanceData.Num())
	{
		//BuildTreeIfOutdated;
		arInstancesTranform.Pop();
		InstanceIndex = 0;
	}
}

void UGenerateFoliageComponent::ClearFoliageInstances()
{
	if (!DeleteArray.IsEmpty())
	{
		FIntPoint SectionID = DeleteArray.Last();
		if (FSectionData* pFoliageTypeToInstanceIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionID))
		{
				//UE_LOG(LogTemp, Warning, TEXT("Clear Foliage Instances SectionID X:%d Y:%d"), SectionID.X, SectionID.Y);
				for (auto& Data : pFoliageTypeToInstanceIndex->NewInstanceIndex)
				{
					TObjectPtr<UFoliageInstancedStaticMeshComponent>& FoliageInstance = Data.Key;
					if (FoliageInstance)
					{
						TSet<int32>* Arr = RemoveInstanceIndex.Find(FoliageInstance);
						if (Arr && !Data.Value.IsEmpty())
						{
							Arr->Append(Data.Value.Array());
							//nInstanceCount -= Data.Value.Num();
						}
					}
				}
				pFoliageTypeToInstanceIndex->NewInstanceIndex.Empty();
		}
		DeleteArray.Pop();
	}
}

void UGenerateFoliageComponent::AddFoliageInstance(FIntPoint SectionID, FVector StartPos, FVector EndPos)
{
	if (FoliageTypes.IsEmpty())
	{
		if (FoliageDataTable)
		{
			FoliageDataTable->GetAllRows< FFoliageDataTable>(TEXT(""), FoliageTypes);
		}
	}
	FFoliageData Data{};
	float RandX = FMath::RandRange(StartPos.X, EndPos.X);
	float RandY = FMath::RandRange(StartPos.Y, EndPos.Y);
	FVector RandPos{ RandX, RandY,  0 };
	Data.InstanceTransform.SetLocation(RandPos);
	Data.SectionID = SectionID;
	FRotator Rot{};
	Rot.Yaw = FMath::RandRange(-360, 360);
	Data.InstanceTransform.SetRotation(Rot.Quaternion());
	Data.InstanceIndex = FMath::Rand() % FoliageTypes.Num();
	arInstancesTranform.Add(Data);
}

void UGenerateFoliageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bFoliageUpdated)
	{
		UpdateFoliageInstances();
	}
}

void UGenerateFoliageComponent::StartGenerateWorld(bool bEditor)
{
	EditorModeGenerate = bEditor;
	if (EditorModeGenerate)
	{
		SectionIDToFoliageTypeToInstanceIndex.Empty();
	}
	bFoliageUpdated = false;
}

void UGenerateFoliageComponent::NewGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	if (Vertices.Num() <= 1 || nInstanceCount >= FoliageMaxCount )
		return;
	FSectionData* SectionIndex = SectionIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionID);
	if (!SectionIndex)
	{
		SectionIndex = &SectionIDToFoliageTypeToInstanceIndex.FindOrAdd(SectionID, {});
		SectionIndex->SectionIndex = nSectionIndex;
		nSectionIndex++;
		SectionIndex->RandomSeed = FoliageSeed;
		if (bRandomSeed)
		{
			FMath::RandInit(FDateTime::Now().GetMillisecond());
			SectionIndex->RandomSeed = FMath::Rand();
		}
		SectionIndex->bSectionNew = false;
	}
	if (SectionIndex->bSectionNew)
		return;
	SectionIndex->bSectionNew = true;
	SectionIndex->bAllReadyNew = false;
	FMath::RandInit(SectionIndex->RandomSeed);
	int Rand = FMath::RandRange(SectionRange.X, SectionRange.Y);
	for (int i = 0; i < Rand; i++)
	{
		AddFoliageInstance(SectionID, *Vertices.begin(), Vertices.Last());
	}
}

void UGenerateFoliageComponent::DelGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	FSectionData* SectionIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionID);
	if (SectionIndex && SectionIndex->bSectionNew)
	{
		SectionIndex->bSectionNew = false;
		DeleteArray.Add(SectionID);
		ClearFoliageInstances();
	}
}

void UGenerateFoliageComponent::FinishGenerateWorld()
{
	bFoliageUpdated = true;
	nSectionIndex = 0;
	InstanceIndex = 0;
}

void UGenerateFoliageComponent::Initialize(USceneComponent* ParentComponent)
{
	Super::Initialize(ParentComponent);
}
