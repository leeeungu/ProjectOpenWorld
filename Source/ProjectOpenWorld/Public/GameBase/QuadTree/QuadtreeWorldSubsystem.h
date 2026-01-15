#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "QuadtreeWorldSubsystem.generated.h"

enum class EUpdateTransformFlags : int32;
enum class ETeleportType : uint8;
	class AWorldGenerator;

UCLASS()
class PROJECTOPENWORLD_API UQuadtreeWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
private:
	static TObjectPtr<UQuadtreeWorldSubsystem> Instance;
	TObjectPtr<AWorldGenerator> WorldGenerator{};
	int PlayerSectionX{};
	int PlayerSectionY{};
	int SectionRadius{ 3 };

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void OnUpdatePlayerLocation(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);
};
