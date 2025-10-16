#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameBase/Interface/ConfirmationInterface.h"
#include "ConfirmationWidgetBase.generated.h"

class UTextBlock;

UCLASS()
class PROJECTOPENWORLD_API UConfirmationWidgetBase : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inferface", meta = (ExposeOnSpawn = "true"))
	TScriptInterface<IConfirmationInterface>  EventInterface{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly , Category = "Text", meta = (BindWidget))
	TObjectPtr<UTextBlock> AlertMessage{};
public:

	void SetEventInterface(UObject* OtherInterface) {
		EventInterface = TScriptInterface<IConfirmationInterface>(OtherInterface);
	}

	void SetAlertMessage(FText Message);
	
};
