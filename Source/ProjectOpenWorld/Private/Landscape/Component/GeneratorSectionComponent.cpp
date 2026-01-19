#include "Landscape/Component/GeneratorSectionComponent.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include <queue>
#include <set>

#if WITH_EDITOR
#include "Subsystems/UnrealEditorSubsystem.h"  //PreSave
#endif

UGeneratorSectionComponent::UGeneratorSectionComponent() : UActorComponent{}
{
	PrimaryComponentTick.bCanEverTick = true;
	scales =
	{ 0.000010, 0.000100, 0.001000, 0.010000 };
	amplitudes =
	{20000.000000, 800.000000, 100.000000, 50.000000};
	OnGenerateStart.Clear();
	OnNewSection.Clear();
	OnDeleteSection.Clear();
	OnGenerateFinished.Clear();
}

void UGeneratorSectionComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.HasArchetypeInstanceChanged(this))
	{
		//UE_LOG(LogTemp, Warning, TEXT("AWorldGenerator Property Changed"));
		FName PropertyName = PropertyChangedEvent.GetPropertyName();
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UGeneratorSectionComponent, bPropertyChaned))
		{
			if (bPropertyChaned)
			{
				FVector CameraLocation = GetOwner()->GetActorLocation();
				FRotator CameraRotation{};
				UUnrealEditorSubsystem* SubSystem = GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>();
				if (SubSystem)
				{
					SubSystem->GetLevelViewportCameraInfo(CameraLocation, CameraRotation);
				}
				FIntPoint PlayerSectionIndex = GetSectionIndex(CameraLocation);
				if (!GeneratorBusy && !TileDataReady)
				{
					PlayerSectionIndexX = PlayerSectionIndex.X;
					PlayerSectionIndexY = PlayerSectionIndex.Y;
					SectionMap.Empty();
					FAsyncWorldGenerater Generator(this);
					Generator.DoWork();
					StartGenerateTerrain(true);
					while (TileDataReady)
					{
						UpdateTerrain();
					}
				}
				bPropertyChaned = false;
			}
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UGeneratorSectionComponent::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter)
	{
		PlayerCharacter->GetRootComponent()->TransformUpdated.AddUObject(this, &UGeneratorSectionComponent::OnUpdatePlayerLocation);
		FIntPoint PlayerSectionIndex = GetSectionIndex(GetPlayerLocation());
		PlayerSectionIndexX = PlayerSectionIndex.X;
		PlayerSectionIndexY = PlayerSectionIndex.Y;
		GenerateTerrainAsync();
	}
}

void UGeneratorSectionComponent::OnUpdatePlayerLocation(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	if (!GeneratorBusy && !TileDataReady)
	{
		FIntPoint PlayerSectionIndex = GetSectionIndex(UpdatedComponent->GetComponentLocation());
		if (PlayerSectionIndex.X == PlayerSectionIndexX && PlayerSectionIndex.Y == PlayerSectionIndexY)
		{
			return;
		}
		PlayerSectionIndexX = PlayerSectionIndex.X;
		PlayerSectionIndexY = PlayerSectionIndex.Y;
		GenerateTerrainAsync();
	}
	else
	{
		bDelayUpdate = true;
	}
}
void UGeneratorSectionComponent::StartGenerateTerrain(bool bEditor)
{
	GeneratorBusy = false;
	MaxSection = UpdateSectionArray.Num();
	/*for (FIntPoint Section : UpdateSectionArray)
	{
		UE_LOG(LogTemp, Warning, TEXT("Section X:%d Y:%d"), Section.X, Section.Y);
	}*/
	CurrentIndex = MaxSection - 1;
	OnGenerateStart.Broadcast(bEditor);
}
void UGeneratorSectionComponent::UpdateTerrain()
{
	int nCount = SectionUpdateTickCount;
	while (!UpdateSectionArray.IsEmpty() && nCount > 0)
	{
		FIntPoint SectionID = UpdateSectionArray.Last();
		CurrentIndex = UpdateSectionArray.Num() - 1;
		if (nDeleteSectionCount <= CurrentIndex)
		{
			SectionMap.Remove(SectionID);
			OnDeleteSection.Broadcast(SectionID, *SumVertices.Find(SectionID),
				*SumUVs.Find(SectionID),
				*SumTriangles.Find(SectionID),
				*SumNormals.Find(SectionID),
				*SumTangents.Find(SectionID));

			SumVertices.Remove(SectionID);
			SumUVs.Remove(SectionID);
			SumTriangles.Remove(SectionID);
			SumNormals.Remove(SectionID);
			SumTangents.Remove(SectionID);
		}
		else if (CurrentIndex < nDeleteSectionCount)
		{
			//UE_LOG(LogTemp, Warning, TEXT("New Section X:%d Y:%d"), SectionID.X, SectionID.Y);
			SectionMap.Add(SectionID);
			OnNewSection.Broadcast(SectionID, *SumVertices.Find(SectionID),
				*SumUVs.Find(SectionID),
				*SumTriangles.Find(SectionID),
				*SumNormals.Find(SectionID),
				*SumTangents.Find(SectionID));
		}
		UpdateSectionArray.Pop();
		nCount--;
	}
	if (UpdateSectionArray.IsEmpty())
	{
		EndGenerateTerrain();
	}
}

void UGeneratorSectionComponent::EndGenerateTerrain()
{
	OnGenerateFinished.Broadcast();
	TileDataReady = false;
}

void UGeneratorSectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GeneratorBusy)
	{
		if (TileDataReady)
		{
			StartGenerateTerrain();
		}
		return;
	}
	else
	{
		if (TileDataReady)
		{
			UpdateTerrain();
		}
		else if (bDelayUpdate)
		{
			FIntPoint PlayerSectionIndex = GetSectionIndex(GetPlayerLocation());
			if (PlayerSectionIndex.X == PlayerSectionIndexX && PlayerSectionIndex.Y == PlayerSectionIndexY)
			{
				return;
			}
			PlayerSectionIndexX = PlayerSectionIndex.X;
			PlayerSectionIndexY = PlayerSectionIndex.Y;
			GenerateTerrainAsync();
		}
	}
}

void UGeneratorSectionComponent::BindGenerteComponent(UGenerateWorldComponent* InGenerateWorldComponent)
{
	if (!InGenerateWorldComponent)
		return;
	OnGenerateStart.AddUniqueDynamic(InGenerateWorldComponent, &UGenerateWorldComponent::StartGenerateWorld);
	OnNewSection.AddUniqueDynamic(InGenerateWorldComponent, &UGenerateWorldComponent::NewGenerateWorld);
	OnDeleteSection.AddUniqueDynamic(InGenerateWorldComponent, &UGenerateWorldComponent::DelGenerateWorld);
	OnGenerateFinished.AddUniqueDynamic(InGenerateWorldComponent, &UGenerateWorldComponent::FinishGenerateWorld);
}

void UGeneratorSectionComponent::GenerateTerrainAsync()
{
	GeneratorBusy = true;
	bDelayUpdate = false;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [&]()
		{
			auto WorldGenTask = new FAsyncTask< FAsyncWorldGenerater>(this);
			WorldGenTask->StartBackgroundTask();
			WorldGenTask->EnsureCompletion();
			delete WorldGenTask;
		}
	);
}

FVector UGeneratorSectionComponent::GetPlayerLocation() const
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter)
		return PlayerCharacter->GetActorLocation();
	return FVector();
}

FIntPoint UGeneratorSectionComponent::GetSectionIndex(FVector Location)
{
	return FIntPoint(FMath::FloorToInt(Location.X / (CellSize * (xVertexCount - 1))),
		FMath::FloorToInt(Location.Y / (CellSize * (yVertexCount - 1))));
}

void FAsyncWorldGenerater::DoWork()
{
	int Index{};

	TSet<FIntPoint> DelSectionSet = WorldGenerator->SectionMap;
	WorldGenerator->UpdateSectionArray.Empty();

	for (int i = -WorldGenerator->SectionRadiusCount; i < WorldGenerator->SectionRadiusCount; i++)
	{
		for (int k = -WorldGenerator->SectionRadiusCount; k < WorldGenerator->SectionRadiusCount; k++)
		{
			FIntPoint NewSectionIndex = FIntPoint(PlayerSectionIndex.X + i, PlayerSectionIndex.Y + k);
			if (!WorldGenerator->SectionMap.Find(NewSectionIndex))
			{
				GenerateTerrainTile(PlayerSectionIndex.X + i, PlayerSectionIndex.Y + k);
				WorldGenerator->UpdateSectionArray.Add(NewSectionIndex);
			}
			DelSectionSet.Remove(NewSectionIndex);
			Index++;
		}
	}
	FIntPoint Temp = PlayerSectionIndex;
	WorldGenerator->UpdateSectionArray.Sort
	(
		[Temp](const FIntPoint& A, const FIntPoint& B)
		{
			int32 AdX = abs(Temp.X - A.X);
			int32 AdY = abs(Temp.Y - A.Y);
			int32 DistA = AdX * AdX + AdY * AdY;
			int32 BdX = abs(B.X - Temp.X);
			int32 BdY = abs(B.Y - Temp.Y);
			int32 DistB = BdX * BdX + BdY * BdY;
			if (DistA != DistB)
				return DistA > DistB;

			if (AdX == BdX)
			{
				{
					return AdY > BdY;
				}
			}
			if (AdY == BdY)
				return AdX > BdX;
			return AdY > BdY;
		}
	);

	int nIndex = WorldGenerator->UpdateSectionArray.Num();
	for (const FIntPoint& point : DelSectionSet)
	{
		WorldGenerator->UpdateSectionArray.Add(point);

	}
	WorldGenerator->nDeleteSectionCount = WorldGenerator->UpdateSectionArray.Num() - DelSectionSet.Num();
//	UE_LOG(LogTemp, Warning, TEXT("Delete Section Count:%d %d %d "), WorldGenerator->UpdateSectionArray.Num(), WorldGenerator->nDeleteSectionCount, DelSectionSet.Num());

	

	//int Dx[4] = { 1,-1,0,0 };
	//int Dy[4] = { 0,0,1,-1 };
	//int HalfX = WorldGenerator->SectionRadiusCount / 2;
	//int HalfY = WorldGenerator->SectionRadiusCount / 2;
	//using namespace std;
	//queue<FAsyncWorldGenerater::TrraninNode> Q{};
	//Q.push({ PlayerSectionIndexX, PlayerSectionIndexY });
	//set<FAsyncWorldGenerater::TrraninNode> Visited{};
	//Visited.insert({ PlayerSectionIndexX, PlayerSectionIndexY });
	//int dissqr = WorldGenerator->SectionRadiusCount * WorldGenerator->SectionRadiusCount;
	//while (!Q.empty() && WorldGenerator)
	//{
	//	FAsyncWorldGenerater::TrraninNode Node = Q.front();
	//	Q.pop();
	//	GenerateTerrainTile(Node.SectionIndexX, Node.SectionIndexY);
	//	for (int i = 0; i < 4 && WorldGenerator; i++)
	//	{
	//		int NewX = Node.SectionIndexX + Dx[i];
	//		int NewY = Node.SectionIndexY + Dy[i];
	//		if ((NewX - PlayerSectionIndexX) * (NewX - PlayerSectionIndexX) +
	//			(NewY - PlayerSectionIndexY) * (NewY - PlayerSectionIndexY) > dissqr)
	//		{
	//			continue;
	//		}
	//		{
	//			FAsyncWorldGenerater::TrraninNode NewNode(NewX, NewY);
	//			if (Visited.find(NewNode) == Visited.end())
	//			{
	//				Visited.insert(NewNode);
	//				Q.push(NewNode);
	//			}
	//		}
	//	}
	//}


	if (WorldGenerator)
	{
		WorldGenerator->TileDataReady = true;
	}
}

void FAsyncWorldGenerater::InitGenerater()
{
	PlayerSectionIndex.X = WorldGenerator->PlayerSectionIndexX;
	PlayerSectionIndex.Y = WorldGenerator->PlayerSectionIndexY;
}

void FAsyncWorldGenerater::GenerateTerrainTile(const int inSectionIndexX, const int inSectionIndexY)
{
	int CellSize = WorldGenerator->CellSize;
	int xVertexCount = WorldGenerator->xVertexCount;
	int yVertexCount = WorldGenerator->yVertexCount;

	FVector Offset = FVector(inSectionIndexX * (xVertexCount - 1), inSectionIndexY * (yVertexCount - 1), 0.0f) * CellSize;
	TArray<FVector> Vertices{};
	TArray<FVector2D> UVs{};
	TArray<FVector> Normals{};
	TArray<FProcMeshTangent> Tangents{};
	FVector Vertex{};
	FVector2D UV{};

	// verices and uvs
	for (int32 iVY = -1; iVY <= yVertexCount; iVY++)
	{
		for (int32 iVX = -1; iVX <= xVertexCount; iVX++)
		{
			//vertex calculate
			Vertex.X = iVX * CellSize + Offset.X;
			Vertex.Y = iVY * CellSize + Offset.Y;
			Vertex.Z = GetHeight(Vertex.X, Vertex.Y);
			Vertices.Add(Vertex);
			//DrawDebugBox(GetWorld(), Vertex, FVector(10, 10, 10), FColor::Red, true, 1);

			UV.X = (iVX + (inSectionIndexX * (xVertexCount - 1))) * CellSize / 100.f;
			UV.Y = (iVY + (inSectionIndexY * (yVertexCount - 1))) * CellSize / 100.f;
			UVs.Add(UV);
		}
	}

	if (Triangles.Num() == 0)
	{
		for (int32 iTY = 0; iTY < yVertexCount; iTY++)
		{
			for (int32 iTX = 0; iTX < xVertexCount; iTX++)
			{
				Triangles.Add(iTX + iTY * (xVertexCount + 2));
				Triangles.Add(iTX + (iTY + 1) * (xVertexCount + 2));
				Triangles.Add(iTX + iTY * (xVertexCount + 2) + 1);
				Triangles.Add(iTX + (iTY + 1) * (xVertexCount + 2));
				Triangles.Add(iTX + (iTY + 1) * (xVertexCount + 2) + 1);
				Triangles.Add(iTX + iTY * (xVertexCount + 2) + 1);
			}
		}
	}
	TArray<FVector> SubVertices{};
	TArray<FVector2D> SubUVs{};
	TArray<int32> SubTriangles{};
	TArray<FVector> SubNormals{};
	TArray<FProcMeshTangent> SubTangents{};
	int VertexIndex{};
	// calculate normals and tangents
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);

	// subset verices and uvs
	for (int32 iVY = -1; iVY <= yVertexCount; iVY++)
	{
		for (int32 iVX = -1; iVX <= xVertexCount; iVX++)
		{
			if (-1 < iVY && iVY < yVertexCount && -1 < iVX && iVX < xVertexCount)
			{
				uint8 ObjectID = FMath::Clamp(static_cast<uint8>(Vertices[VertexIndex].Z), 0, 5);
				SubVertices.Add(Vertices[VertexIndex]);
				SubUVs.Add(UVs[VertexIndex]);
				SubNormals.Add(Normals[VertexIndex]);
				SubTangents.Add(Tangents[VertexIndex]);
			}
			VertexIndex++;
		}
	}

	if (SubTriangles.Num() == 0)
	{
		for (int32 iTY = 0; iTY <= yVertexCount - 2; iTY++)
		{
			for (int32 iTX = 0; iTX <= xVertexCount - 2; iTX++)
			{
				SubTriangles.Add(iTX + iTY * xVertexCount);
				SubTriangles.Add(iTX + iTY * xVertexCount + xVertexCount);
				SubTriangles.Add(iTX + iTY * xVertexCount + 1);

				SubTriangles.Add(iTX + iTY * xVertexCount + xVertexCount);
				SubTriangles.Add(iTX + iTY * xVertexCount + xVertexCount + 1);
				SubTriangles.Add(iTX + iTY * xVertexCount + 1);
			}
		}
	}
	if (WorldGenerator)
	{
		FIntPoint SectionIndexKey = FIntPoint(inSectionIndexX, inSectionIndexY);
		WorldGenerator->SumVertices.Add(SectionIndexKey, SubVertices);
		WorldGenerator->SumUVs.Add(SectionIndexKey, SubUVs);
		WorldGenerator->SumTriangles.Add(SectionIndexKey, SubTriangles);
		WorldGenerator->SumNormals.Add(SectionIndexKey, SubNormals);
		WorldGenerator->SumTangents.Add(SectionIndexKey, SubTangents);
	}
}
float FAsyncWorldGenerater::GetHeight(float inX, float inY) const
{
	float HeightScale{};
	int nSize = WorldGenerator->amplitudes.Num();
	for (int i = 0; i < nSize; i++)
	{
		HeightScale += PerlinNoiseExtended(inX, inY, WorldGenerator->scales[i], WorldGenerator->amplitudes[i], FVector2D(0.1) * i);
	}
	return HeightScale;
}

float FAsyncWorldGenerater::PerlinNoiseExtended(float inX, float inY, float scale, float Amplitude, FVector2D offset) 
{
	//+ FVector2D(0.1f, 0.1f) + offset
	return FMath::PerlinNoise2D(FVector2D(inX, inY) * scale) * Amplitude;
}


