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

	int Max = FMath::Min(FoliageMaxCount, CurrentUpdateTick + UpdateTickCount);
	int nCount{};
	for (nCount; nCount < UpdateTickCount && CurrentUpdateTick < Max && arInstancesTranform.IsValidIndex(CurrentUpdateTick);)
	{
		int32 InstanceID = arInstancesTranform[CurrentUpdateTick].InstanceIndex;
		if (FoliageTypes.IsValidIndex(InstanceID))
		{
			const FFoliageDataTable* FoliageDataTableStruct = FoliageTypes[InstanceID];
			const TArray<FFoliageInstanceData>& InstanceData = FoliageDataTableStruct->InstanceData;
			int i = {};
			for (InstanceIndex; InstanceIndex < InstanceData.Num() && nCount < UpdateTickCount; InstanceIndex++)
			{
				const FFoliageInstanceData& Instance = InstanceData[InstanceIndex];
				TObjectPtr<UFoliageInstancedStaticMeshComponent>& FoliageInstance = FoliageTypeToInstanceIndex.FindOrAdd(Instance.FoliageType, nullptr);
				if (!FoliageInstance || !FoliageInstance.Get())
				{
					FoliageInstance = Cast<UFoliageInstancedStaticMeshComponent>(GetOwner()->AddComponentByClass(UFoliageInstancedStaticMeshComponent::StaticClass(), true, FTransform{}, false));
					FoliageInstance->SetupAttachment(GetOwner()->GetRootComponent());
					FoliageInstance->SetStaticMesh(Instance.FoliageType->GetStaticMesh());
				}
				if (FoliageInstance)
				{
					//if (InstanceIndex != 0)
					//	continue;
					FTransform NewTransform = arInstancesTranform[CurrentUpdateTick].InstanceTransform;
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
					//DrawDebugLine(GetWorld(), FVector(Location.X, Location.Y, Location.Z - 5000.0f), FVector(Location.X, Location.Y, Location.Z + 5000.0f), FColor::Red, false, 5.0f);
					Location.Z += FMath::RandRange(Instance.FoliageType->ZOffset.Min, Instance.FoliageType->ZOffset.Max);
					if (Location.Z < FoliageDataTableStruct->HeightRange.Min || Location.Z > FoliageDataTableStruct->HeightRange.Max)
					{
						continue;
					}
					NewTransform.SetLocation(Location);
					FRotator Rot = NewTransform.Rotator();
					NewTransform.SetScale3D(FVector(Instance.FoliageType->GetRandomScale()));
					FSectionData* SectionIndex = SectionIDToFoliageTypeToInstanceIndex.Find(arInstancesTranform[CurrentUpdateTick].SectionID);
					if (SectionIndex)
					{
						auto* RemoveIndexSet = RemoveInstanceIndex.Find(FoliageInstance);
						if (!RemoveIndexSet || (RemoveIndexSet && RemoveIndexSet->IsEmpty()))
						{
							if (nInstanceCount < FoliageMaxCount)
							{
								int32 Index = FoliageInstance->AddInstance(NewTransform, true);
								auto& NewIndexSet = SectionIndex->NewInstanceIndex.FindOrAdd(FoliageInstance, {});
								NewIndexSet.Add(Index);
								nInstanceCount++;
							}
						}
						else if (!RemoveIndexSet->IsEmpty())
						{
							int32 Index = RemoveIndexSet->Last();
							if (SectionIndex->bSectionNew)
							{
								Index = FoliageInstance->AddInstance(NewTransform, true);
								auto& NewIndexSet = SectionIndex->NewInstanceIndex.FindOrAdd(FoliageInstance, {});
								NewIndexSet.Add(Index);
								nInstanceCount++;
							}
							else
							{
								FoliageInstance->UpdateInstanceTransform(Index, NewTransform, true, true, true);
							}
							RemoveIndexSet->Pop();
						}
					}
					nCount++;

				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Foliage Instance is Null"));
				}
				i++;
			}
			if (InstanceData.Num() <= InstanceIndex)
			{
				CurrentUpdateTick++;
				InstanceIndex = 0;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FoliageTypes Invalid Index %d"), InstanceID);
		}
	}
	//bFoliageUpdated = CurrentUpdateTick < FoliageMaxCount;
	
}

void UGenerateFoliageComponent::ClearFoliageInstances()
{
	if (!DeleteArray.IsEmpty())
	{
		FIntPoint SectionID = DeleteArray.Last();
		if (FSectionData* pFoliageTypeToInstanceIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionID) )
		{
			if (!pFoliageTypeToInstanceIndex->NewInstanceIndex.IsEmpty() && !pFoliageTypeToInstanceIndex->bSectionNew)
			{
				for (auto& Data : pFoliageTypeToInstanceIndex->NewInstanceIndex)
				{
					TObjectPtr<UFoliageInstancedStaticMeshComponent>& FoliageInstance = Data.Key;
					if (FoliageInstance && !Data.Value.IsEmpty())
					{
						TArray<int32>& Arr = RemoveInstanceIndex.FindOrAdd(FoliageInstance, {});
						Arr.Append(Data.Value.Array());
						nInstanceCount -= Data.Value.Num();
						pFoliageTypeToInstanceIndex->NewInstanceIndex.Remove(Data.Key);
						return;
					}
				}
				pFoliageTypeToInstanceIndex->NewInstanceIndex.Empty();
				
			}
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
	ClearFoliageInstances();
}

void UGenerateFoliageComponent::StartGenerateWorld(bool bEditor)
{
	EditorModeGenerate = bEditor;
	if (EditorModeGenerate)
	{
		SectionIDToFoliageTypeToInstanceIndex.Empty();
	}
	arInstancesTranform.Empty();
	bFoliageUpdated = false;
		
}

void UGenerateFoliageComponent::NewGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	if (Vertices.Num() <= 1 || nInstanceCount >= FoliageMaxCount )
		return;
	FSectionData& SectionIndex = SectionIDToFoliageTypeToInstanceIndex.FindOrAdd(SectionID, {});
	if (SectionIndex.SectionIndex <= -1)
	{
		//UE_LOG(LogTemp, Warning, TEXT("New Section Index %d"), nSectionIndex);
		SectionIndex.SectionIndex = nSectionIndex;
		nSectionIndex++;
		FMath::RandInit(FDateTime::Now().GetMillisecond());
		SectionIndex.RandomSeed = FMath::Rand();
	}
	if (SectionIndex.bSectionNew)
		return;
	SectionIndex.bSectionNew = true;
	FMath::RandInit(SectionIndex.RandomSeed);
	int Rand = FMath::RandRange(SectionRange.X, SectionRange.Y);
	for (int i = 0; i < Rand; i++)
	{
		AddFoliageInstance(SectionID, *Vertices.begin(), Vertices.Last());
	}
}

void UGenerateFoliageComponent::DelGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	/*FSectionData* SectionIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionID);
	if (SectionIndex)
	{
		SectionIndex->bSectionNew = false;
	}
	DeleteArray.Add(SectionID);*/
}

void UGenerateFoliageComponent::FinishGenerateWorld()
{
	bFoliageUpdated = true;
	CurrentUpdateTick = 0;
	nSectionIndex = 0;
	InstanceIndex = 0;
	/*if (EditorModeGenerate)
	{
		while (bFoliageUpdated)
		{
			UpdateFoliageInstances();
		}
		EditorModeGenerate = false;
	}*/
}

void UGenerateFoliageComponent::Initialize(USceneComponent* ParentComponent)
{
	Super::Initialize(ParentComponent);
}
