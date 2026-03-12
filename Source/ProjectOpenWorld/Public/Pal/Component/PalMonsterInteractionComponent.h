#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameBase/Interface/MonsterInteractionInterface.h"
#include "PalMonsterInteractionComponent.generated.h"
UENUM(BlueprintType)
enum class EMonsterInteractionEvent : uint8
{
	Default UMETA(DisplayName = "Default"),
	InteractionStart UMETA(DisplayName = "InteractionStart"),
	InteractionEnd UMETA(DisplayName = "InteractionEnd"),
};

USTRUCT(BlueprintType)
struct FPalMonsterInteractionData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pal Monster Interface")

	TArray < TObjectPtr<AActor>> InteractionList{};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalMonsterInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pal Monster Interface")
	TArray<FPalMonsterInteractionData> MonsterInteractionInterfaceList{};
	TArray<TPair<TObjectPtr<AActor>, TObjectPtr<ACharacter>>> setActiveInteraction;
public:
	UPalMonsterInteractionComponent();

protected:
	virtual void BeginPlay() override;

public:
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Pal Monster Interface")
	void RegisterMonsterInteractionInterface(uint8 InteractionID, TScriptInterface<IMonsterInteractionInterface> MonsterInteractionInterface,
		AActor* Interaction);
	UFUNCTION(BlueprintPure, Category = "Pal Monster Interface")
	TArray<AActor*> GetMonsterInteractionInterfaceListByID(uint8 InteractionID) const;
	UFUNCTION(BlueprintCallable, Category = "Pal Monster Interface")
	void InvokeInteractionEvent(uint8 InteractionID, ACharacter* TargetMonster, EMonsterInteractionEvent Type = EMonsterInteractionEvent::Default);
	UFUNCTION(BlueprintCallable, Category = "Pal Monster Interface")
	void EndActiveInteraction();

};
