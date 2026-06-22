#pragma once
#include "CoreMinimal.h"
#include "Building/Actor/BuildingActor.h"
#include "SubSystem/GPT/GPTResponseInterface.h"
#include "PalGroupBanner.generated.h"

class UVaRestRequestJSON;
class UTexture2D;

UCLASS()
class PROJECTOPENWORLD_API APalGroupBanner : public ABuildingActor, public IGPTResponseInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sticker")
	TSubclassOf<UUserWidget> WidgetClass{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sticker")
	TObjectPtr< UUserWidget> Widget{};

public:
	UFUNCTION(BlueprintCallable, Category = "TeamFlag")
	void SendFlagRequest(const FString& Prompt);

	virtual void OnRequestComplete_Implementation(UVaRestRequestJSON* Request) override;
	virtual void OnRequestFail_Implementation(UVaRestRequestJSON* Request) override;

	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override;
protected:
	void SetFlagImage(UTexture2D* pImage);
};