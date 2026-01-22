#include "Landscape/Component/GenerateFoliageComponent.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "FoliageInstancedStaticMeshComponent.h"

UGenerateFoliageComponent::UGenerateFoliageComponent() :UGenerateWorldComponent{}
{
	PrimaryComponentTick.bCanEverTick = true;
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
		FoliageTypes.Empty(false);
		FoliageDataTable->GetAllRows< FFoliageDataTable>(TEXT(""), FoliageTypes);
	}
}

void UGenerateFoliageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bGeneratingFoliage == false)
	{
		if (bUpdateBackData)
		{
			UpdateBackData.GenerateValueArray(UpdateData);
			bUpdateBackData = false;
			return;

		}
		int Max = UpdateComponentTickCount;
		while (!UpdateData.IsEmpty() && Max > 0)
		{
			Max--;
			FoliageUpdateData& Data = UpdateData.Last();
			if (Data.bTickGenerate)
			{
				if (Data.MeshComponent)
				{
					Data.MeshComponent->ClearInstances();
					if(Data.FoliageMesh)
						Data.MeshComponent->SetStaticMesh(Data.FoliageMesh->GetStaticMesh());
				}
				Data.bTickGenerate = false;
				return;
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
					Location.Z += HitResult.Location.Z;
					FFloatInterval HeightRange = FoliageTypes[Data.InstanceIndex]->HeightRange;
					if (Location.Z <= HeightRange.Min && Location.Z < HeightRange.Max)
					{
						NewTransform.SetLocation(Location);
						Data.MeshComponent->AddInstance(NewTransform, true);
					}
				}
			}
			if (!Data.FoliageData.IsValidIndex(nUpdateTickIndex))
			{
				nUpdateTickIndex = 0;
				Data.bTickGenerate = false;
				UpdateData.Pop();
			}
		}
		if (UpdateData.IsEmpty())
		{
			bGeneratingFoliage = false;
			nUpdateTickIndex = 0; 
		}
	}
}

void UGenerateFoliageComponent::StartGenerateWorld(bool bEditor)
{
	if (bGeneratingFoliage)
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
	UE_LOG(LogTemp, Warning, TEXT("Start Generate Foliage"));
}

void UGenerateFoliageComponent::NewGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (SectionData.Vertices->Num() <= 1 || !bGeneratingFoliage || EditorModeGenerate)
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
	if (!bGeneratingFoliage || EditorModeGenerate)
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
	if (bGeneratingFoliage && !EditorModeGenerate)
	{
		//bFoliageUpdated = true;
		UpdateBackData.Empty();
		UpdateBackData.Reserve(50000);
		UE_LOG(LogTemp, Warning, TEXT("Start Generate Foliage Async"));
		bGeneratingFoliage = false;
		bUpdateBackData = false;
		//UpdateBuffer = false;
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [&]()
			{
				auto WorldGenTask = new FAsyncTask< FAsyncFoliageGenerater>(this);
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
	//TMap<TObjectPtr<UFoliageInstancedStaticMeshComponent>, TSet<int32>>& RemoveInstanceIndex = FoliageGenerater->RemoveInstanceIndex;
	//TMap< TObjectPtr<UFoliageType_InstancedStaticMesh>, TObjectPtr< UFoliageInstancedStaticMeshComponent>> &
	//FoliageTypeToInstanceIndex = FoliageGenerater->FoliageTypeToInstanceIndex;
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

		while (FoliageGenerater && !FoliageGenerater->AddArray.IsEmpty() && !EmpthyComponentArray.IsEmpty())
		{
			TObjectPtr< UFoliageInstancedStaticMeshComponent>& EmpthyFoliiageComponent = EmpthyComponentArray.Last();
			if (!EmpthyFoliiageComponent)
			{
				EmpthyComponentArray.Pop();
				continue;
			}

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
					const FFoliageDataTable* FoliageDataTableStruct = FoliageTypes[CurrentInstanceIndex];
					const TArray<FFoliageInstanceData>& FoliageInstanceArray = FoliageDataTableStruct->InstanceData;
					for (int InstanceIndex = 0; InstanceIndex < FoliageInstanceArray.Num(); InstanceIndex++)
					{
						// Data
						const TObjectPtr<UFoliageType_InstancedStaticMesh>& FoliageType = FoliageInstanceArray[InstanceIndex].FoliageType;
						const FVector& InstanceOffset = FoliageInstanceArray[InstanceIndex].InstanceOffset;

						// SetComponent
						TObjectPtr< UFoliageInstancedStaticMeshComponent>& TargetFoliageComponent =
							SectionData->MeshComponent.FindOrAdd(FoliageType, nullptr);
						if (!TargetFoliageComponent)
						{
							TargetFoliageComponent = EmpthyFoliiageComponent;
							EmpthyComponentArray.Pop();
						}
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

						// Generate Transform
						float RandX = FMath::RandRange(StartPos.X, EndPos.X);
						float RandY = FMath::RandRange(StartPos.Y, EndPos.Y);
						FVector RandPos{ RandX, RandY,  0 };
						FTransform NewTransform{};
						NewTransform.SetLocation(RandPos);
						FRotator Rot{};
						Rot.Yaw = FMath::RandRange(-360, 360);
						NewTransform.SetRotation(Rot.Quaternion());

						// New Data
						FVector Location = NewTransform.GetLocation();
						Location += InstanceOffset;
						Location.Z += FMath::RandRange(FoliageType->ZOffset.Min, FoliageType->ZOffset.Max);
						NewTransform.SetLocation(Location);
						NewTransform.SetScale3D(FVector(FoliageType->GetRandomScale()));
						UpdateData.FoliageData.Add(NewTransform);
					}
				}
			}
		}
	}

	if (FoliageGenerater)
	{
		FoliageGenerater->bGeneratingFoliage = false;
		FoliageGenerater->bUpdateBackData = true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Finish Generate Foliage"));
}