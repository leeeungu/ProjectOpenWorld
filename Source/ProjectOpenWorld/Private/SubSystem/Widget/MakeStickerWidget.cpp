#include "SubSystem/Widget/MakeStickerWidget.h"
#include "Player/Character/BasePlayer.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SubSystem/GPT/GPTInstanceSubsystem.h"
#include "SubSystem/Actor/StickerDecalActor.h"

void UMakeStickerWidget::OnRequestComplete_Implementation(UVaRestRequestJSON* Request)
{
	bReceivedRequest = true;
	if (!GetOwningPlayer())
			return;
	UGameInstance* GameInstance = GetOwningPlayer()->GetGameInstance();
	UGPTInstanceSubsystem* GPTInstance = GameInstance->GetSubsystem<UGPTInstanceSubsystem>();
	if (GPTInstance)
	{
		UTexture2D* ResponseTexture = GPTInstance->GetResponseTexture(Request);
		if (ResponseTexture && StickerTarget.IsValid())
		{
			StickerTarget->SetStickerImage(ResponseTexture);
		}
	}
}

void UMakeStickerWidget::OnRequestFail_Implementation(UVaRestRequestJSON* Request)
{
	bReceivedRequest = true;
	UTexture2D* ResponseTexture{};
	if (ResponseTexture && StickerTarget.IsValid())
	{
		StickerTarget->SetStickerImage(ResponseTexture);
	}
}

bool UMakeStickerWidget::SetMainWidget()
{
	if (!GetOwningPlayer())
		return false;
	AddToViewport();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayer(), this, EMouseLockMode::DoNotLock, true, true);
	GetOwningPlayer()->SetShowMouseCursor(true);
	return true;
}

void UMakeStickerWidget::UnSetMainWidget()
{
	if (!GetOwningPlayer())
		return ;
	RemoveFromParent();
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer(), true);
	GetOwningPlayer()->SetShowMouseCursor(false);
}

void UMakeStickerWidget::SetStickerTarget(AStickerDecalActor* Target)
{
	StickerTarget = Target;
}

void UMakeStickerWidget::SendRequestToGPT(const FText& Prompt, ETextCommit::Type CommitMethod)
{
	if (!GetOwningPlayer())
		return;
	UGameInstance* GameInstance = GetOwningPlayer()->GetGameInstance();
	UGPTInstanceSubsystem* GPTInstance = GameInstance->GetSubsystem<UGPTInstanceSubsystem>();
	if (GPTInstance)
	{
		FGPTImageRequest RequestData{};
		RequestData.Text = Prompt.ToString();
		GPTInstance->SendGPTImageRequest(RequestData, this);
		CloseWidget();
	}
}

void UMakeStickerWidget::CloseWidget()
{
	ABasePlayer* Player = Cast<ABasePlayer>(GetOwningPlayerPawn());
	if (Player)
	{
		Player->RemoveFromViewPort(this);
	}
}
