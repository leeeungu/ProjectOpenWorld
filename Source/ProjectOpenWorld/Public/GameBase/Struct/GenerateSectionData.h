#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GenerateSectionData.generated.h"

struct FProcMeshTangent;

USTRUCT()
struct FGenerateSectionData
{
	GENERATED_USTRUCT_BODY()
public:
	FIntPoint SectionID{};
	const TArray<FVector>* Vertices{};
	const TArray<FVector2D>* UVs{};
	const TArray<int32>* Triangles{};
	const TArray<FVector>* Normals{};
	const TArray<FProcMeshTangent>* Tangents{};
};