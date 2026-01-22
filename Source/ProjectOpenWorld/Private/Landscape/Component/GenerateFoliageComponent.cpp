#include "Landscape/Component/GenerateFoliageComponent.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "FoliageInstancedStaticMeshComponent.h"

UGenerateFoliageComponent::UGenerateFoliageComponent() :UGenerateWorldComponent{}
{
	PrimaryComponentTick.bCanEverTick = true;
	FoliageComponentArray.Empty(false);
	FoliageComponentArray.Reserve(FoliageComponentCount);
	EmpthyComponentArray.Reserve(FoliageComponentCount);
	for (int i = 0; i < FoliageComponentCount; i++)
	{
		UFoliageInstancedStaticMeshComponent* FoliageCompoent = CreateDefaultSubobject<UFoliageInstancedStaticMeshComponent>(FName(*FString::Printf(TEXT("FoliageInstancedStaticMeshComponent_%d"), i)));
		if (FoliageCompoent)
		{
			FoliageComponentArray.Add(FoliageCompoent);
			EmpthyComponentArray.Add(FoliageCompoent);
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
		AddArray.Empty(false);
		UpdateData.Empty(false);
		FoliageTypes.Empty(false);
		FoliageDataTable->GetAllRows< FFoliageDataTable>(TEXT(""), FoliageTypes);
	}
}

void UGenerateFoliageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bGeneratingFoliage)
	{
		if (bUpdateBackData)
		{
			TArray< FoliageUpdateData> NewUpdateData{};
			UpdateBackData.GenerateValueArray(NewUpdateData);
			UpdateData.Append(NewUpdateData);
			bGeneratingFoliage = false;
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
				if (Data.bTickGenerate)
				{
					nUpdateTickIndex = 0;
					if (Data.MeshComponent)
					{
						Data.MeshComponent->ClearInstances();
						if (Data.FoliageMesh)
						{
							Data.MeshComponent->SetStaticMesh(Data.FoliageMesh->GetStaticMesh());
							Data.MeshComponent->SetCastShadow(Data.FoliageMesh->CastShadow);
							Data.MeshComponent->bReceivesDecals = Data.FoliageMesh->bReceivesDecals;
							Data.MeshComponent->SetCullDistances(Data.FoliageMesh->CullDistance.Min, Data.FoliageMesh->CullDistance.Max);
							Data.MeshComponent->SetCollisionProfileName(Data.FoliageMesh->BodyInstance.GetCollisionProfileName());
							Data.MeshComponent->SetVisibleInRayTracing(Data.FoliageMesh->bVisibleInRayTracing);
						}
						//UE_LOG(LogTemp, Warning, TEXT("Data : %s"), *Data.FoliageMesh.GetName());
					}
					Data.bTickGenerate = false;
					//return;
				}
				else if (Data.MeshComponent && Data.FoliageData.IsValidIndex(nUpdateTickIndex))
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
						UE_LOG(LogTemp, Warning, TEXT("SlopeAngle : %f %f, %f"), Location.Z, Data.FoliageMesh->ZOffset.Min, Data.FoliageMesh->ZOffset.Max);
						if (Data.FoliageMesh->Height.Contains(Location.Z))
						{
							NewTransform.SetLocation(Location);
							Data.MeshComponent->AddInstance(NewTransform, true);
						}
					}
					nUpdateTickIndex++;
				}

				if (!Data.FoliageData.IsValidIndex(nUpdateTickIndex))
				{
					//UE_LOG(LogTemp, Warning, TEXT("Finish Generate Foliage SectionID X:%d Y:%d"), Data.SectionID.X, Data.SectionID.Y);
					Data.bTickGenerate = false;
					UpdateData.Pop();
				}
			}
			if (UpdateData.IsEmpty())
			{
				bUpdateBackData = false;
			}
		}
	}
}

void UGenerateFoliageComponent::StartGenerateWorld(bool bEditor)
{
	if (bGeneratingFoliage || bUpdateBackData)
		return;
	bGeneratingFoliage = true;
	EditorModeGenerate = bEditor;
	if (EditorModeGenerate)
	{
		EmpthyComponentArray.Empty(false);
		nSectionIndex = 0;
		for (int i = 0; i < FoliageComponentCount; i++)
		{
			UFoliageInstancedStaticMeshComponent* FoliageCompoent = FoliageComponentArray[i];
			if (FoliageCompoent)
			{
				FoliageCompoent->ClearInstances();
				EmpthyComponentArray.Add(FoliageCompoent);
			}
		}
		SectionIDToFoliageTypeToInstanceIndex.Empty();
	}
	//UE_LOG(LogTemp, Warning, TEXT("Start Generate Foliage"));
}

void UGenerateFoliageComponent::NewGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (SectionData.Vertices->Num() <= 1 || !bGeneratingFoliage || EditorModeGenerate || bUpdateBackData)
		return;

	FSectionData* SectionIndex = SectionIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionData.SectionID);
	if (!SectionIndex)
	{
		SectionIndex = &SectionIDToFoliageTypeToInstanceIndex.FindOrAdd(SectionData.SectionID, {});
		SectionIndex->SectionIndex = nSectionIndex;
		nSectionIndex++;
		SectionIndex->RandomSeed = FoliageSeed * nSectionIndex;
		if (bRandomSeed)
		{
			FMath::RandInit(FDateTime::Now().GetMillisecond());
			SectionIndex->RandomSeed = FMath::Rand();
		}
	}
	if (!SectionIndex->bAlreadyGenerate)
	{
		SectionIndex->bAlreadyGenerate = true;
		AddArray.Add({SectionData.SectionID, *SectionData.Vertices->begin(), SectionData.Vertices->Last() });
	}
}

void UGenerateFoliageComponent::DelGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (!bGeneratingFoliage || EditorModeGenerate || bUpdateBackData)
		return;

	FSectionData* SectionIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionData.SectionID);
	if (SectionIndex && SectionIndex->bAlreadyGenerate)
	{
		SectionIndex->bAlreadyGenerate = false;
		DeleteArray.Add(SectionData.SectionID);
	}
}

void UGenerateFoliageComponent::FinishGenerateWorld()
{
	if (bGeneratingFoliage && !bUpdateBackData && !EditorModeGenerate)
	{
		PlayerLastSectionID = GeneratorSectionComponent->GetPlayerSection();
		bGeneratingFoliage = false;
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

void FAsyncFoliageGenerater::DoWork()
{
	using FoliageAddData = UGenerateFoliageComponent::FoliageAddData;
	using FSectionData = UGenerateFoliageComponent::FSectionData;
	using FoliageUpdateData = UGenerateFoliageComponent::FoliageUpdateData;

	TArray<FIntPoint>& DeleteArray = FoliageGenerater->DeleteArray;
	TArray<FoliageAddData>&	AddArray = FoliageGenerater->AddArray;
	TMap<FIntPoint, FSectionData>& SectionIDToFoliageTypeToInstanceIndex = FoliageGenerater->SectionIDToFoliageTypeToInstanceIndex;
	TArray<FFoliageDataTable*>& FoliageTypes = FoliageGenerater->FoliageTypes;
	FoliageGenerater->UpdateBackData.Empty(50000);
	int Count{};
	TArray<TObjectPtr<UFoliageInstancedStaticMeshComponent>> EmpthyComponentArray = FoliageGenerater->EmpthyComponentArray;
	if (FoliageTypes.Num() > 0)
	{
		while (!DeleteArray.IsEmpty())
		{
			FIntPoint SectionID = DeleteArray.Last();
			if (FSectionData* pFoliageTypeToInstanceIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionID))
			{
				TArray<TObjectPtr<UFoliageInstancedStaticMeshComponent>> Temp{};
				pFoliageTypeToInstanceIndex->MeshComponent.GenerateValueArray(Temp);
				EmpthyComponentArray.Append(Temp);
				pFoliageTypeToInstanceIndex->MeshComponent.Empty();
			}
			DeleteArray.Pop(false);
		}
		
		bool bBreak{};
		while (FoliageGenerater && !FoliageGenerater->AddArray.IsEmpty() && !bBreak)
		{
			FoliageAddData& AddData = AddArray.Last();
			AddArray.Pop(false);
			FIntPoint SectionID = AddData.SectionID;
			FSectionData* SectionData = SectionIDToFoliageTypeToInstanceIndex.Find(SectionID);
			if (SectionData)
			{
				FMath::RandInit(SectionData->RandomSeed);
				int Rand = FMath::RandRange(FoliageGenerater->FoliageDataCreateRange.X, FoliageGenerater->FoliageDataCreateRange.Y);
				FVector StartPos = AddData.StartPos;
				FVector EndPos = AddData.EndPos;
				for (int CurrentRnadIndex = 0; CurrentRnadIndex < Rand; CurrentRnadIndex++)
				{
					int32 CurrentInstanceIndex = FMath::Rand() % FoliageTypes.Num();
					//UE_LOG(LogTemp, Warning, TEXT("Generate Foliage Instance Index : %d"), CurrentInstanceIndex);
					const FFoliageDataTable* FoliageDataTableStruct = FoliageTypes[CurrentInstanceIndex];
					const TArray<FFoliageInstanceData>& FoliageInstanceArray = FoliageDataTableStruct->InstanceData;
					float RandX = FMath::RandRange(StartPos.X, EndPos.X);
					float RandY = FMath::RandRange(StartPos.Y, EndPos.Y);
					FVector RandPos{ RandX, RandY,  0 };
					for (int InstanceIndex = 0; InstanceIndex < FoliageInstanceArray.Num(); InstanceIndex++)
					{
						// Data
						const TObjectPtr<UFoliageType_InstancedStaticMesh>& FoliageType = FoliageInstanceArray[InstanceIndex].FoliageType;
						const FVector& InstanceOffset = FoliageInstanceArray[InstanceIndex].InstanceOffset;

						// SetComponent
						TObjectPtr< UFoliageInstancedStaticMeshComponent>& TargetFoliageComponent =
							SectionData->MeshComponent.FindOrAdd(FoliageType, nullptr);
						if (TargetFoliageComponent.Get() == nullptr && !EmpthyComponentArray.IsEmpty())
						{
							TObjectPtr< UFoliageInstancedStaticMeshComponent>& EmpthyFoliiageComponent = EmpthyComponentArray.Last();
							EmpthyComponentArray.Pop();
							TargetFoliageComponent = EmpthyFoliiageComponent;
						}
						if (!TargetFoliageComponent)
							continue;
						FoliageUpdateData& UpdateData = FoliageGenerater->UpdateBackData.FindOrAdd(TargetFoliageComponent);
						UpdateData.SectionID = SectionID;
						UpdateData.MeshComponent = TargetFoliageComponent;
						UpdateData.InstanceIndex = CurrentInstanceIndex;
						if (UpdateData.bTickGenerate == false)
						{
							UpdateData.FoliageData.Empty();
							UpdateData.bTickGenerate = true;
							UpdateData.FoliageMesh = FoliageType;
						}
						//UE_LOG(LogTemp, Warning, TEXT("Start Generate Foliage Com %s FoliageType : %s"), *UpdateData.MeshComponent->GetName(), *FoliageType.GetName());

						// Generate Random Transform
						FTransform NewTransform{};
						NewTransform.SetLocation(RandPos);
						FRotator Rot{};
						if(FoliageType->RandomYaw)
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
						Count++;
					}
				}
			}
		}
	}
	if (FoliageGenerater)
	{
		FoliageGenerater->bGeneratingFoliage = true;
		FoliageGenerater->bUpdateBackData = true;
	}
}