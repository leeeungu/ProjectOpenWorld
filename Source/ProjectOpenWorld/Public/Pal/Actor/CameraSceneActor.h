#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameBase/Interface/MonsterInteractionInterface.h"
#include "CameraSceneActor.generated.h"

class UCameraComponent;
class ACharacter;
class USplineComponent;

UENUM(BlueprintType)
enum class ETargetType : uint8
{
	Player,
	Custom,
	Monster
};
// ACameraRig_Rail ÀÌ¶û °°À½
UCLASS()
class PROJECTOPENWORLD_API ACameraSceneActor : public AActor, public IMonsterInteractionInterface
{
	GENERATED_BODY()
protected:
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraSceneActor")
	//USceneComponent* Root;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraSceneActor")
	uint8 InteractionID{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraSceneActor")
	TObjectPtr<UCameraComponent> CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraSceneActor")
	TObjectPtr<USplineComponent> SplineComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr< ACharacter> CachedPlayerCharacter{};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr< AActor>  PreviousViewTarget{};
	
	int32 SplinePointIndex{ 0 };
	bool bIsCameraMoving{ false };
	UPROPERTY(EditAnywhere, Category = "CameraSceneActor")
	float FadeInTime = 0.f;
	UPROPERTY(EditAnywhere, Category = "CameraSceneActor")
	float FadeOutTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "CameraSceneActor")
	float SceneTime = 2.f;
	float currentTime{};
	UPROPERTY(EditAnywhere, Category = "CameraSceneActor")
	bool bLoop{};
	UPROPERTY(EditAnywhere, Category = "CameraSceneActor")
	bool bEndWait{};

	UPROPERTY(EditAnywhere, Category = "CameraSceneActor")
	ETargetType TargetType{};

	TObjectPtr< ACharacter> Monster{};

	UPROPERTY(VisibleAnywhere, Category = "CameraSceneActor")
	TObjectPtr < USceneComponent> LookAtTargetComp{ };

	TObjectPtr < USceneComponent> CustomTargetComp{ };

public:	
	ACameraSceneActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void OnInteractionEvent_Implementation(ACharacter* TargetMonster) override;
	virtual uint8 GetInteractionID_Implementation() const override { return InteractionID; }
	virtual void OnInteractionStartEvent_Implementation(ACharacter* TargetMonster) override;
	virtual void OnInteractionEndEvent_Implementation(ACharacter* TargetMonster) override;
};
