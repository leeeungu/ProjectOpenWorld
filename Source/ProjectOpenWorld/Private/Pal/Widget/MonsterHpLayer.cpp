#include "Pal/Widget/MonsterHpLayer.h"
#include "Pal/Widget/PalHpWidget.h"
#include "Pal/Widget/PalHpWidget_Boss.h"
#include "Pal/Data/PalSocketProfile.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Pal/Character/PalBaseMonster.h"
#include "Pal/Character/BossMonster.h"

void UMonsterHpLayer::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	PrewarmPool();
}

void UMonsterHpLayer::PrewarmPool()
{
	if (!MonsterBarClass)
		return;
	for (int32 i = Pool.Num(); i < InitialPoolSize; ++i)
	{
		if (UPalHpWidget* Bar = CreateBar())
			Pool.Add(Bar);
	}
}

void UMonsterHpLayer::RegisterMonster(AActor* Target)
{
	FHpBarEntry* Entry = &Entries.FindOrAdd(Target);
	if (!Target || !BarCanvas || !Entry)
		return;
	FName Name{};
	int32 Level{};
	UStatComponent* Stat{};
	if (APalBaseMonster* C = Cast<APalBaseMonster>(Target))
	{
		Name = C->GetPalName();
		Level = C->GetMonsterLevel();
		Stat = C->GetStatComponent();
		if (ABossMonster* pBoss = Cast< ABossMonster>(Target))
		{
			BossHpWidget->InitializeHPWidget(Stat, Name, Level);
			BossHpWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			Entry->Widget = AcquireBar();
			if (Entry->Widget)
			{
				if (UCanvasPanelSlot* CanvasSlot = BarCanvas->AddChildToCanvas(Entry->Widget))
				{
					CanvasSlot->SetAutoSize(true);
					CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
				}
				Entry->Widget->InitializeHPWidget(Stat, Name, Level);
				Entry->Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				return;
			}
		}
	}
	Entries.Remove(Target);
}

void UMonsterHpLayer::UnregisterMonster(AActor* Target)
{
	if (FHpBarEntry* Entry = Entries.Find(Target))
	{
		ReleaseBar(Entry->Widget);
		Entries.Remove(Target);
	}
	if (ABossMonster* pBoss = Cast< ABossMonster>(Target))
	{
		BossHpWidget->InitializeHPWidget(nullptr, {}, 0);
		BossHpWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMonsterHpLayer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
		return;

	const FName Socket = PalSocketProfile::GetPalRoot();
	const float Scale = UWidgetLayoutLibrary::GetViewportScale(PC);
	const FVector2D ViewSize = (Scale > 0.f) 	? UWidgetLayoutLibrary::GetViewportSize(PC) / Scale : FVector2D::ZeroVector;


	for (auto It = Entries.CreateIterator(); It; ++It)   // 가변 반복자
	{
		FHpBarEntry& E = It->Value;

		if (!It->Key.IsValid() || !E.Widget)
		{
			if (E.Widget) 
				ReleaseBar(E.Widget);
			It.RemoveCurrent();
			continue;
		}

		APalBaseMonster* pTarget = Cast< APalBaseMonster>(It->Key.Get());
		const FVector WorldLoc = pTarget->GetActorLocation() + FVector(0,0, pTarget->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 1.2);
		FVector2D Pos{};
		const bool bInFront = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, WorldLoc, Pos, false);
		const bool bVisible = bInFront  && Pos.X >= 0.f && Pos.Y >= 0.f && Pos.X <= ViewSize.X && Pos.Y <= ViewSize.Y;
		if (!bVisible)
		{
			E.Widget->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			E.Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(E.Widget->Slot))
				CanvasSlot->SetPosition(Pos);
		}
	}
}

UPalHpWidget* UMonsterHpLayer::CreateBar()
{
	if (!MonsterBarClass)
		return nullptr;
	return CreateWidget<UPalHpWidget>(this, MonsterBarClass);
}

UPalHpWidget* UMonsterHpLayer::AcquireBar()
{
	if (Pool.Num() > 0)
		return Pool.Pop();
	return CreateBar();
}

void UMonsterHpLayer::ReleaseBar(UPalHpWidget* Bar)
{
	if (!Bar)
		return;
	Bar->RemoveFromParent(); 
	Pool.Add(Bar);
}
