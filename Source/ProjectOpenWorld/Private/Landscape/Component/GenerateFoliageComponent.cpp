#include "Landscape/Component/GenerateFoliageComponent.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "FoliageInstancedStaticMeshComponent.h"
#include "Landscape/Component/PalFoliageInstanceComponent.h"

UGenerateFoliageComponent::UGenerateFoliageComponent() :UGenerateWorldComponent{}
{
	PrimaryComponentTick.bCanEverTick = true;
	FoliageMeshData.Empty(true);
	FoliageMeshData.Reserve(FoliageComponentCount);
	for (int i = 0; i < FoliageComponentCount; i++)
	{
		UFoliageInstancedStaticMeshComponent* FoliageCompoent =
			CreateDefaultSubobject<UPalFoliageInstanceComponent>(FName(*FString::Printf(TEXT("FoliageInstancedStaticMeshComponent_%d"), i)));
		if (FoliageCompoent)
		{
			FoliageCompoent->ComponentTags.Add(FName("Landscape"));
			FoliageCompoent->SetRenderCustomDepth(false);
			FoliageCompoent->SetCustomDepthStencilValue(0);
			FoliageMeshData.Add(FoliageCompoent);

		}
	}
	//Script/Engine.DataTable'/Game/Landscape/DT_FoliageData.DT_FoliageData'
	ConstructorHelpers::FObjectFinder<UDataTable> FoliageDataObj(TEXT("/Game/Landscape/DT_FoliageData.DT_FoliageData"));
	if (FoliageDataObj.Succeeded())
	{
		FoliageDataTable= FoliageDataObj.Object;
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
	bGeneratingFoliage = false;
	bUpdateBackData = false;
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
	FoliageCount = FoliageMeshData.Num();
}

void UGenerateFoliageComponent::SetFoliageMeshComponent(const TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageMesh, TObjectPtr<UFoliageInstancedStaticMeshComponent> MeshComp) const
{
	if (FoliageMesh && MeshComp)
	{
		MeshComp->SetStaticMesh(FoliageMesh->GetStaticMesh());
		MeshComp->SetCastShadow(FoliageMesh->CastShadow);
		MeshComp->bReceivesDecals = FoliageMesh->bReceivesDecals;
		MeshComp->SetCullDistances(FoliageMesh->CullDistance.Min, FoliageMesh->CullDistance.Max);
		MeshComp->SetCollisionProfileName(FoliageMesh->BodyInstance.GetCollisionProfileName());
		MeshComp->GetBodyInstance()->SetResponseToChannels(FoliageMesh->BodyInstance.GetCollisionResponse().GetResponseContainer());
		MeshComp->SetVisibleInRayTracing(FoliageMesh->bVisibleInRayTracing);
		MeshComp->SetRenderCustomDepth(FoliageMesh->bRenderCustomDepth);
		MeshComp->SetCustomDepthStencilWriteMask(FoliageMesh->CustomDepthStencilWriteMask);
		MeshComp->SetCustomDepthStencilValue(FoliageMesh->CustomDepthStencilValue);
	}	
}

void UGenerateFoliageComponent::GenerateFoliageAsync()
{
	if (!bGeneratingFoliage && !bUpdateBackData )
	{
		//UE_LOG(LogTemp, Warning, TEXT("Finish Generate Foliage Async %f "), Time);
		bDelayUpdate = false;
		PlayerLastSectionID = GeneratorSectionComponent->GetPlayerSection();
		bGeneratingFoliage = false;
		bUpdateBackData = false;
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

void UGenerateFoliageComponent::UpdateGenerateFoliage()
{
	if (!bGeneratingFoliage)
	{
		if (bUpdateBackData)
		{
			int Max = UpdateComponentTickCount;
			while (!UpdateData.IsEmpty() && Max > 0)
			{
				Max--;
				FoliageUpdateData& Data = UpdateData.Last();
				FoliageSectionData* SectionData = &FoliageSectionDataMap.FindOrAdd(Data.SectionID);
				if (SectionData)
				{
					// Section 제거 처리
					if (Data.bRemove)
					{
						//UE_LOG(LogTemp, Warning, TEXT("Update Foliage %d"), Data.bRemove);
						if (SectionData)
						{
							for (auto& Instance : SectionData->FoliageInstanceMap)
							{
								Instance.Value->ClearInstances();
								Instance.Value->SetStaticMesh(nullptr);
								EmpthyFoliageMeshData.Push(Instance.Value);
							}
							SectionData->FoliageInstanceMap.Empty(false);
						}
						FoliageSectionDataMap.Remove(Data.SectionID);
						nUpdateTickIndex = Data.FoliageData.Num() + 1;
					}
					else 
					{
						if (EmpthyFoliageMeshData.IsEmpty())
						{
							UpdateData.Empty();
							break;
						}
						// 생성 일시 설치 가능한 인스턴스 추가
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
							float SlopeAngle = FMath::RadiansToDegrees(
								FVector::DotProduct(-FVector::UpVector, HitResult.ImpactNormal));
							if (Data.FoliageMesh->AlignToNormal)
							{
								NewTransform.SetRotation(FQuat(FVector::ForwardVector.Rotation()));
							}
							NewTransform.SetLocation(Location);
							// 높이 및 경사도 체크
							if (Data.FoliageMesh->Height.Contains(Location.Z))
							{
								TObjectPtr<UFoliageInstancedStaticMeshComponent>& MeshComponent =
									SectionData->FoliageInstanceMap.FindOrAdd(Data.FoliageMesh, nullptr);
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

				// 섹션 내 폴리지 데이터 업데이트 완료
				if (!Data.FoliageData.IsValidIndex(nUpdateTickIndex))
				{
					nUpdateTickIndex = 0;
					UpdateData.Pop();
				}
			}

			// 섹션 별로 전체 섹션 업데이트 완료
			if (UpdateData.IsEmpty())
			{
				//UE_LOG(LogTemp, Warning, TEXT("Finish Update Foliage %f"), Time);
				bUpdateBackData = false;
			}
		}
	}
}

void UGenerateFoliageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Time += DeltaTime;

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
			UpdateGenerateFoliage();
		}
		else if (bDelayUpdate)
		{
			GenerateFoliageAsync();
		}
	}
}

void UGenerateFoliageComponent::StartGenerateWorld(bool bEditor)
{
	Super::StartGenerateWorld(bEditor);
	EditorModeGenerate = bEditor;
	Time = 0.0f;
	//UE_LOG(LogTemp, Warning, TEXT("Start Generate Foliage %f "), Time);
}

void UGenerateFoliageComponent::NewGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (SectionData.Vertices->Num() <= 1)
		return;

	FoliageSectionData* FoliageSectionData = FoliageSectionDataMap.Find(SectionData.SectionID);
	if (!FoliageSectionData)
	{
		// 새로 추가된 색션의 Data 생성
		FVector2D SectionID = SectionData.SectionID;
		int SectionIndex = static_cast<int>(FMath::PerlinNoise2D(SectionID * 0.1)* 100000);
		float RandomSeed = FoliageSeed * SectionIndex;
		if (bRandomSeed)
		{
			FMath::RandInit(FDateTime::Now().GetMillisecond());
			RandomSeed = FMath::Rand();
		}
		FoliageSectionData = &FoliageSectionDataMap.Add(SectionData.SectionID);
		FoliageSectionData->RandomSeed = RandomSeed;
		FoliageSectionData->SectionIndex = SectionIndex;
	}

	FoliageAddData& AddData = AddMap.FindOrAdd(SectionData.SectionID);
	AddData.StartPos = *SectionData.Vertices->begin();
	AddData.EndPos = SectionData.Vertices->Last();
}

void UGenerateFoliageComponent::DelGenerateWorld(const FGenerateSectionData& SectionData)
{
	FoliageAddData& AddData = AddMap.FindOrAdd(SectionData.SectionID);
	AddData.bRemove = true;
}

void UGenerateFoliageComponent::FinishGenerateWorld()
{
	Super::FinishGenerateWorld();
	bDelayUpdate = true;
	UE_LOG(LogTemp, Warning, TEXT("Finish Generate Foliage %d"), AddMap.Num());
	SetComponentTickEnabled(true);
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
	AddMap = FoliageGenerater->AddMap;
	FoliageGenerater->AddMap.Empty(false);
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
						if (!UpdateData.bRemove)
						{
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
	FoliageGenerater->UpdateData.StableSort(
		[](const FoliageUpdateData& A, const FoliageUpdateData& B)
		{
			if (A.bRemove)
				return false;
			return B.bRemove;
		}
	);

	if (FoliageGenerater)
	{
		FoliageGenerater->bGeneratingFoliage = true;
		FoliageGenerater->bUpdateBackData = true;
	}
}