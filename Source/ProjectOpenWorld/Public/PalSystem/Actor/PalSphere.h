#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PalSphere.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROJECTOPENWORLD_API APalSphere : public AActor
{
	GENERATED_BODY()
private:
	float CurTime{};

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PalSphere")
	TObjectPtr<UStaticMeshComponent> Mesh{};
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalSphere")
	TObjectPtr<UCurveFloat> Curve{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PalSphere")
	FVector StartLocation{};
public:	
	APalSphere();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void Hitted(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void ThrowSphere();



};
