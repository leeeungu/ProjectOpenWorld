#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PlayerPalManagerComponent.generated.h"

class ABaseCreature;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPlayerPalManagerComponent : public USceneComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "PlayerPal")
	TObjectPtr< ABaseCreature> SpawnedPal{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerPal")
	float DespawnDistance{ 1000.f };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerPal")
	float FollowDistance{ 600.f };

public:	
	UPlayerPalManagerComponent();

protected:
	virtual void BeginPlay() override;

	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport = ETeleportType::None) override;

public:	
//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category = "PlayerPal")
	void SetTargetPal(TSubclassOf< ABaseCreature> PalClass);
	void SpawnPal();
	void DespawnPal();
};
