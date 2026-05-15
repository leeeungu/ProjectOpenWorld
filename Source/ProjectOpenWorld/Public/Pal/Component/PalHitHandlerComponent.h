#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalDamageType.h"
#include "PalHitHandlerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageTaken, const FPalDamagePayload&, Payload);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalHitHandlerComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintAssignable, Category = "Hitboxes")
	FOnDamageTaken OnDamageTaken{};

	void TakeDamage(const FPalDamagePayload& Payload);
};
