#include "Player/Widget/PlayerPalSpawnerWidget.h"
#include "Components/Image.h"
#include "Pal/Character/PalBaseCreature.h"
#include "Player/Character/BasePlayer.h"
#include "Pal/Component/PalStorageComponent.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"
#include "Pal/Widget/PalHpWidget_MonsterDefault.h"

void UPlayerPalSpawnerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 스토리지 미지정이면 소유 폰에서 자동 탐색
	if (!Storage.IsValid())
	{
		if (ABasePlayer* Pawn = Cast< ABasePlayer>(GetOwningPlayerPawn()))
		{
			if (UPalStorageComponent* Comp = Pawn->GetPalStorageComponent())
			{
				InitWidget(Comp);
			}
		}
	}
	UpdateVisibleSlots();
}

void UPlayerPalSpawnerWidget::NativeDestruct()
{
	if (Storage.IsValid())
	{
		Storage->OnPalStoreChanged.RemoveAll(this);
		Storage = nullptr;
	}
	Super::NativeDestruct();
}

void UPlayerPalSpawnerWidget::InitWidget(UPalStorageComponent* InStorage)
{
	if (Storage.IsValid())
	{
		Storage->OnPalStoreChanged.RemoveAll(this);
	}

	Storage = InStorage;

	if (Storage.IsValid())
	{
		Storage->OnPalStoreChanged.AddUniqueDynamic(this, &UPlayerPalSpawnerWidget::OnPalStoreChanged);
	}
}

void UPlayerPalSpawnerWidget::RotateSelection(int32 Direction)
{
	if (!Storage.IsValid() || Direction == 0) 
		return;
	const int32 Size = Storage->GetInventorySize();
	if (Size <= 0) 
		return;

	SelectedIndex = ((SelectedIndex + Direction) % Size + Size) % Size;
	UpdateVisibleSlots();
	OnSelectionRotated(Direction);
}

void UPlayerPalSpawnerWidget::SetSlot(UImage* Img, AActor* Pal) const
{
	if (!Img) 
		return;
	UTexture2D* Icon = Pal ? UPalCharacterDataSubsystem::GetPalCharacterIconByName(ResolvePalName(Pal)) : nullptr;
	FSlateBrush BewBrush{};
	if (Icon)
	{
		BewBrush.DrawAs = ESlateBrushDrawType::Image;
		Img->SetBrush(BewBrush);
	}
	else
	{
		BewBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
		Img->SetBrush(BewBrush);
	}
	Img->SetBrushFromTexture(Icon);

}

void UPlayerPalSpawnerWidget::UpdateCenterInfo(AActor* Center) const
{
	if (!CenterHpWidget) 
		return;
	if (APalBaseCharacter* Character = Cast<APalBaseCharacter>(Center))
	{
		CenterHpWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		CenterHpWidget->InitializeHPWidget(Character->GetStatComponent(), Character->GetPalName(), 1);
	}
	else
	{
		CenterHpWidget->InitializeHPWidget(nullptr, NAME_None, 0);
	}
}

void UPlayerPalSpawnerWidget::OnPalStoreChanged(int ChangeIndex, AActor* ChangedPal)
{
	if (!Storage.IsValid()) return;
	const int32 Size = Storage->GetInventorySize();
	if (Size <= 0) 
		return;

	const int32 Sel = ((SelectedIndex % Size) + Size) % Size;
	const int32 LeftIdx = (Sel - 1 + Size) % Size;
	const int32 RightIdx = (Sel + 1) % Size;
	const int32 Changed = ChangeIndex;

	if (Changed != Sel && Changed != LeftIdx && Changed != RightIdx)
		return;

	AActor* Pal = Storage->GetStoredPal(Changed);
	if (Changed == Sel)
	{
		SetSlot(CenterSlotImage, Pal);
		UpdateCenterInfo(Pal);
	}
	if (Changed == LeftIdx) 
		SetSlot(LeftSlotImage, Pal);
	if (Changed == RightIdx)
		SetSlot(RightSlotImage, Pal);
}

FName UPlayerPalSpawnerWidget::ResolvePalName(AActor* Pal) const
{
	if (!Pal)
	{
		return NAME_None;
	}
	if (const APalBaseCharacter* V2 = Cast<APalBaseCharacter>(Pal))
	{
		return V2->GetPalName();
	}
	return NAME_None;
}

void UPlayerPalSpawnerWidget::UpdateVisibleSlots()
{
	if (!Storage.IsValid())
	{
		SetSlot(LeftSlotImage, nullptr);
		SetSlot(CenterSlotImage, nullptr);
		SetSlot(RightSlotImage, nullptr);
		UpdateCenterInfo(nullptr);
		return;
	}

	const int32 Size = Storage->GetInventorySize();
	if (Size <= 0)
	{
		UpdateCenterInfo(nullptr);
		return;
	}

	SelectedIndex = (SelectedIndex % Size + Size) % Size;
	AActor* Center = Storage->GetStoredPal(SelectedIndex);
	AActor* Left = Storage->GetStoredPal((SelectedIndex - 1 + Size) % Size);
	AActor* Right = Storage->GetStoredPal((SelectedIndex + 1) % Size);

	SetSlot(LeftSlotImage, Left);
	SetSlot(CenterSlotImage, Center);
	SetSlot(RightSlotImage, Right);
	UpdateCenterInfo(Center);
}