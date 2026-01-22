#include "Landscape/Component/GenerateFoliageComponent.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "FoliageInstancedStaticMeshComponent.h"

UGenerateFoliageComponent::UGenerateFoliageComponent() :UGenerateWorldComponent{}
{
	PrimaryComponentTick.bCanEverTick = true;

	UpdateData.Reserve(50000);
	if (FoliageDataTable && RemoveInstanceIndex.IsEmpty())
	{
		FoliageDataTable->GetAllRows< FFoliageDataTable>(TEXT(""), FoliageTypes);
		for (int i = 0; i < FoliageTypes.Num(); i++)
		{
			TestFoliageTypes.Add(*FoliageTypes[i]);
			//for (const FFoliageInstanceData& Instance : Data->InstanceData)
			//{
			//	
			//}
		}
	}
}

void UGenerateFoliageComponent::BeginPlay()
{
	Super::BeginPlay();
	nSectionIndex = 0;
	SectionIDToFoliageTypeToInstanceIndex.Empty();
	if (FoliageDataTable && RemoveInstanceIndex.IsEmpty())
	{
		FoliageDataTable->GetAllRows< FFoliageDataTable>(TEXT(""), FoliageTypes);
		for (int i = 0; i < FoliageTypes.Num(); i++)
		{
			FFoliageDataTable* Data = FoliageTypes[i];
			for (const FFoliageInstanceData& Instance : Data->InstanceData)
			{
				TObjectPtr<UFoliageInstancedStaticMeshComponent>& FoliageInstance = FoliageTypeToInstanceIndex.FindOrAdd(Instance.FoliageType, nullptr);
				if (!FoliageInstance || !FoliageInstance.Get())
				{
					FoliageInstance = Cast<UFoliageInstancedStaticMeshComponent>(GetOwner()->AddComponentByClass(UFoliageInstancedStaticMeshComponent::StaticClass(), true, FTransform{}, false));
					FoliageInstance->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
					FoliageInstance->SetStaticMesh(Instance.FoliageType->GetStaticMesh());
					//if (Instance.FoliageType->BodyInstance.GetCollisionProfileName() == no)
						FoliageInstance->SetCollisionProfileName(Instance.FoliageType->BodyInstance.GetCollisionProfileName());
					RemoveInstanceIndex.Add(FoliageInstance, {});
				}
			}
		}
	}
}

void UGenerateFoliageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bFoliageUpdated)
	{
		if (bGeneratingFoliage)
		{
			 if (UpdateBuffer)
			 {
				 UpdateData.Append(UpdateBackData);
				// UpdateBackData.Empty();
				 UpdateBuffer = false;
				 return;
			 }
			int Max = UpdateTickCount;
			while (bGeneratingFoliage && !UpdateData.IsEmpty() && Max > 0)
			{
				FoliageUpdateData Data = UpdateData.Last();
				UpdateData.Pop(false);
				if (Data.FoliageComponent)
				{
					FVector Location = Data.NewTransform.GetLocation();
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
					}
					else
						continue;

					TSet<int32>* NewIndexSet = SectionIDToFoliageTypeToInstanceIndex.Find(Data.SectionID)->NewInstanceIndex.Find(Data.FoliageComponent);
					FFloatInterval HeightRange = FoliageTypes[Data.InstanceIndex]->HeightRange;
					TSet<int32>* RemoveIndexSet = RemoveInstanceIndex.Find(Data.FoliageComponent);
					if (Location.Z < HeightRange.Min || Location.Z > HeightRange.Max)
						continue;
					Data.NewTransform.SetLocation(Location);
					int32 InstanceID = -1;
					if (!RemoveIndexSet)
						continue;
					if (RemoveIndexSet->IsEmpty())
					{
						nInstanceCount++;
						InstanceID = Data.FoliageComponent->AddInstance(Data.NewTransform, true);
					}
					else
					{
						InstanceID = *RemoveIndexSet->begin();
						RemoveIndexSet->Remove(InstanceID);
						Data.FoliageComponent->UpdateInstanceTransform(InstanceID, Data.NewTransform, true, true, true);
					}
					if (NewIndexSet)
						NewIndexSet->FindOrAdd(InstanceID);
				}
				Max--;
			}
			if (bGeneratingFoliage && UpdateData.IsEmpty())
			{
				UE_LOG(LogTemp, Warning, TEXT("Finish Update Foliage"));
				bFoliageUpdated = false;
			}
		}
	}
}

void UGenerateFoliageComponent::StartGenerateWorld(bool bEditor)
{
	if (!bGeneratingFoliage)
		return;
	EditorModeGenerate = bEditor;
	if (EditorModeGenerate)
	{
		SectionIDToFoliageTypeToInstanceIndex.Empty();
	}
	bFoliageUpdated = false;
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
		TArray<TObjectPtr<UFoliageInstancedStaticMeshComponent>> Components{};
		FoliageTypeToInstanceIndex.GenerateValueArray(Components);
		for (TObjectPtr<UFoliageInstancedStaticMeshComponent>& Component : Components)
		{
			SectionIndex->NewInstanceIndex.Add(Component);
		}
		SectionIndex->bSectionNew = false;
	}
	if (SectionIndex->bSectionNew)
		return;
	AddArray.Add({ SectionData.SectionID, *SectionData.Vertices->begin(), SectionData.Vertices->Last() });
	SectionIndex->bSectionNew = true;
	SectionIndex->bAllReadyNew = false;
}

void UGenerateFoliageComponent::DelGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (!bGeneratingFoliage || EditorModeGenerate)
		return;
	FSectionData* SectionIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionData.SectionID);
	if (SectionIndex && SectionIndex->bSectionNew)
	{
		SectionIndex->bSectionNew = false;
		DeleteArray.Add(SectionData.SectionID);
	}
}

void UGenerateFoliageComponent::FinishGenerateWorld()
{
	if (bGeneratingFoliage && !EditorModeGenerate)
	{
		bFoliageUpdated = true;
		UpdateBackData.Empty();
		UpdateBackData.Reserve(50000);
		UE_LOG(LogTemp, Warning, TEXT("Start Generate Foliage Async"));
		bGeneratingFoliage = false;
		UpdateBuffer = false;
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
	TMap<TObjectPtr<UFoliageInstancedStaticMeshComponent>, TSet<int32>>& RemoveInstanceIndex = FoliageGenerater->RemoveInstanceIndex;
	TMap< TObjectPtr<UFoliageType_InstancedStaticMesh>, TObjectPtr< UFoliageInstancedStaticMeshComponent>> &
	FoliageTypeToInstanceIndex = FoliageGenerater->FoliageTypeToInstanceIndex;


	while (!DeleteArray.IsEmpty())
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
		DeleteArray.Pop(false);
	}

	while (FoliageGenerater && !FoliageGenerater->AddArray.IsEmpty())
	{
		FoliageAddData& Data = AddArray.Last();
		AddArray.Pop(false);
		FIntPoint SectionID = Data.SectionID;
		FSectionData* SectionIndex = SectionIndex = SectionIDToFoliageTypeToInstanceIndex.Find(SectionID);
		if (SectionIndex)
		{
			if (!FoliageGenerater)
				return;
			FMath::RandInit(SectionIndex->RandomSeed);
			int Rand = FMath::RandRange(FoliageGenerater->SectionRange.X, FoliageGenerater->SectionRange.Y);
			FVector StartPos = Data.StartPos;
			FVector EndPos = Data.EndPos;
			for (int CurrentRnadIndex =0; CurrentRnadIndex < Rand; CurrentRnadIndex++)
			{
				int32 CurrentInstanceIndex = FMath::Rand() % FoliageTypes.Num();
				const FFoliageDataTable* FoliageDataTableStruct = FoliageTypes[CurrentInstanceIndex];
				const TArray<FFoliageInstanceData>& InstanceData = FoliageDataTableStruct->InstanceData;
				for (int InstanceIndex = 0; InstanceIndex < InstanceData.Num(); InstanceIndex++)
				{
					float RandX = FMath::RandRange(StartPos.X, EndPos.X);
					float RandY = FMath::RandRange(StartPos.Y, EndPos.Y);
					FVector RandPos{ RandX, RandY,  0 };
					FTransform NewTransform{};
					NewTransform.SetLocation(RandPos);
					FRotator Rot{};
					Rot.Yaw = FMath::RandRange(-360, 360);
					NewTransform.SetRotation(Rot.Quaternion());
					const FFoliageInstanceData& Instance = InstanceData[InstanceIndex];
					TObjectPtr<UFoliageInstancedStaticMeshComponent>* FoliageInstance = FoliageTypeToInstanceIndex.Find(Instance.FoliageType);
					TSet<int32>* RemoveIndexSet = RemoveInstanceIndex.Find(*FoliageInstance);
					FVector Location = NewTransform.GetLocation();
					Location += Instance.InstanceOffset;
					Location.Z += FMath::RandRange(Instance.FoliageType->ZOffset.Min, Instance.FoliageType->ZOffset.Max);
					NewTransform.SetLocation(Location);
					NewTransform.SetScale3D(FVector(Instance.FoliageType->GetRandomScale()));
					if (FoliageGenerater)
						FoliageGenerater->UpdateBackData.Add({ *FoliageInstance, NewTransform ,SectionID, CurrentInstanceIndex , InstanceIndex });
				}
			}
		}
	}


	if (FoliageGenerater)
	{
		FoliageGenerater->bGeneratingFoliage = true;
		FoliageGenerater->UpdateBuffer = true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Finish Generate Foliage"));
}