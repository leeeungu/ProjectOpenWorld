#include "SubSystem/Actor/StickerDecalActor.h"
#include "Player/Character/BasePlayer.h"
#include "Blueprint/UserWidget.h"
#include "Components/DecalComponent.h"
#include "SubSystem/Widget/MakeStickerWidget.h"
#include "GameFramework/PlayerController.h"

void AStickerDecalActor::BeginPlay()
{
	Super::BeginPlay();
}

void AStickerDecalActor::OnInteractionStart_Implementation(ACharacter* pOther)
{
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

void AStickerDecalActor::OnInteractionEnd_Implementation(ACharacter* pOther)
{

}

void AStickerDecalActor::SetStickerImage(UTexture2D* pImage)
{
	if (Widget)
	{
		UDecalComponent* DecalCom = GetDecal();
		if (DecalCom)
		{
			UMaterialInstanceDynamic* DynamicMaterial = DecalCom->CreateDynamicMaterialInstance();
			if (DynamicMaterial)
			{
				DynamicMaterial->SetTextureParameterValue(FName("Image"), pImage);
			}
			DecalCom->SetDecalMaterial(DynamicMaterial);
		}
	}
}
