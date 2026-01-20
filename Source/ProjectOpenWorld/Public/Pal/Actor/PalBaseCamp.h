#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameBase/Interface/GenerateWorldInterface.h"
#include "PalBaseCamp.generated.h"

class UPalStorageComponent;
class UPalCommanderComponent;
class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;
class UNavigationInvokerComponent;

UCLASS()
class PROJECTOPENWORLD_API APalBaseCamp : public AActor, public IGenerateWorldInterface
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
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UNavigationInvokerComponent> NavInvoker{};
	
public:	
	APalBaseCamp();
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void  OnCampBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCampEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
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

	// GenerateWorldInterface
	virtual void NewGenerateWorldEvent(const FGenerateSectionData& SectionData) override;
	virtual void DelGenerateWorldEvent(const FGenerateSectionData& SectionData) override;
};
