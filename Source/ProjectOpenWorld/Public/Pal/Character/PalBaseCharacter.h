#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PalBaseCharacter.generated.h"

class UStatComponent;

UCLASS()
class PROJECTOPENWORLD_API APalBaseCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "PalBase")
	TObjectPtr< UStatComponent> StatComponent{};
public:
	UStatComponent* GetStatComponent() const { return StatComponent; }
	void UseControllerDesiredRotation();
	void UseOrientRotationToMovement();
};
