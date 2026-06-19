#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SubSystem/GPT/GPTResponseInterface.h"
#include "Player/Interface/MainWidgetInterface.h"
#include "MakeStickerWidget.generated.h"

class IGPTResponseInterface;

UCLASS()
class PROJECTOPENWORLD_API UMakeStickerWidget : public UUserWidget, 
	public IMainWidgetInterface
	//public IGPTResponseInterface, 
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TScriptInterface< IGPTResponseInterface> StickerTarget{};

	bool bReceivedRequest{ false };
public:
	// igptinterface
	//virtual void OnRequestComplete_Implementation(UVaRestRequestJSON* Request) override;
	//virtual void OnRequestFail_Implementation(UVaRestRequestJSON* Request) override;

	// imainwidgetinterface
	virtual bool SetMainWidget() override;
	virtual void UnSetMainWidget() override;

	void SetStickerTarget(TScriptInterface<IGPTResponseInterface> Target);

	UFUNCTION(BlueprintCallable, Category = "Sticker")
	void SendRequestToGPT(const FText& Prompt, ETextCommit::Type CommitMethod);

	UFUNCTION(BlueprintCallable, Category = "Sticker")
	void CloseWidget();

	bool IsReceivedRequest() const { return bReceivedRequest; }
};
