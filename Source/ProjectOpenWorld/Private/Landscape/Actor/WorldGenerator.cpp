#include "Landscape/Actor/WorldGenerator.h"
#include "KismetProceduralMeshLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "UObject/ObjectSaveContext.h"
#if WITH_EDITOR
#include "Subsystems/UnrealEditorSubsystem.h"
#endif

//#include 

AWorldGenerator::AWorldGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	//PrimaryActorTick.TickInterval = 0.5f;

	TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	TerrainMesh->SetupAttachment(GetRootComponent());
	TileRelplaceableDistance = CellSize * (NumOfSectionsX + NumOfSectionsY) / 2 * (xVertexCount + yVertexCount) ;
	scales =
	{
		0.00001f,
		0.0001f,
		0.001f,
		0.01f
	};
	amplitudes = {
		20000.0f,
		10000.0f,
		500.0f,
		100.0f
	};
}

void AWorldGenerator::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);
#if WITH_EDITOR
	FVector CameraLocation = GetActorLocation();
	FRotator CameraRotation{};
	int inSectionIndexX = FMath::FloorToInt(CameraLocation.X / (CellSize * (xVertexCount - 1)));
	int inSectionIndexY = FMath::FloorToInt(CameraLocation.Y / (CellSize * (yVertexCount - 1)));

	UUnrealEditorSubsystem* SubSystem = GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>();
	if (SubSystem)
	{
		SubSystem->GetLevelViewportCameraInfo(CameraLocation, CameraRotation);
		int CameraSectionIndexX = FMath::FloorToInt(CameraLocation.X / (CellSize * (xVertexCount - 1)));
		int CameraSectionIndexY = FMath::FloorToInt(CameraLocation.Y / (CellSize * (yVertexCount - 1)));

		if(CameraSectionIndexX != inSectionIndexX || CameraSectionIndexY != inSectionIndexY)
		{
			inSectionIndexX = CameraSectionIndexX;
			inSectionIndexY = CameraSectionIndexY;
			bPropertyChaned = true;
		}

	}
	if (!bPropertyChaned)
		return;
	bPropertyChaned = false;
	TerrainMesh->ClearAllMeshSections();
	MeshSectionIndex = 0;
	for (int32 iX = -NumOfSectionsX / 2; iX <= NumOfSectionsX / 2; iX++)
	{
		for (int32 iY = -NumOfSectionsY / 2; iY <= NumOfSectionsY / 2; iY++)
		{
			int TargetSectionX = inSectionIndexX + iX;
			int TargetSectionY = inSectionIndexY + iY;
			//QueuedTiles.Add(FIntPoint(TargetSectionX, TargetSectionY), -1);
			GenerateTerrain(TargetSectionX, TargetSectionY);
			DrawTile();
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("AWorldGenerator PreSave Completed"));
#endif // 
}

#if WITH_EDITOR
void AWorldGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.HasArchetypeInstanceChanged(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorldGenerator Property Changed"));
		bPropertyChaned = true;
	}
}
#endif // 

void AWorldGenerator::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	GeneratorBusy = false;
	TileDataReady = false;
	TerrainMesh->ClearAllMeshSections();
	MeshSectionIndex = 0;
	if (false)
	{
		PlayerCharacter->GetRootComponent()->TransformUpdated.AddUObject(this, &AWorldGenerator::OnUpdatePlayerLocation);

		//PlayerSectionIndexX = FMath::FloorToInt(PlayerCharacter->GetActorLocation().X / (CellSize * (xVertexCount - 1)));
		//PlayerSectionIndexY = FMath::FloorToInt(PlayerCharacter->GetActorLocation().Y / (CellSize * (yVertexCount - 1)));
		//bPropertyChaned = false;
		//TerrainMesh->ClearAllMeshSections();
		//MeshSectionIndex = 0;
		//for (int32 iX = -NumOfSectionsX / 2; iX <= NumOfSectionsX / 2; iX++)
		//{
		//	for (int32 iY = -NumOfSectionsY / 2; iY <= NumOfSectionsY / 2; iY++)
		//	{
		//		int TargetSectionX = PlayerSectionIndexX + iX;
		//		int TargetSectionY = PlayerSectionIndexY + iY;
		//		//QueuedTiles.Add(FIntPoint(TargetSectionX, TargetSectionY), -1);
		//		GenerateTerrain(TargetSectionX, TargetSectionY);
		//		DrawTile();
		//	}
		//}
	}
}

void AWorldGenerator::OnUpdatePlayerLocation(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{

	//int NewPlayerSectionIndexX = FMath::FloorToInt(UpdatedComponent->GetComponentLocation().X / (CellSize * (xVertexCount - 1)));
	//int NewPlayerSectionIndexY = FMath::FloorToInt(UpdatedComponent->GetComponentLocation().Y / (CellSize * (yVertexCount - 1)));
	//if (NewPlayerSectionIndexX == PlayerSectionIndexX && NewPlayerSectionIndexY == PlayerSectionIndexY)
	//{
	//	return;
	//}
	//if (GeneratorBusy)
	//	return;

	//PlayerSectionIndexX = NewPlayerSectionIndexX;
	//PlayerSectionIndexY = NewPlayerSectionIndexY;
	//for (int32 iX = -NumOfSectionsX / 2; iX <= NumOfSectionsX / 2; iX++)
	//{
	//	for (int32 iY = -NumOfSectionsY / 2; iY <= NumOfSectionsY / 2; iY++)
	//	{
	//		int TargetSectionX = PlayerSectionIndexX + iX;
	//		int TargetSectionY = PlayerSectionIndexY + iY;
	//		if (!QueuedTiles.Contains(FIntPoint(TargetSectionX, TargetSectionY)))
	//		{
	//			QueuedTiles.Add(FIntPoint(TargetSectionX, TargetSectionY), -1);
	//		}
	//	}
	//}
}

void AWorldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*FIntPoint Location{};
	if (GeneratorBusy && !TileDataReady)
	{
		return;
	}
	else if (!GeneratorBusy && TileDataReady)
	{
		return;
	}
	else if (GeneratorBusy && TileDataReady)
	{
		DrawTile();
	}
	else if (!GeneratorBusy && !TileDataReady && GetClosestQueuedTile(Location))
	{
		GenerateTerrainAsync(Location.X, Location.Y);
	}*/
}

void AWorldGenerator::GenerateTerrain(const int inSectionIndexX, const int inSectionIndexY)
{
	FVector Offset = FVector(inSectionIndexX * (xVertexCount - 1) , inSectionIndexY * (yVertexCount - 1) , 0.0f) * CellSize;
//	UE_LOG(LogTemp, Warning, TEXT("Generating Tile X:%d Y:%d Offset %s"), inSectionIndexX, inSectionIndexY, *Offset.ToString());
	TArray<FVector> Vertices{};
	TArray<FVector2D> UVs{};
	TArray<FVector> Normals{};
	TArray<FProcMeshTangent> Tangents{};

	FVector Vertex{}; 
	FVector2D UV{};
	
	// verices and uvs

	for(int32 iVY = -1 ; iVY <= yVertexCount; iVY++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Generating Vertex Y:%f"), iVY * CellSize + Offset.Y);
		for(int32 iVX = -1; iVX <= xVertexCount; iVX++)
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

	int VertexIndex{};

	// calculate normals and tangents
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);

	// subset verices and uvs
	for (int32 iVY = -1; iVY <= yVertexCount; iVY++)
	{
		for (int32 iVX = -1; iVX <= xVertexCount; iVX++)
		{
			if(-1 < iVY && iVY < yVertexCount && -1 < iVX && iVX < xVertexCount)
			{
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
				SubTriangles.Add(iTX + iTY * xVertexCount+ xVertexCount);
				SubTriangles.Add(iTX + iTY * xVertexCount + 1);

				SubTriangles.Add(iTX + iTY * xVertexCount + xVertexCount);
				SubTriangles.Add(iTX + iTY * xVertexCount + xVertexCount + 1);
				SubTriangles.Add(iTX + iTY * xVertexCount + 1);
			}
		}
	}
	TileDataReady = true;
}

void AWorldGenerator::GenerateTerrainAsync(const int inSectionIndexX, const int inSectionIndexY)
{
	GeneratorBusy = true;
	SectionIndexX = inSectionIndexX;;
	SectionIndexY = inSectionIndexY;
	QueuedTiles.Add(FIntPoint(inSectionIndexX, inSectionIndexY), MeshSectionIndex);

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [&]()
		{
			auto WorldGenTask = new FAsyncTask< FAsyncWorldGenerater>(this);
			WorldGenTask->StartBackgroundTask();
			WorldGenTask->EnsureCompletion();
			delete WorldGenTask;
		}
	);
}

void AWorldGenerator::DrawTile()
{
	TileDataReady = false;
	int FurthestTileIndex = GetFurthestTile();
	if (FurthestTileIndex > -1)
	{
		TArray<int> ValueArray{};
		TArray<FIntPoint> KeyArray{};
		QueuedTiles.GenerateKeyArray(KeyArray);
		QueuedTiles.GenerateValueArray(ValueArray);

		int ReplaceableMeshSection = ValueArray[FurthestTileIndex];
		FIntPoint ReplaceableTile = KeyArray[FurthestTileIndex];

		TerrainMesh->UpdateMeshSection(ReplaceableMeshSection, SubVertices, SubNormals, SubUVs, TArray<FColor>(), SubTangents);
		QueuedTiles.Add(FIntPoint(SectionIndexX, SectionIndexY), ReplaceableMeshSection);
		QueuedTiles.Remove(ReplaceableTile);
	}
	else
	{
		TerrainMesh->CreateMeshSection(MeshSectionIndex, SubVertices, SubTriangles, SubNormals, SubUVs, TArray<FColor>(), SubTangents, true);
		if (TerrainMaterial)
		{
			TerrainMesh->SetMaterial(MeshSectionIndex, TerrainMaterial);
		}
		MeshSectionIndex++;
	}

	SubVertices.Empty();
	SubUVs.Empty();
	SubTriangles.Empty();
	SubNormals.Empty();
	SubTangents.Empty();
	GeneratorBusy = false;
}

FVector AWorldGenerator::GetPlayerLocation() const
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if(PlayerCharacter)
		return PlayerCharacter->GetActorLocation();
	return FVector();
}

FVector2D AWorldGenerator::GetTileLocation(FIntPoint TileCoordinate) const
{
	return FVector2D(TileCoordinate * FIntPoint(xVertexCount - 1, yVertexCount - 1) * CellSize)
		+ FVector2D(xVertexCount - 1, yVertexCount - 1) * CellSize / 2;
}

bool AWorldGenerator::GetClosestQueuedTile(FIntPoint& ClosestTile) const
{
	float CollestDistance = TNumericLimits<float>::Max();
	bool bFound = false;
	for (const auto& Entry : QueuedTiles)
	{
		const FIntPoint& Key = Entry.Key;
		const int& Value = Entry.Value;
		if (Value == -1)
		{
			FVector2D TileLocation = GetTileLocation(Key);
			FVector PlayerLocation = GetPlayerLocation();
			float Distance = FVector2D::DistSquared(FVector2D(PlayerLocation.X, PlayerLocation.Y), TileLocation);
			if (Distance < CollestDistance)
			{
				ClosestTile = Key;
				CollestDistance = Distance;
				bFound = true;
			}
		}
	}
	return bFound;
}

int AWorldGenerator::GetFurthestTile() const
{
	float FurthestDistance = -1;
	int FutheestTileIndex = -1;
	int CurrentIndex{};
	for (const auto& Entry : QueuedTiles)
	{
		const FIntPoint& Key = Entry.Key;
		const int& Value = Entry.Value;
		if (Value != -1)
		{
			FVector2D TileLocation = GetTileLocation(Key);
			FVector PlayerLocation = GetPlayerLocation();
			float Distance = FVector2D::Distance(FVector2D(PlayerLocation.X, PlayerLocation.Y), TileLocation);
			if (Distance > FurthestDistance && Distance > TileRelplaceableDistance )
			{
				FurthestDistance = Distance;
				FutheestTileIndex = CurrentIndex;
				UE_LOG(LogTemp, Warning, TEXT("Furthest Tile X:%d Y:%d Distance:%f"), Key.X, Key.Y, Distance);
			}
		}
		CurrentIndex++;
	}
	return FutheestTileIndex;
}

float AWorldGenerator::GetHeight(float inX, float inY) const
{
	float HeightScale{};
	int nSize = amplitudes.Num();
	for (int i = 0; i < nSize; i++)
	{
		HeightScale += PerlinNoiseExtended(inX, inY, scales[i], amplitudes[i]);
	}


	return HeightScale;
		//PerlinNoiseExtended(inX, inY, 0.00001f, 20000, FVector2D(0.1f))
		//+ PerlinNoiseExtended(inX, inY, 0.0001f, 10000, FVector2D(0.2f))
		//+ PerlinNoiseExtended(inX, inY, 0.001f, 500, FVector2D(0.3f))
		//+ PerlinNoiseExtended(inX, inY, 0.01f, 100, FVector2D(0.4f));
}

float AWorldGenerator::PerlinNoiseExtended(float inX, float inY, float scale, float Amplitude, FVector2D offset) const
{
	return FMath::PerlinNoise2D(FVector2D(inX, inY) * scale + FVector2D(0.1f,0.1f) + offset) * Amplitude;
}

void FAsyncWorldGenerater::DoWork()
{
	WorldGenerator->GenerateTerrain(WorldGenerator->SectionIndexX, WorldGenerator->SectionIndexY);
}
