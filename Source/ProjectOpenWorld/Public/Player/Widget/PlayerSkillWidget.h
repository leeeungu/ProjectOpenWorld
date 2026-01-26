#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerSkillWidget.generated.h"


enum class EPlayerAttackType : uint8;
class UPlayerAttackComponent;
class UTextBlock;
class UWidgetAnimation;

UCLASS()
class PROJECTOPENWORLD_API UPlayerSkillWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerSkillWidget")
	EPlayerAttackType SkillType{};

	TObjectPtr< UPlayerAttackComponent> PlayerAttackComponent{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerSkillWidget", meta = (BindWidgetAnim), Transient)
	TObjectPtr< UWidgetAnimation> CoolDownAnimation{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerSkillWidget", meta = (BindWidget))
	TObjectPtr<UTextBlock> CoolTimeTextBlock{};

	float RemainTime{};

	float GetAnimationPlayRate() const;
	bool bIsCoolingDown{ false };
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	virtual void OnMontagePlay(EPlayerAttackType InAttackType);
	
};
