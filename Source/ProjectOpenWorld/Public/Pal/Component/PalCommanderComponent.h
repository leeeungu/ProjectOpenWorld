#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include <list>
#include "PalCommanderComponent.generated.h"

class ABaseCreature;
class ICommanderManageable;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalCommanderComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TSet< TObjectPtr<ABaseCreature>> pals{};

	std::list<TObjectPtr<AActor>> WorkList;
	std::list< TObjectPtr<ABaseCreature>> NotWorkPals{};
public:	
	UPalCommanderComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void FinishCommand(AActor* PalActor, FPalCommand Command);

	bool StartWork(ABaseCreature* pal, const FPalCommand& Command);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StorePal(AActor* NewPal);
	void RemovePal(AActor* targetPal);

	void RegisterWork(AActor* WorkActor);

	UFUNCTION(BlueprintCallable, Category = "Commander")
	bool CommadAll(const FPalCommand& Command);

	UFUNCTION(BlueprintCallable, Category = "Commander")
	bool CommadReady(const FPalCommand& Command);

};
