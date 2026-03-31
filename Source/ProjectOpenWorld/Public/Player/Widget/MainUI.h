#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

class UWeaponeWidget;
enum class EWeapone : uint8;

UCLASS()
class PROJECTOPENWORLD_API UMainUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Weapone")
	TObjectPtr<UWeaponeWidget> WBP_WeaponUI{};


public:
	void ChangeWeapone(FName NewWeaponeID, EWeapone NewWeaponeType);
};
