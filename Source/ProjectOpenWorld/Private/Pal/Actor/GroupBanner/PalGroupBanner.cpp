#include "Pal/Actor/GroupBanner/PalGroupBanner.h"
#include "SubSystem/GPT/GPTInstanceSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Player/Character/BasePlayer.h"
#include "Blueprint/UserWidget.h"
#include "SubSystem/Widget/MakeStickerWidget.h"
#include "GameFramework/PlayerController.h"
#include "Building/Component/PalBuildingStaticMeshComponent.h"
#include "VaRestRequestJSON.h"


void APalGroupBanner::SendFlagRequest(const FString& Prompt)
{
	UWorld* World = GetWorld();
	if (!World)
		return;
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
		return;
	UGPTInstanceSubsystem* GPTInstance = GameInstance->GetSubsystem<UGPTInstanceSubsystem>();
	if (!GPTInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TeamFlag] GPTInstanceSubsystem 없음"));
		return;
	}
	FGPTImageRequest RequestData{};
	RequestData.Text = Prompt;
	GPTInstance->SendGPTImageRequest(RequestData, this);
}

void APalGroupBanner::OnRequestComplete_Implementation(UVaRestRequestJSON* Request)
{
	UWorld* World = GetWorld();
	if (!World)
		return;
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
		return;
	UGPTInstanceSubsystem* GPTInstance = GameInstance->GetSubsystem<UGPTInstanceSubsystem>();
	if (!GPTInstance)
		return;
	UTexture2D* ResponseTexture = GPTInstance->GetResponseTexture(Request);
	if (ResponseTexture)
	{
		SetFlagImage(ResponseTexture);
	}
}

void APalGroupBanner::OnRequestFail_Implementation(UVaRestRequestJSON* Request)
{
	UE_LOG(LogTemp, Warning, TEXT("[TeamFlag] GPT 이미지 요청 실패 %s"), *Request->GetResponseContentAsString());
	SetFlagImage(nullptr);
}

void APalGroupBanner::OnInteractionStart_Implementation(ACharacter* pOther)
{
	if (!pOther)
		return;
	if (!GetBuildingProgress()->IsBuildingEnd())
	{
		GetBuildingProgress()->StartBuilding(pOther);
		return;
	}
	if (!Widget)
	{
		ABasePlayer* pPlayer = Cast<ABasePlayer>(pOther);
		if (pPlayer && WidgetClass)
		{
			APlayerController* PlayerController = Cast<APlayerController>(pPlayer->GetController());
			Widget = CreateWidget(PlayerController, WidgetClass);
			UMakeStickerWidget* MakeStickerWidget = Cast<UMakeStickerWidget>(Widget);
			if (MakeStickerWidget)
			{
				MakeStickerWidget->SetStickerTarget(this);
				pPlayer->AddToViewPort(Widget);
			}
		}
	}
	else
	{
		UMakeStickerWidget* MakeStickerWidget = Cast<UMakeStickerWidget>(Widget);
		if (MakeStickerWidget && MakeStickerWidget->IsReceivedRequest())
		{
			ABasePlayer* pPlayer = Cast<ABasePlayer>(pOther);
			if (pPlayer)
			{
				pPlayer->RemoveFromViewPort(Widget);
				Widget->RemoveFromRoot();
				Widget = nullptr;
			}
		}
		else if (MakeStickerWidget)
		{
			ABasePlayer* pPlayer = Cast<ABasePlayer>(pOther);
			if (pPlayer)
			{
				pPlayer->AddToViewPort(MakeStickerWidget);
			}
		}
	}
}

void APalGroupBanner::OnInteractionEnd_Implementation(ACharacter* pOther)
{
}

void APalGroupBanner::SetFlagImage(UTexture2D* pImage)
{
	if (!pImage)
		return;
	UStaticMeshComponent* MeshComp = GetBuildingMeshComponent();
	if (!MeshComp)
		return;
	UMaterialInstanceDynamic* DynMat = MeshComp->CreateDynamicMaterialInstance(0);
	if (DynMat)
	{
		DynMat->SetTextureParameterValue(FName("Image"), pImage);
	}
}
