#include "Landscape/Component/GenerateFoliageComponent.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "FoliageInstancedStaticMeshComponent.h"

UGenerateFoliageComponent::UGenerateFoliageComponent() :UGenerateWorldComponent{}
{
	PrimaryComponentTick.bCanEverTick = true;
	FoliageComponentArray.Empty(false);
	FoliageComponentArray.Reserve(FoliageComponentCount);
	for (int i = 0; i < FoliageComponentCount; i++)
	{
		UFoliageInstancedStaticMeshComponent* FoliageCompoent = CreateDefaultSubobject<UFoliageInstancedStaticMeshComponent>(FName(*FString::Printf(TEXT("FoliageInstancedStaticMeshComponent_%d"), i)));
		if (FoliageCompoent)
		{
			FoliageComponentArray.Add(FoliageCompoent);
			EmpthyComponentArray.Enqueue(FoliageCompoent);
		}
	}
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
	nSectionIndex = 0;
	for (int i = 0; i < FoliageComponentCount; i++)
	{
		UFoliageInstancedStaticMeshComponent* FoliageCompoent = FoliageComponentArray[i];
		if (FoliageCompoent)
		{
			EmpthyComponentArray.Enqueue(FoliageCompoent);
			FoliageCompoent->ClearInstances();
		}
	}
	SectionIDToFoliageTypeToInstanceIndex.Empty();
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
			for (FIntPoint& SectionID : DeleteArray)
			{
				if (FoliageSectionData* pFoliageTypeToInstanceIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionID))
				{
					for (TObjectPtr< UFoliageInstancedStaticMeshComponent>& MeshComp : pFoliageTypeToInstanceIndex->GetAllFoliageMeshComponent_Async())
					{
						if (MeshComp)
						{
							nInstanceCount -= MeshComp->GetNumInstances();
							EmpthyComponentArray.Enqueue(MeshComp);
							MeshComp->ClearInstances();
							EmptyComponentCount++;
						}
					}
					pFoliageTypeToInstanceIndex->Clear_Async();
				}
			}
			DeleteArray.Empty(false);
		}
	}
	else
	{
		if (bUpdateBackData)
		{
			int Max = UpdateComponentTickCount;
			while (!UpdateData.IsEmpty() && Max > 0)
			{
				Max--;
				FoliageUpdateData& Data = UpdateData.Last();
				if (Data.bAdd)
				{
					Data.bAdd = false;
					DeleteArray.Remove(Data.SectionID);
					SetFoliageMeshComponent(Data.FoliageMesh, Data.MeshComponent);
					//UE_LOG(LogTemp, Warning, TEXT("Start Generate Foliage SectionID %d,%d %s"), Data.SectionID.X, Data.SectionID.Y,* Data.MeshComponent->GetName());
					nUpdateTickIndex = 0;
				}
				if (Data.MeshComponent && Data.FoliageData.IsValidIndex(nUpdateTickIndex))
				{
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
						//UE_LOG(LogTemp, Warning, TEXT("SlopeAngle : %f %f, %f"), Location.Z, Data.FoliageMesh->ZOffset.Min, Data.FoliageMesh->ZOffset.Max);
						if (Data.FoliageMesh->Height.Contains(Location.Z))
						{
							NewTransform.SetLocation(Location);
							Data.MeshComponent->AddInstance(NewTransform, true);
							nInstanceCount++;
						}
					}
					nUpdateTickIndex++;
				}

				if (!Data.FoliageData.IsValidIndex(nUpdateTickIndex))
				{
					if (Data.MeshComponent)
					{
						//UE_LOG(LogTemp, Warning, TEXT("Finish Generate Foliage SectionID %d,%d %s with %d Instances"),
						//	Data.SectionID.X, Data.SectionID.Y, *Data.MeshComponent->GetName(), Data.MeshComponent->GetNumInstances());
						if (UpdateBackData.Find(Data.SectionID))
						{
							UpdateBackData.Find(Data.SectionID)->Remove(Data.MeshComponent);
						}
					}
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
	if (EditorModeGenerate)
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
	}
}

void UGenerateFoliageComponent::NewGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (SectionData.Vertices->Num() <= 1 || EditorModeGenerate)
		return;

	FoliageSectionData* FoliageSectionData = SectionIDToFoliageTypeToInstanceIndex.Find(SectionData.SectionID);
	if (!FoliageSectionData)
	{
		float RandomSeed = FoliageSeed * nSectionIndex;
		if (bRandomSeed)
		{
			FMath::RandInit(FDateTime::Now().GetMillisecond());
			RandomSeed = FMath::Rand();
		}
		FoliageSectionData = &SectionIDToFoliageTypeToInstanceIndex.Add(SectionData.SectionID, { nSectionIndex , RandomSeed });
		nSectionIndex++;
	}

	FoliageAddData& AddData = AddMap.FindOrAdd(SectionData.SectionID);
	AddData.StartPos = *SectionData.Vertices->begin();
	AddData.EndPos = SectionData.Vertices->Last();
	DeleteArray.Remove(SectionData.SectionID);
}

void UGenerateFoliageComponent::DelGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (EditorModeGenerate )
		return;

	FoliageSectionData* FoliageSectionData = SectionIDToFoliageTypeToInstanceIndex.Find(SectionData.SectionID);
	if (FoliageSectionData)// && FoliageSectionData->GetAlreadyGenerate())
	{
		DeleteArray.Add(SectionData.SectionID);
		AddMap.Remove(SectionData.SectionID);
		//UE_LOG(LogTemp, Warning, TEXT("Del Generate Foliage SectionID %d,%d"), SectionData.SectionID.X, SectionData.SectionID.Y);
	}
}

void UGenerateFoliageComponent::FinishGenerateWorld()
{
	Super::FinishGenerateWorld();
	bDelayUpdate  = true;
}

void UGenerateFoliageComponent::Initialize(USceneComponent* ParentComponent)
{
	Super::Initialize(ParentComponent);
	for (int i = 0; i < FoliageComponentCount; i++)
	{
		UFoliageInstancedStaticMeshComponent* FoliageCompoent = FoliageComponentArray[i];
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

	TMap<FIntPoint, FoliageSectionData>& SectionIDToFoliageTypeToInstanceIndex = FoliageGenerater->SectionIDToFoliageTypeToInstanceIndex;
	TQueue<TObjectPtr<UFoliageInstancedStaticMeshComponent>>& EmpthyComponentArray = FoliageGenerater->EmpthyComponentArray;
	TMap<FIntPoint, TMap<TObjectPtr<UFoliageType_InstancedStaticMesh>, FoliageUpdateData>> UpdateBySectionID{};
	{
		//for (FIntPoint& SectionID : DeleteArray)
		//{
		//	TMap<TObjectPtr<UFoliageType_InstancedStaticMesh>, FoliageUpdateData>& UpdateData = UpdateBySectionID.FindOrAdd(SectionID);
		//	if (FoliageSectionData* pFoliageTypeToInstanceIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionID))
		//	{
		//		TArray<TObjectPtr< UFoliageType_InstancedStaticMesh>> FoliageMeshArray = pFoliageTypeToInstanceIndex->GetAllFoliageMeshType_Async();
		//		for (TObjectPtr< UFoliageType_InstancedStaticMesh>& Mesh : FoliageMeshArray)
		//		{
		//			FoliageUpdateData& FoliageUpdateData = UpdateData.FindOrAdd(Mesh); // .bClear = true;
		//			FoliageUpdateData.SectionID = SectionID;
		//			FoliageUpdateData.bClear = true;
		//			FoliageUpdateData.FoliageMesh = Mesh;
		//			if (!EmpthyComponentArray.IsEmpty() && !FoliageUpdateData.MeshComponent)
		//			{
		//				FoliageUpdateData.MeshComponent = EmpthyComponentArray.Last();
		//				EmpthyComponentArray.Pop();
		//			}
		//		}
		//		EmpthyComponentArray.Append(pFoliageTypeToInstanceIndex->GetAllFoliageMeshComponent_Async());
		//		pFoliageTypeToInstanceIndex->Clear_Async();
		//	}
		//}
		//DeleteArray.Empty(false);
	}
	{
		TArray<FIntPoint> KeyArray{};
		AddMap.GenerateKeyArray(KeyArray);
		while (FoliageGenerater && !KeyArray.IsEmpty())
		{
			FIntPoint SectionID = KeyArray.Last();
			FoliageAddData& AddData = *AddMap.Find(SectionID);
			KeyArray.Pop(false);
			if (FoliageSectionData* SectionData = SectionIDToFoliageTypeToInstanceIndex.Find(SectionID))
			{
				FMath::RandInit(SectionData->GetRandomSeed_Async());
				int Rand = FMath::RandRange(FoliageGenerater->FoliageDataCreateRange.X, FoliageGenerater->FoliageDataCreateRange.Y);
				FVector StartPos = AddData.StartPos;
				FVector EndPos = AddData.EndPos;

				for (int CurrentRnadIndex = 0; CurrentRnadIndex < Rand; CurrentRnadIndex++)
				{
					int32 CurrentInstanceIndex = FMath::Rand() % FoliageTypes.Num();
					const FFoliageDataTable* FoliageDataTableStruct = FoliageTypes[CurrentInstanceIndex];
					const TArray<FFoliageInstanceData>& FoliageInstanceArray = FoliageDataTableStruct->InstanceData;
					FVector RandPos{ FMath::RandRange(StartPos.X, EndPos.X), FMath::RandRange(StartPos.Y, EndPos.Y),  0 };


					for (int InstanceIndex = 0; InstanceIndex < FoliageInstanceArray.Num(); InstanceIndex++)
					{
						// Data
						const TObjectPtr<UFoliageType_InstancedStaticMesh>& FoliageType = FoliageInstanceArray[InstanceIndex].FoliageType;
						FoliageUpdateData& UpdateData = UpdateBySectionID.FindOrAdd(SectionID).FindOrAdd(FoliageType);
						const FVector& InstanceOffset = FoliageInstanceArray[InstanceIndex].InstanceOffset;
						TObjectPtr< UFoliageInstancedStaticMeshComponent> TargetFoliageComponent{};
						if (!SectionData->GetFoliageMeshData(FoliageType, TargetFoliageComponent) && !EmpthyComponentArray.IsEmpty())
						{
							EmpthyComponentArray.Dequeue(TargetFoliageComponent);
							FoliageGenerater->EmptyComponentCount--;
						}
						if (!TargetFoliageComponent)
							continue;
						if (SectionData->ContainsFoliageMesh(FoliageType) == false)
							SectionData->SetFoliageMeshData(FoliageType, TargetFoliageComponent);
						UpdateData.SectionID = SectionID;
						UpdateData.MeshComponent = TargetFoliageComponent;
						if (UpdateData.bAdd == false)
						{
							UpdateData.bAdd = true;
							UpdateData.FoliageMesh = FoliageType;
							//UpdateData.FoliageData.Empty();
						}

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

	for (auto& UpdateMap : UpdateBySectionID)
	{
		for (auto& UpdateData : UpdateMap.Value)
		{
			FoliageUpdateData& NewUpdateData = UpdateData.Value;
			FoliageUpdateData& Data = FoliageGenerater->UpdateBackData.FindOrAdd(UpdateMap.Key).FindOrAdd(UpdateData.Value.MeshComponent);
			Data.Rset();
			Data = NewUpdateData;

		}
	}
	FoliageGenerater->UpdateData.Empty(false);
	for (auto& UpdateMap : UpdateBySectionID)
	{
		for (auto& UpdateData : UpdateMap.Value)
		{
			FoliageGenerater->UpdateData.Add(UpdateData.Value);
		}
	}

	if (FoliageGenerater)
	{
		FoliageGenerater->bGeneratingFoliage = true;
		FoliageGenerater->bUpdateBackData = true;
	}
}