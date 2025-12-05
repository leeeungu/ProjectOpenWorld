#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PalBaseCamp.generated.h"

class UPalStorageComponent;
class UPalCommanderComponent;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECTOPENWORLD_API APalBaseCamp : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UPalStorageComponent> PalStore{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UPalCommanderComponent> PalCommander{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Root{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CampMesh{};
	
public:	
	APalBaseCamp();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	FORCEINLINE UPalStorageComponent* GetPalStoreComponent() const {return PalStore;}
	UFUNCTION(BlueprintPure)
	FORCEINLINE UPalCommanderComponent* GetPalCommanderComponent() const {return PalCommander;}
};
