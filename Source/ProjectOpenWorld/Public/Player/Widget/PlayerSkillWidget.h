#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerSkillWidget.generated.h"


enum class EPlayerAttackType : uint8;
class UPlayerAttackComponent;
class UTextBlock;
class UWidgetAnimation;
class UTexture2D;
class UImage;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerSkillWidget", meta = (BindWidget))
	TObjectPtr<UImage> SkillImage{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerSkillWidget")
	
	TObjectPtr<UTexture2D> SkillImageTexture{};
	
	float RemainTime{};

	float GetAnimationPlayRate() const;
	bool bIsCoolingDown{ false };
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	virtual void OnMontagePlay(EPlayerAttackType InAttackType);
	
};
