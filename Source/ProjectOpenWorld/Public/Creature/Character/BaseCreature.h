#pragma once

#include "CoreMinimal.h"
#include "GameBase/BaseCharacter.h"
#include "Creature/Interface/CreatureActionInterface.h"
#include "Creature/Interface/CreatureMessageInterface.h"
#include "BaseCreature.generated.h"



UCLASS()
class PROJECTOPENWORLD_API ABaseCreature : public ABaseCharacter, public ICreatureMessageInterface
{
	GENERATED_BODY()
protected:
	TScriptInterface<ICreatureActionInterface> ActionComponents[(uint8)ECreatureActionType::Action_Max]{};
	
	UPROPERTY(EditDefaultsOnly, Meta = (Bitmask, BitmaskEnum = "ECreatureRollType"))
	int32 CreatureRoll{};
protected:
	virtual void BeginPlay() override;
public:
	virtual void ReceiveMessage_Implementation(EMessageType MessageType, AActor* SendActor, UObject* TargetObject = nullptr) override;
};
