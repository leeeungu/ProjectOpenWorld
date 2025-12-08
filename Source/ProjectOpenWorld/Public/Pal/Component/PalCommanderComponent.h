#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "PalCommanderComponent.generated.h"

class ABaseCreature;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalCommanderComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TSet< ABaseCreature*> pals{};
public:	
	UPalCommanderComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StorePal(AActor* NewPal);
	void RemovePal(AActor* targetPal);

	UFUNCTION(BlueprintCallable, Category = "Commander")
	void CommadAll(const FPalCommand& Command);

	UFUNCTION(BlueprintCallable, Category = "Commander")
	void CommadReady(const FPalCommand& Command);

};
