#include "Pal/Widget/PalBox/PalInfomation.h"
#include "Pal/Widget/PalBox/PalInventorySlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"
#include "Pal/Character/PalBaseCreature.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"
#include "Pal/Widget/Infomation/StatusBarWidget.h"

void UPalInfomation::NativeConstruct()
{
	Super::NativeConstruct();
	SetPalCreature(nullptr);
}

void UPalInfomation::SetPalCreature(APalBaseCreature* SelectedCreature)
{
	if (PalHealthBar && CurrentSelectedCreature.IsValid())
	{
		PalHealthBar->UnBindStatWidget(CurrentSelectedCreature->GetStatComponent(), EStatusType::HP);
	}
	CurrentSelectedCreature = SelectedCreature;
	InfoVerticalBox->SetVisibility(ESlateVisibility::Hidden);
	if (CurrentSelectedCreature.IsValid())
	{
		if(PalHealthBar)
			PalHealthBar->BindStatWidget(CurrentSelectedCreature->GetStatComponent(), EStatusType::HP);
		PalInventorySlot->SetPalCreature(CurrentSelectedCreature.Get());
		FText CharacterID = FText::FromName(CurrentSelectedCreature->GetPalName());
		//Script/Engine.StringTable'/Game/Pal/StringTable/ST_PalName.ST_PalName'
		
		CharacterID= FText::FromStringTable("/Game/Pal/StringTable/ST_PalName.ST_PalName", *CharacterID.ToString());

		PalNameText->SetText(CharacterID);
		InfoVerticalBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UPalInfomation::OnSlotSelected_Implementation(int32 Index, AActor* SelectedCreature)
{
	SetPalCreature(Cast< APalBaseCreature>(SelectedCreature));
}
