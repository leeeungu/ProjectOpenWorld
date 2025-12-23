#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PalBaseCamp.generated.h"

class UPalStorageComponent;
class UPalCommanderComponent;
class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;

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
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr < USphereComponent> CampBounds{};
public:	
	APalBaseCamp();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void CommandActorSpawned(AActor* NewActor);
public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "BaseCamp")
	bool SpawnPal(int Index, AActor*& Spawned);
	UFUNCTION(BlueprintCallable, Category = "BaseCamp")
	void StorePalClass(TSubclassOf<AActor> NewPal, int Index);
	UFUNCTION(BlueprintCallable, Category = "BaseCamp")
	void RemovePalClass(TSubclassOf<AActor> targetPal, int Index);
	UFUNCTION(BlueprintPure)
	FORCEINLINE UPalStorageComponent* GetPalStoreComponent() const {return PalStore;}
	UFUNCTION(BlueprintPure)
	FORCEINLINE UPalCommanderComponent* GetPalCommanderComponent() const {return PalCommander;}

	UFUNCTION()
	void PalDead(AActor* DeadPal);
};
