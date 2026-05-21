#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TestPawn.generated.h"

class USphereComponent;

UCLASS()
class PROJECTOPENWORLD_API ATestPawn : public APawn
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Collision{};
public:
	ATestPawn();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:	
};
