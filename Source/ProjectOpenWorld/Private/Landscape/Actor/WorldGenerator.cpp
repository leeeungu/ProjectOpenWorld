#include "Landscape/Actor/WorldGenerator.h"
#include "KismetProceduralMeshLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "UObject/ObjectSaveContext.h" //PreSave
#if WITH_EDITOR
#include "Subsystems/UnrealEditorSubsystem.h"  //PreSave
#endif
#include <queue>
#include <set>
#include "NavigationSystem.h"

AWorldGenerator::AWorldGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	//NavModifier = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifier"));
	//NavModifier->SetAreaClass(UNavArea_Default::StaticClass());
	arrTerrainMeshs.Reserve(10);
	for (int i = 0; i < 10; i++)
	{
		UProceduralMeshComponent* pTerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(FName(*(FString(TEXT("TerrainMesh") + FString::FromInt(i)))));
		arrTerrainMeshs.Push(pTerrainMesh);
		if (pTerrainMesh)
		{
			pTerrainMesh->ClearAllMeshSections();
			pTerrainMesh->SetVisibility(false);
			pTerrainMesh->SetActive(false);
			pTerrainMesh->SetComponentTickEnabled(false);
			pTerrainMesh->DetachFromParent();
			pTerrainMesh->MarkRenderStateDirty();
			pTerrainMesh->SetCanEverAffectNavigation(true);
			pTerrainMesh->bNavigationRelevant = true;
			pTerrainMesh->SetCollisionProfileName(TEXT("BlockAll"));
		}
	}
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
	//UE_LOG(LogTemp, Warning, TEXT("AWorldGenerator PreSave Completed"));
#endif // 
}

#if WITH_EDITOR
void AWorldGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.HasArchetypeInstanceChanged(this))
	{
		//UE_LOG(LogTemp, Warning, TEXT("AWorldGenerator Property Changed"));
		bPropertyChaned = true;
	}
}
#endif // 

void AWorldGenerator::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	MeshSectionIndex = 0;
	if (PlayerCharacter)
	{
		FIntPoint PlayerSectionIndex = GetSectionIndex(PlayerCharacter->GetActorLocation());
		PlayerSectionIndexX = PlayerSectionIndex.X;
		PlayerSectionIndexY = PlayerSectionIndex.Y;
		//UE_LOG(LogTemp, Warning, TEXT("AWorldGenerator Player Start At Section X:%d Y:%d"), PlayerSectionIndexX, PlayerSectionIndexY);
		GenerateTerrainAsync();
		PlayerCharacter->GetRootComponent()->TransformUpdated.AddUObject(this, &AWorldGenerator::OnUpdatePlayerLocation);
	}
}

void AWorldGenerator::OnUpdatePlayerLocation(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	FIntPoint PlayerSectionIndex = GetSectionIndex(UpdatedComponent->GetComponentLocation());
	if (PlayerSectionIndex.X == PlayerSectionIndexX && PlayerSectionIndex.Y == PlayerSectionIndexY)
	{
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("AWorldGenerator Player Move To Section X:%d Y:%d"), PlayerSectionIndex.X, PlayerSectionIndex.Y);
	PlayerSectionIndexX = PlayerSectionIndex.X;
	PlayerSectionIndexY = PlayerSectionIndex.Y;
	GenerateTerrainAsync();
}

void AWorldGenerator::SetCurrentMesh()
{
	if (TerrainMesh != GenerateTerrain)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AWorldGenerator Switch Mesh %d"), CurrentMeshIndex);
		if (TerrainMesh)
		{
			TerrainMesh->ClearAllMeshSections();
			TerrainMesh->SetVisibility(false);
			TerrainMesh->SetActive(false);
			TerrainMesh->SetComponentTickEnabled(false);
			TerrainMesh->DetachFromParent();
			TerrainMesh->MarkRenderStateDirty();
		}
		TerrainMesh = GenerateTerrain;
		if (TerrainMesh)
		{
			TerrainMesh->SetupAttachment(TerrainMesh);
			TerrainMesh->SetVisibility(true);
			TerrainMesh->SetActive(true);
			TerrainMesh->SetComponentTickEnabled(true);
			//TerrainMesh->UpdateNavigationBounds();
			UNavigationSystemV1::UpdateComponentInNavOctree(*TerrainMesh);
		}
	}
}

void AWorldGenerator::SetGeneratorMesh()
{
	if (arrTerrainMeshs.Num() <= 1)
	{
		GenerateTerrain = TerrainMesh;
		return;
	}

	CurrentMeshIndex++;
	if (!arrTerrainMeshs.IsValidIndex(CurrentMeshIndex))
		CurrentMeshIndex = 0;
	GenerateTerrain = arrTerrainMeshs[CurrentMeshIndex];
	if (GenerateTerrain)
	{
		GenerateTerrain->ClearAllMeshSections();
		GenerateTerrain->SetVisibility(false);
		GenerateTerrain->SetActive(false);
		GenerateTerrain->SetComponentTickEnabled(false);

		MaxSection = SumVertices.Num();
		MeshSectionIndex = 0;
		UpdateTerrain();
	}
	GeneratorBusy = false;
}

void AWorldGenerator::UpdateTerrain()
{
	if (!GenerateTerrain)
	{
		return;
	}
	int nCount = MeshSectionIndex;
	int Max = FMath::Min(MaxSection, MeshSectionIndex + SectionCreateTickCount);
	for (int i = MeshSectionIndex; i < Max; i++)
	{
		GenerateTerrain->CreateMeshSection(i, SumVertices[i], SumTriangles[i], SumNormals[i], SumUVs[i], TArray<FColor>(), SumTangents[i], true);
		if (TerrainMaterial)
		{
			GenerateTerrain->SetMaterial(i, TerrainMaterial);
		}
	}
	MeshSectionIndex = Max;
	if (MaxSection == MeshSectionIndex)
	{
		SetCurrentMesh();
		GenerateTerrain = nullptr;
		int Sqr = SectionRadiusCount * SectionRadiusCount;
		SumVertices.Empty(Sqr);
		SumUVs.Empty(Sqr);
		SumTriangles.Empty(Sqr);
		SumNormals.Empty(Sqr);
		SumTangents.Empty(Sqr);
		TileDataReady = false;
	}
}
void AWorldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GeneratorBusy)
	{
		if (TileDataReady)
		{
			SetGeneratorMesh();
		}
		return;
	}
	else
	{
		if (TileDataReady)
		{
			UpdateTerrain();
		}
	}
}

void AWorldGenerator::GenerateTerrainAsync()
{
	if (GeneratorBusy)
	{
		if (TileDataReady)
		{
			SetCurrentMesh();
		}
		return;
	}
	if (TileDataReady)
		return;
	GeneratorBusy = true;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [&]()
		{
			auto WorldGenTask = new FAsyncTask< FAsyncWorldGenerater>(this);
				WorldGenTask->StartBackgroundTask();
				WorldGenTask->EnsureCompletion();
			delete WorldGenTask;
		}
	);
}

FVector AWorldGenerator::GetPlayerLocation() const
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if(PlayerCharacter)
		return PlayerCharacter->GetActorLocation();
	return FVector();
}

FIntPoint AWorldGenerator::GetSectionIndex(FVector Location)
{
	return FIntPoint(FMath::FloorToInt(Location.X / (CellSize * (xVertexCount - 1))),
		FMath::FloorToInt(Location.Y / (CellSize * (yVertexCount - 1))));
}

void FAsyncWorldGenerater::DoWork()
{
	int PlayerSectionIndexX = PlayerSectionIndex.X;
	int PlayerSectionIndexY = PlayerSectionIndex.Y;
	int NumOfSectionsX = WorldGenerator->SectionRadiusCount;
	int NumOfSectionsY = WorldGenerator->SectionRadiusCount;
	MeshSectionIndex = 0;
	int Dx[4] = { 1,-1,0,0 };
	int Dy[4] = { 0,0,1,-1 };
	int HalfX = NumOfSectionsX / 2;
	int HalfY = NumOfSectionsY / 2;
	using namespace std;

	queue<FAsyncWorldGenerater::TrraninNode> Q{};
	Q.push({ PlayerSectionIndexX, PlayerSectionIndexY });
	set<FAsyncWorldGenerater::TrraninNode> Visited{};
	Visited.insert({ PlayerSectionIndexX, PlayerSectionIndexY });
	int dissqr = WorldGenerator->SectionRadiusCount * WorldGenerator->SectionRadiusCount;
	while (!Q.empty())
	{
		FAsyncWorldGenerater::TrraninNode Node = Q.front();
		Q.pop();
		GenerateTerrainTile(Node.SectionIndexX, Node.SectionIndexY);
		for (int i = 0; i < 4; i++)
		{
			int NewX = Node.SectionIndexX + Dx[i];
			int NewY = Node.SectionIndexY + Dy[i];
			if( (NewX - PlayerSectionIndexX) * (NewX - PlayerSectionIndexX) + (NewY - PlayerSectionIndexY) * (NewY - PlayerSectionIndexY) > dissqr)
			{
				continue;
			}
			{
				FAsyncWorldGenerater::TrraninNode NewNode(NewX, NewY);
				if (Visited.find(NewNode) == Visited.end())
				{
					Visited.insert(NewNode);
					Q.push(NewNode);
				}
			}
		}
	}
	WorldGenerator->TileDataReady = true;
}

void FAsyncWorldGenerater::InitGenerater()
{
	int Index = WorldGenerator->CurrentMeshIndex + 1;
	if (Index >= WorldGenerator->arrTerrainMeshs.Num())
		Index = 0;
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
	WorldGenerator->SumVertices.Push(SubVertices);
	WorldGenerator->SumUVs.Push(SubUVs);
	WorldGenerator->SumTriangles.Push(SubTriangles);
	WorldGenerator->SumNormals.Push(SubNormals);
	WorldGenerator->SumTangents.Push(SubTangents);
}
float FAsyncWorldGenerater::GetHeight(float inX, float inY) const
{
	float HeightScale{};
	int nSize = WorldGenerator->amplitudes.Num();
	for (int i = 0; i < nSize; i++)
	{
		HeightScale += PerlinNoiseExtended(inX, inY, WorldGenerator->scales[i], WorldGenerator->amplitudes[i] , FVector2D(0.1) * i);
	}
	return HeightScale;
}

float FAsyncWorldGenerater::PerlinNoiseExtended(float inX, float inY, float scale, float Amplitude, FVector2D offset) const
{
	return FMath::PerlinNoise2D(FVector2D(inX, inY) * scale + FVector2D(0.1f, 0.1f) + offset) * Amplitude;
}

//PerlinNoiseExtended(inX, inY, 0.00001f, 20000, FVector2D(0.1f))
//+ PerlinNoiseExtended(inX, inY, 0.0001f, 10000, FVector2D(0.2f))
//+ PerlinNoiseExtended(inX, inY, 0.001f, 500, FVector2D(0.3f))
//+ PerlinNoiseExtended(inX, inY, 0.01f, 100, FVector2D(0.4f));