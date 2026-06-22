#include "Player/Widget/MainUI.h"
#include "Item/Widget/WeaponeWidget.h"
#include "Player/Widget/PlayerStatusProgress.h"
#include "Pal/Widget/MonsterHpLayer.h"
#include "Player/Widget/PlayerPalSpawnerWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"

void UMainUI::ChangeWeapone(FName NewWeaponeID, EWeapone NewWeaponeType)
{
	if (WBP_WeaponUI)
	{
		WBP_WeaponUI->ChangeWeapone(NewWeaponeID, NewWeaponeType);
	}
}

void UMainUI::SetPlayerStatWidget(UStatComponent* StatCom)
{
	if (PlayerHPBar)
	{
		PlayerHPBar->SetStatWidget(StatCom);
	}
}

void UMainUI::SetPalStorageComponent(UPalStorageComponent* InStorage)
{
	if (PlayerPalSpawnerWidget)
	{
		PlayerPalSpawnerWidget->InitWidget(InStorage);
	}
}

void UMainUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainUI::NativeDestruct()
{
	Super::NativeDestruct();
}

void UMainUI::RegisterMonster(AActor* Target)
{
	if(!MonsterHpLayer)
		return;
	MonsterHpLayer->RegisterMonster(Target);
}

void UMainUI::UnregisterMonster(AActor* Target)
{
	if (!MonsterHpLayer)
		return;
	MonsterHpLayer->UnregisterMonster(Target);
}

void UMainUI::StartCursor()
{
	if (CursorImage)
	{
		CursorImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UMainUI::EndCursor()
{
	if (CursorImage)
	{
		CursorImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainUI::RotateSelection(int32 Direction)
{
	if (PlayerPalSpawnerWidget)
	{
		PlayerPalSpawnerWidget->RotateSelection(Direction);
	}
}

int32 UMainUI::GetSelectedPal() const
{
	if (PlayerPalSpawnerWidget)
	{
		return PlayerPalSpawnerWidget->GetSelectedPal();
	}
	return -1;
}

void UMainUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerController* PC = GetOwningPlayer();
	if (!PC || !CursorImage)
		return;

	float MouseX = 0.f, MouseY = 0.f;
	if (!PC->GetMousePosition(MouseX, MouseY))
		return;

	const float DPI = UWidgetLayoutLibrary::GetViewportScale(this);
	CursorImage->SetRenderTranslation(FVector2D(MouseX / DPI, MouseY / DPI)); // Slot 타입 무관
}