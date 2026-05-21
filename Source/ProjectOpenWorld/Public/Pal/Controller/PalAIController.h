#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h" 
#include "PalAIController.generated.h"

class ABaseCharacter;
class UPalAIMoveComponent;
enum class EPalAIMonsterState : uint8;
UCLASS()
class PROJECTOPENWORLD_API APalAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PalAIMove")
	TObjectPtr<UPalAIMoveComponent> AIMoveComponent{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UBehaviorTree> BTree{};
	UPROPERTY()
	TObjectPtr < ABaseCharacter> OwnerPal{};
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION(BlueprintCallable, Category = "BB")
	void SetBBActor(FName BBKey, AActor* NewTarget);
	UFUNCTION(BlueprintCallable, Category = "BB")
	void SetBBLocation(FName BBKey, FVector NewLocation);
	UFUNCTION(BlueprintCallable, Category = "BB")
	void SetBBEnum(FName BBKey, uint8 NewEnum);
	UFUNCTION(BlueprintCallable, Category = "BB")
	void ClearBBValue(FName BBKey);
public:
	APalAIController();

	UFUNCTION(BlueprintPure, Category = "PalAIController")
	FORCEINLINE UPalAIMoveComponent* GetPalAIMoveComponent() const { return AIMoveComponent; }
	
	


};
