#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PlayerDetectCollision.generated.h"

class ABasePlayer;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDetectChanged, AActor*, Actor, bool, bDetected);


UCLASS()
class PROJECTOPENWORLD_API UPlayerDetectCollision : public USphereComponent
{
	GENERATED_BODY()

	TObjectPtr< ABasePlayer> OwnerPlayer{};
public:
	UPlayerDetectCollision();

	UPROPERTY(BlueprintAssignable, Category = "Detect")
	FOnDetectChanged OnDetectChanged{};
protected:
	UFUNCTION()
	void OnDetectBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual void BeginPlay() override;
};
