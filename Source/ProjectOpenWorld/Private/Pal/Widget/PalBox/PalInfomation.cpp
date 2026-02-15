#include "Pal/Widget/PalBox/PalInfomation.h"
#include "Pal/Widget/PalBox/PalInventorySlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"

void UPalInfomation::NativeConstruct()
{
	Super::NativeConstruct();
	SetPalCreature(nullptr);
}

void UPalInfomation::SetPalCreature(ABaseCreature* SelectedCreature)
{
	CurrentSelectedCreature = SelectedCreature;
	InfoVerticalBox->SetVisibility(ESlateVisibility::Hidden);
	if (CurrentSelectedCreature.IsValid())
	{
		PalInventorySlot->SetPalCreature(SelectedCreature);
		FText CharacterID = FText::FromName(SelectedCreature->GetPalID());
		//Script/Engine.StringTable'/Game/Pal/StringTable/ST_PalName.ST_PalName'
		
		CharacterID= FText::FromStringTable("/Game/Pal/StringTable/ST_PalName.ST_PalName", *CharacterID.ToString());

		PalNameText->SetText(CharacterID);
		InfoVerticalBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}
