// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pal/Interface/CommanderManageable.h"
#include "Interaction/InteractionInterface.h"
#include "ResourceActor.generated.h"

class UItemPrimaryDataAsset;
class AItemActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExtractEnd);

UCLASS()
class PROJECTOPENWORLD_API AResourceActor : public AActor, public ICommanderManageable , public IInteractionInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource")
	TObjectPtr<UStaticMeshComponent> ResourceMesh{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (ExposeOnSpawn = "TRUE"))
	TArray<TObjectPtr<UItemPrimaryDataAsset>> ItemData{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (ExposeOnSpawn = "TRUE"))
	int ExtractCount = 5;
	// count를 줘서 램덤으로 깍을 지  vs 내구도를 주고 램덤으로 줄지 

	UPROPERTY()
	TSet< TWeakObjectPtr<UObject>> InteractionList{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TSubclassOf< AItemActor> SpawnClass{};
public:
	FOnExtractEnd onExtractEnd{};
public:	
	// Sets default values for this actor's properties
	AResourceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	

	void SpawnRandomItem();
	bool CanExtract() { return  ExtractCount > 0; }
	// Called every frame
	//virtual void Tick(float DeltaTime) override {} ;


	virtual EPalCommandKind GetCommandKind_Implementation() override;
	virtual uint8 GetSubCommandType_Implementation() override;
	virtual FPalCommand GetCommand_Implementation() override;
	virtual bool IsCommandFinished_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Resource")
	void UpdateResource(ACharacter* pOther);

	virtual void OnBeginDetected_Implementation(ACharacter* pOther) override;
	virtual void OnEndDetected_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
	virtual void OnInteraction_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionCanceled_Implementation(ACharacter* pOther) override;

};
