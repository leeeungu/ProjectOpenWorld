#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponeWidget.generated.h"


class UTextBlock;
class UImage;
class UCanvasPanel;
enum class EWeapone : uint8;

UCLASS()
class PROJECTOPENWORLD_API UWeaponeWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Weapone")
	TObjectPtr<UTextBlock> NameText{};
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Weapone")
	TObjectPtr<UImage> MouseImage{};
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Weapone")
	TObjectPtr<UImage> ChangeImage{};
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Weapone")
	TObjectPtr<UCanvasPanel> WeaponSlot{};
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Weapone")
	TObjectPtr<UImage> WeaponImage{};

public:
	void ChangeWeapone(FName NewWeaponeID, EWeapone NewWeaponeType);

	virtual void NativeConstruct() override;
};
