#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PalMontageTestActor.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class UAnimMontage;
class UBaseAnimInstance;
UCLASS()
class PROJECTOPENWORLD_API APalMontageTestActor : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "Test")
	TObjectPtr<USkeletalMeshComponent> Mesh{};
	UPROPERTY(VisibleAnywhere, Category = "Test")
	TObjectPtr<USphereComponent> Sphere{};
	UPROPERTY(EditAnywhere, Category = "Test")
	TArray<TObjectPtr<UAnimMontage>> TargetMontage{};
	TObjectPtr<UBaseAnimInstance> AnimInstance{};
	bool bIsPlayingMontage{};
public:	
	APalMontageTestActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void PlayMontage();
};
