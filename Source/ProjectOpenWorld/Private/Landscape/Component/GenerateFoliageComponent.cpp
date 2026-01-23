#include "Landscape/Component/GenerateFoliageComponent.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "FoliageInstancedStaticMeshComponent.h"

UGenerateFoliageComponent::UGenerateFoliageComponent() :UGenerateWorldComponent{}
{
	PrimaryComponentTick.bCanEverTick = true;
	FoliageMeshData.Empty(true);
	FoliageMeshData.Reserve(FoliageComponentCount);
	for (int i = 0; i < FoliageComponentCount; i++)
	{
		UFoliageInstancedStaticMeshComponent* FoliageCompoent =
			CreateDefaultSubobject<UFoliageInstancedStaticMeshComponent>(FName(*FString::Printf(TEXT("FoliageInstancedStaticMeshComponent_%d"), i)));
		if (FoliageCompoent)
		{
			FoliageMeshData.Add(FoliageCompoent);
		}
	}
	FoliageCount = FoliageMeshData.Num();
}

void UGenerateFoliageComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UGenerateFoliageComponent, FoliageDataTable))
	{
		if (FoliageDataTable)
		{
			FoliageTypes.Empty(false);
			FoliageDataTable->GetAllRows< FFoliageDataTable>(TEXT(""), FoliageTypes);
		}
	}
	FoliageCount = FoliageMeshData.Num();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UGenerateFoliageComponent::BeginPlay()
{
	Super::BeginPlay();
	if (FoliageDataTable)
	{
		DeleteArray.Empty(false);
		AddMap.Empty(false);
		UpdateData.Empty(false);
		FoliageTypes.Empty(false);
		FoliageDataTable->GetAllRows< FFoliageDataTable>(TEXT(""), FoliageTypes);
	}

	bDelayUpdate = false;
	EmpthyFoliageMeshData.Empty(false);
	for (int i = 0; i < FoliageMeshData.Num(); i++)
	{
		UFoliageInstancedStaticMeshComponent* FoliageCompoent = FoliageMeshData[i];
		if (FoliageCompoent)
		{
			FoliageCompoent->ClearInstances();
			EmpthyFoliageMeshData.Add(FoliageCompoent);
		}
	}
}

void UGenerateFoliageComponent::SetFoliageMeshComponent(TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageMesh, TObjectPtr<UFoliageInstancedStaticMeshComponent> MeshComp)
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

void UGenerateFoliageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!IsGenerating() && bDelayUpdate)
	{
		if (!bGeneratingFoliage && !bUpdateBackData && !EditorModeGenerate)
		{
			UE_LOG(LogTemp, Warning, TEXT("Start Generate Foliage SectionID "));
			bDelayUpdate = false;
			PlayerLastSectionID = GeneratorSectionComponent->GetPlayerSection();
			bGeneratingFoliage = false;
			bUpdateBackData = false;
			//UpdateBuffer = false;
			AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [&]()
				{
					FAsyncTask< FAsyncFoliageGenerater>* WorldGenTask = new FAsyncTask< FAsyncFoliageGenerater>(this);
					WorldGenTask->StartBackgroundTask();
					WorldGenTask->EnsureCompletion();
					delete WorldGenTask;
				}
			);
		}
	}
	if (bGeneratingFoliage)
	{
		if (bUpdateBackData)
		{
			bGeneratingFoliage = false;
		}
	}
	else
	{
		if (bUpdateBackData)
		{
			int Max = UpdateComponentTickCount;
			while (!UpdateData.IsEmpty() && Max > 0 && !EmpthyFoliageMeshData.IsEmpty())
			{
				Max--;
				FoliageUpdateData& Data = UpdateData.Last();
				FoliageSectionData* SectionData = &FoliageSectionDataMap.FindOrAdd(Data.SectionID);
				if (SectionData)
				{
					if (Data.bRemove)
					{

						if (SectionData)
						{
							for (auto& Instance : SectionData->FoliageInstanceData)
							{
								Instance.Value->ClearInstances();
								Instance.Value->SetStaticMesh(nullptr);
								EmpthyFoliageMeshData.Push(Instance.Value);
							}
							SectionData->FoliageInstanceData.Empty(false);
						}
						nUpdateTickIndex = Data.FoliageData.Num() + 1;
					}
					else
					{
						//FoliageStaticMeshData* StaticMesh = FoliageComponentMap.Find(Data.FoliageMesh))
						FTransform NewTransform = Data.FoliageData[nUpdateTickIndex];
						FVector Location = NewTransform.GetLocation();
						FHitResult HitResult;
						if (GetWorld()->LineTraceSingleByChannel(
							OUT HitResult,
							FVector(Location.X, Location.Y, Location.Z - 12000.0f),
							FVector(Location.X, Location.Y, Location.Z + 12000.0f),
							ECollisionChannel::ECC_Visibility,
							FCollisionQueryParams::DefaultQueryParam
						))
						{
							Location.Z += HitResult.ImpactPoint.Z;
							float SlopeAngle = FMath::RadiansToDegrees(FVector::DotProduct(-FVector::UpVector, HitResult.ImpactNormal));
							if (Data.FoliageMesh->AlignToNormal)
							{
								NewTransform.SetRotation(FQuat(FVector::ForwardVector.Rotation()));
							}
							NewTransform.SetLocation(Location);
							//UE_LOG(LogTemp, Warning, TEXT("SlopeAngle : %f %f, %f"), Location.Z, Data.FoliageMesh->ZOffset.Min, Data.FoliageMesh->ZOffset.Max);
							if (Data.FoliageMesh->Height.Contains(Location.Z))
							{
								TObjectPtr<UFoliageInstancedStaticMeshComponent>& MeshComponent = SectionData->FoliageInstanceData.FindOrAdd(Data.FoliageMesh, nullptr);
								if (!MeshComponent)
								{
									MeshComponent = EmpthyFoliageMeshData.Last();
									EmpthyFoliageMeshData.Pop();
									SetFoliageMeshComponent(Data.FoliageMesh, MeshComponent);
								}
								MeshComponent->AddInstance(NewTransform, true);
								nInstanceCount++;
							}
						}
						nUpdateTickIndex++;
					}
				}

				if (!Data.FoliageData.IsValidIndex(nUpdateTickIndex))
				{
					nUpdateTickIndex = 0;
					UpdateData.Pop();
				}
			}
			if (UpdateData.IsEmpty())
			{

				UE_LOG(LogTemp, Warning, TEXT("Finish Generate Foliage SectionID "));
				bUpdateBackData = false;
			}
		}
	}
}

void UGenerateFoliageComponent::StartGenerateWorld(bool bEditor)
{
	Super::StartGenerateWorld(bEditor);
	EditorModeGenerate = bEditor;
	/*if (EditorModeGenerate)
	{
		nSectionIndex = 0;
		for (int i = 0; i < FoliageComponentCount; i++)
		{
			UFoliageInstancedStaticMeshComponent* FoliageCompoent = FoliageComponentArray[i];
			if (FoliageCompoent)
			{
				FoliageCompoent->ClearInstances();
				EmpthyComponentArray.Enqueue(FoliageCompoent);
			}
		}
		SectionIDToFoliageTypeToInstanceIndex.Empty();
	}*/
}

void UGenerateFoliageComponent::NewGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (SectionData.Vertices->Num() <= 1 || EditorModeGenerate)
		return;

	FoliageSectionData* FoliageSectionData = FoliageSectionDataMap.Find(SectionData.SectionID);
	if (!FoliageSectionData)
	{
		float RandomSeed = FoliageSeed * nSectionCount;
		if (bRandomSeed)
		{
			FMath::RandInit(FDateTime::Now().GetMillisecond());
			RandomSeed = FMath::Rand();
		}
		FoliageSectionData = &FoliageSectionDataMap.Add(SectionData.SectionID);
		FoliageSectionData->RandomSeed = RandomSeed;
		FoliageSectionData->SectionIndex = nSectionCount;
		nSectionCount++;
	}

	FoliageAddData& AddData = AddMap.FindOrAdd(SectionData.SectionID);
	AddData.StartPos = *SectionData.Vertices->begin();
	AddData.EndPos = SectionData.Vertices->Last();
}

void UGenerateFoliageComponent::DelGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (EditorModeGenerate)
		return;
	FoliageAddData& AddData = AddMap.FindOrAdd(SectionData.SectionID);
	AddData.bRemove = true;

}

void UGenerateFoliageComponent::FinishGenerateWorld()
{
	Super::FinishGenerateWorld();
	bDelayUpdate = true;
}

void UGenerateFoliageComponent::Initialize(USceneComponent* ParentComponent)
{
	Super::Initialize(ParentComponent);
	for (int i = 0; i < FoliageComponentCount; i++)
	{
		UFoliageInstancedStaticMeshComponent* FoliageCompoent = FoliageMeshData[i];
		if (FoliageCompoent)
		{
			FoliageCompoent->SetupAttachment(ParentComponent);
		}
	}
}

void FAsyncFoliageGenerater::Initialize()
{
	//DeleteArray = FoliageGenerater->DeleteArray;
	AddMap = FoliageGenerater->AddMap;
	FoliageGenerater->AddMap.Empty(false);
	//FoliageGenerater->DeleteArray.Empty(false);
}

void FAsyncFoliageGenerater::DoWork()
{
	using FoliageAddData = UGenerateFoliageComponent::FoliageAddData;
	using FoliageSectionData = UGenerateFoliageComponent::FoliageSectionData;
	using FoliageUpdateData = UGenerateFoliageComponent::FoliageUpdateData;
	TArray<FFoliageDataTable*>& FoliageTypes = FoliageGenerater->FoliageTypes;
	if (FoliageTypes.Num() <= 0)
		return;

	TMap<FIntPoint, FoliageSectionData>& FoliageSectionDataMap = FoliageGenerater->FoliageSectionDataMap;
	TMap<FIntPoint, TMap< TObjectPtr<UFoliageType_InstancedStaticMesh>, FoliageUpdateData>> UpdateSectionMeshData{};
	{
		TArray<FIntPoint> KeyArray{};
		AddMap.GenerateKeyArray(KeyArray);
		while (FoliageGenerater && !KeyArray.IsEmpty())
		{
			FIntPoint SectionID = KeyArray.Last();
			FoliageAddData& AddData = *AddMap.Find(SectionID);
			KeyArray.Pop(false);
			// 색션 Data
			if (const FoliageSectionData* SectionData = FoliageSectionDataMap.Find(SectionID))
			{
				FMath::RandInit(SectionData->RandomSeed);
				int Rand = FMath::RandRange(FoliageGenerater->FoliageDataCreateRange.X, FoliageGenerater->FoliageDataCreateRange.Y);
				FVector StartPos = AddData.StartPos;
				FVector EndPos = AddData.EndPos;
				// FFoliageDataTable  범위 내 인스턴스 조합 랜덤 생성
				for (int CurrentRnadIndex = 0; CurrentRnadIndex < Rand; CurrentRnadIndex++)
				{
					int32 CurrentInstanceIndex = FMath::Rand() % FoliageTypes.Num();
					const FFoliageDataTable* FoliageDataTableStruct = FoliageTypes[CurrentInstanceIndex];
					const TArray<FFoliageInstanceData>& FoliageInstanceArray = FoliageDataTableStruct->InstanceData;
					FVector RandPos{ FMath::RandRange(StartPos.X, EndPos.X), FMath::RandRange(StartPos.Y, EndPos.Y),  0 };
					// 색션 내의 랜덤 위치 선택
					for (int InstanceIndex = 0; InstanceIndex < FoliageInstanceArray.Num(); InstanceIndex++)
					{
						// Data
						const TObjectPtr<UFoliageType_InstancedStaticMesh>& FoliageType = FoliageInstanceArray[InstanceIndex].FoliageType;
						const FVector& InstanceOffset = FoliageInstanceArray[InstanceIndex].InstanceOffset;
						FoliageUpdateData& UpdateData = UpdateSectionMeshData.FindOrAdd(SectionID).FindOrAdd(FoliageType);
						UpdateData.SectionID = SectionID;
						UpdateData.FoliageMesh = FoliageType;
						UpdateData.bRemove = AddData.bRemove;

						// Generate Random Transform
						FTransform NewTransform{};
						NewTransform.SetLocation(RandPos);
						FRotator Rot{};
						if (FoliageType->RandomYaw)
							Rot.Yaw = FMath::RandRange(-360, 360);
						Rot.Pitch = FMath::RandRange(-FoliageType->RandomPitchAngle, FoliageType->RandomPitchAngle);
						NewTransform.SetRotation(Rot.Quaternion());
						FVector Location = NewTransform.GetLocation();
						Location += InstanceOffset;

						// 색션 밖으로 나가면 지형은 없는데 instance는 생기게 되니 제거
						// clamp 하면 낮은 확률로 섹션 경계에 instance가 모여서 생길수도 있음
						FBox SectionBox{ StartPos, EndPos };
						bool bInSection = SectionBox.IsInsideOrOnXY(Location);
						Location.Z += FMath::RandRange(FoliageType->ZOffset.Min, FoliageType->ZOffset.Max);
						NewTransform.SetLocation(Location);
						NewTransform.SetScale3D(FVector(FoliageType->GetRandomScale()));

						// New Data
						if (bInSection)
							UpdateData.FoliageData.Add(NewTransform);
					}
				}
			}
		}
		AddMap.Empty();
	}

	for (auto& UpdateMap : UpdateSectionMeshData)
	{
		for (auto& UpdateData : UpdateMap.Value)
		{
			FoliageUpdateData& NewUpdateData = UpdateData.Value;
			FoliageGenerater->UpdateData.Add(NewUpdateData);
		}
	}

	if (FoliageGenerater)
	{
		FoliageGenerater->bGeneratingFoliage = true;
		FoliageGenerater->bUpdateBackData = true;
	}
}