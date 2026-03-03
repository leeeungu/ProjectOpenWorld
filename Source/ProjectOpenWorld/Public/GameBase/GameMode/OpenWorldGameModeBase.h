#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameBase/Subsystem/SoundGameInstanceSubsystem.h"
#include "OpenWorldGameModeBase.generated.h"


UCLASS()
class PROJECTOPENWORLD_API AOpenWorldGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundBGM")
	TEnumAsByte<EBGMSoundType> StartBGMType = EBGMSoundType::BGMST_None;
public:
	virtual void StartPlay() override;
};
