#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerItemComponent.generated.h"

class UBaseItemObject;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPlayerItemComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemManager")
	TSet< TSubclassOf<UBaseItemObject>> ItemActor{};
public:	
	UPlayerItemComponent();

protected:
	virtual void BeginPlay() override;

public:	
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "ItemManager")
	virtual bool RegisterItemActor(TSubclassOf<UBaseItemObject> InItemObject);
	UFUNCTION(BlueprintCallable, Category = "ItemManager")
	virtual void UnRegisterItemActor(TSubclassOf<UBaseItemObject> InItemObject);
		
};
