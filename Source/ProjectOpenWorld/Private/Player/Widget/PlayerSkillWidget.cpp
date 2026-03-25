#include "Player/Widget/PlayerSkillWidget.h"
#include "Player/Character/BasePlayer.h"
#include "Player/Component/PlayerAttackComponent.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"

float UPlayerSkillWidget::GetAnimationPlayRate() const
{
	if (PlayerAttackComponent)
	{
		float CoolTimeValue = PlayerAttackComponent->GetMontageCoolTime(SkillType);
		if (CoolTimeValue > 0.0f)
		{
			return 1 / CoolTimeValue;
		}
	}
	return 0.0f;
}

void UPlayerSkillWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (SkillImage && SkillImageTexture)
	{
		SkillImage->SetBrushFromTexture(SkillImageTexture);
	}
}

void UPlayerSkillWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SkillImage && SkillImageTexture)
	{
		SkillImage->SetBrushFromTexture(SkillImageTexture);
	}
}

void UPlayerSkillWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ABasePlayer* Player = Cast<ABasePlayer>(GetOwningPlayerPawn());
	UE_LOG(LogTemp, Warning, TEXT("PlayerSkillWidget::NativeOnInitialized Player : %s"), *GetNameSafe(Player));
	if (Player)
	{
		PlayerAttackComponent = Player->GetPlayerAttackComponent();
		PlayerAttackComponent->OnPlayMontage.AddUniqueDynamic(this, &UPlayerSkillWidget::OnMontagePlay);
		Player->OnStateChangeDelegate.AddUniqueDynamic(this, &UPlayerSkillWidget::OnStateChange);
		UE_LOG(LogTemp, Warning, TEXT("PlayerSkillWidget::NativeOnInitialized PlayerAttackComponent : %s"), *GetNameSafe(PlayerAttackComponent));
	}
	if (CoolTimeTextBlock)
		CoolTimeTextBlock->SetVisibility(ESlateVisibility::Hidden);
	//TickFrequency
}

void UPlayerSkillWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bIsCoolingDown && CoolTimeTextBlock  )
	{
		RemainTime -= InDeltaTime;
		if (RemainTime < 0)
		{
			RemainTime = 0;
			bIsCoolingDown = false;
			CoolTimeTextBlock->SetVisibility( ESlateVisibility::Hidden);
		}
		CoolTimeTextBlock->SetText(FText::AsNumber(FMath::CeilToInt(RemainTime * 10.f) * 0.1f));
	}
}

void UPlayerSkillWidget::OnStateChange(EPlayerState NewPlayerState, EPlayerState PrePlayerState)
{
	if (NewPlayerState != EPlayerState::Battle)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UPlayerSkillWidget::OnMontagePlay(EPlayerAttackType InAttackType)
{
	float PlayRate = GetAnimationPlayRate();
	if (InAttackType == SkillType && CoolDownAnimation && PlayRate > 0.f)
	{
		RemainTime = PlayerAttackComponent->GetMontageCoolTime(SkillType);
		// 소수 첫째까지
		if (CoolTimeTextBlock)
		{
			CoolTimeTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			CoolTimeTextBlock->SetText(FText::AsNumber(FMath::CeilToInt(RemainTime * 10.f) * 0.1f));
		}
		bIsCoolingDown = true;
		PlayAnimation(CoolDownAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, PlayRate);
	}
}
