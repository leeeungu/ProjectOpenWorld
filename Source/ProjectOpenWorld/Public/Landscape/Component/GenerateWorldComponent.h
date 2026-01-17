#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GenerateWorldComponent.generated.h"

struct FProcMeshTangent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) ,Abstract)
class PROJECTOPENWORLD_API UGenerateWorldComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class UGeneratorSectionComponent;
public:	
	UGenerateWorldComponent() ;

protected:
	//virtual void BeginPlay() override;

	UFUNCTION()
	virtual void StartGenerateWorld() {};
	UFUNCTION()
	virtual void UpdateGenerateWorld(int SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) {}
	UFUNCTION()
	virtual void FinishGenerateWorld() {};

public:	
	virtual void Initialize(USceneComponent* ParentComponent) {};
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
