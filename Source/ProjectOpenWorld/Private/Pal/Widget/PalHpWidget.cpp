#include "Pal/Widget/PalHpWidget.h"
#include "GameBase/Component/StatComponent.h"

void UPalHpWidget::InitializeHPWidget(UStatComponent* InStatComponent, FName DisplayName, int32 Level)
{
	SetupInfo(DisplayName, Level);
	if(StatComponent.IsValid())
		UnbindStat();
	StatComponent = InStatComponent;
	if(StatComponent.IsValid())
		BindStat();
}

float UPalHpWidget::GetHpPercent() const
{
	return StatComponent.IsValid() ? static_cast<float>(StatComponent->GetStatPercent(Stat)) : 0.0f;
}