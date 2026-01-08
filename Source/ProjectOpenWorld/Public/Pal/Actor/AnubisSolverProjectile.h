#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameBase/Interface/MonsterInteractionInterface.h"
#include "Interaction/InteractionInterface.h"
#include "AnubisSolverProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
enum class EAnubisPatternType : uint8;
class ABasePlayer;

UCLASS()
class PROJECTOPENWORLD_API AAnubisSolverProjectile : public AActor, public IInteractionInterface, public IMonsterInteractionInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> Collision{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anubis Solver Projectile")
	float MoveSpeed{ 600.0f };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anubis Solver Projectile")
	FVector MoveDirection{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anubis Solver Projectile")
	float RotateSpeed{ 720.0f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anubis Solver Projectile")
	EAnubisPatternType PatternType{};

	bool bProjectileActive{ false };

	UPROPERTY()
	TObjectPtr< ABasePlayer> TargetPlayer{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anubis Solver Projectile")
	uint8 InteractionID{};
public:	
	AAnubisSolverProjectile();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAnubisSolverBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnAnubisSolverEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:	
	virtual void Tick(float DeltaTime) override;

	// Interaction Interface
	virtual void OnBeginDetected_Implementation(ACharacter* pOther) override {}
	virtual void OnEndDetected_Implementation(ACharacter* pOther) override {}
	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
	virtual void OnInteraction_Implementation(ACharacter* pOther) override {}
	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override {}
	virtual void OnInteractionCanceled_Implementation(ACharacter* pOther) override {}


	// Monster Interaction Interface
	virtual void OnInteractionEvent_Implementation(ACharacter* TargetMonster) override;
	virtual void OnInteractionStartEvent_Implementation(ACharacter* TargetMonster) override;
	virtual void OnInteractionEndEvent_Implementation(ACharacter* TargetMonster) override;
	virtual uint8 GetInteractionID_Implementation() const override { return InteractionID; }
};
