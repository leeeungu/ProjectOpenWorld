#pragma once

#include "CoreMinimal.h"
#include "Creature/Component/CreatureActionComponent.h"
#include "CreatureAction_Building.generated.h"

class ABuildingActor;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }
class UStaticMeshComponent;
class UStaticMesh;

UCLASS(ClassGroup = (CreatureAction), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UCreatureAction_Building : public UCreatureActionComponent
{
	GENERATED_BODY()
private:
	TSoftObjectPtr<ABuildingActor> TargetBuilding{};
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Building")
	TObjectPtr<UStaticMesh> BuildToolMesh{};
	TObjectPtr<UStaticMeshComponent> MeshComponent{};
public:
	UCreatureAction_Building();
	virtual bool ActionStart_Implementation( AActor* SendActor, AActor* TargetActor = nullptr) override;
	virtual bool ActionEnd_Implementation() override;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void EndBuilding();

};
