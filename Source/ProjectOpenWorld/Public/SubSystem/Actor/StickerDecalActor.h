#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "Interaction/InteractionInterface.h"
#include "StickerDecalActor.generated.h"

class UUserWidget;
class ACharacter;


UCLASS()
class PROJECTOPENWORLD_API AStickerDecalActor : public ADecalActor, public IInteractionInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sticker")
	TSubclassOf<UUserWidget> WidgetClass{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sticker")
	TObjectPtr< UUserWidget> Widget{};

protected:
	virtual void BeginPlay() override;

public:
	// IInteractionInterface
	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override;

	void SetStickerImage(UTexture2D* pImage);
};
