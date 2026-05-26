#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

class UWeaponeWidget;
enum class EWeapone : uint8;
class UPlayerStatusProgress;
class UStatComponent;
enum class EStatusType : uint8;

UCLASS()
class PROJECTOPENWORLD_API UMainUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Weapone")
	TObjectPtr<UWeaponeWidget> WBP_WeaponUI{};
	UPROPERTY(meta = (BindWidget), Category = "Status", EditDefaultsOnly)
	TObjectPtr<UPlayerStatusProgress> PlayerHPBar{};
	//UPROPERTY(meta = (BindWidgetOptional), Category = "Status", EditDefaultsOnly)
	TObjectPtr<UPlayerStatusProgress> PlayerHealthBar{};
	//UPROPERTY(meta = (BindWidgetOptional), Category = "Status", EditDefaultsOnly)
	TObjectPtr<UPlayerStatusProgress> PlayerShieldBar{};

public:
	void ChangeWeapone(FName NewWeaponeID, EWeapone NewWeaponeType);
	void SetStatWidget(UStatComponent* StatCom);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:
};
