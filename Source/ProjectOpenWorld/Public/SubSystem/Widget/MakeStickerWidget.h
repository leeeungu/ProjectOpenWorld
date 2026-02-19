#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SubSystem/GPT/GPTResponseInterface.h"
#include "Player/Interface/MainWidgetInterface.h"
#include "MakeStickerWidget.generated.h"

class AStickerDecalActor;



UCLASS()
class PROJECTOPENWORLD_API UMakeStickerWidget : public UUserWidget, public IGPTResponseInterface, public IMainWidgetInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TWeakObjectPtr< AStickerDecalActor> StickerTarget{};

	bool bReceivedRequest{ false };
public:
	// igptinterface
	virtual void OnRequestComplete_Implementation(UVaRestRequestJSON* Request) override;
	virtual void OnRequestFail_Implementation(UVaRestRequestJSON* Request) override;

	// imainwidgetinterface
	virtual bool SetMainWidget() override;
	virtual void UnSetMainWidget() override;

	void SetStickerTarget(AStickerDecalActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Sticker")
	void SendRequestToGPT(const FText& Prompt, ETextCommit::Type CommitMethod);

	UFUNCTION(BlueprintCallable, Category = "Sticker")
	void CloseWidget();

	bool IsReceivedRequest() const { return bReceivedRequest; }
};
