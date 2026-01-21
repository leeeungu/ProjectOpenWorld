#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "Creature/Character/BaseCreature.h"
#include <set>
#include "PalCommanderComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalCommanderComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	std::set<TObjectPtr<ABaseCreature>> pals{};
	
	TQueue< TObjectPtr<AActor>> WorkQueue{};

	TSet< TObjectPtr<AActor>> RegisteredWorks{};

	UPROPERTY()
	TObjectPtr<AActor> TargetWorkActor{};

	std::set<TObjectPtr<ABaseCreature>>::iterator ArrayIter{};
	int QueSize{};
public:	
	UPalCommanderComponent();

protected:
	virtual void BeginPlay() override;

	void UpdateWorkQueue();
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Commander")
	void StopAllPal();
	void StorePal(AActor* NewPal);
	void RemovePal(AActor* targetPal);

	UFUNCTION(BlueprintCallable, Category = "Commander")
	void RegisterWork(AActor* WorkActor);
	UFUNCTION(BlueprintCallable, Category = "Commander")
	void UnRegisterWork(AActor* WorkActor);

	UFUNCTION(BlueprintCallable, Category = "Commander")
	bool WorkAllPal(const FPalCommand& Command);

	UFUNCTION(BlueprintCallable, Category = "Commander")
	bool WorkOnePal(const FPalCommand& Command);

};
