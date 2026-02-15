#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "GameBase/Interface/GenerateWorldInterface.h"
#include "BossTower.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;
class ATargetPoint;

UCLASS()
class PROJECTOPENWORLD_API ABossTower : public AActor, public IInteractionInterface,
	public IGenerateWorldInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "BossTower")
	TObjectPtr<UStaticMeshComponent> TowerMeshComponent{};
	UPROPERTY(VisibleAnywhere, Category = "BossTower")
	TObjectPtr < UWidgetComponent> BossTowerWidgetComponent{};

	UPROPERTY(EditAnywhere, Category = "BossTower")
	TObjectPtr<ATargetPoint>  BossPoint{};
	UPROPERTY(EditAnywhere, Category = "BossTower")
	TObjectPtr <AActor> BoosLanscape{};

public:	
	ABossTower();

protected:
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// IInteractionInterface
	virtual void OnBeginDetected_Implementation(ACharacter* pOther) override;
	virtual void OnEndDetected_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
	virtual void OnInteraction_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionCanceled_Implementation(ACharacter* pOther) override;
	//// IGenerateWorldInterface
	//virtual void NewGenerateWorldEvent(const FGenerateSectionData& SectionData) override;
	//virtual void DelGenerateWorldEvent(const FGenerateSectionData& SectionData) override;



};
