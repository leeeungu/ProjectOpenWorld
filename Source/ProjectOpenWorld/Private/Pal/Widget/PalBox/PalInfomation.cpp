#include "Pal/Widget/PalBox/PalInfomation.h"
#include "Pal/Widget/PalBox/PalInventorySlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"
#include "Pal/Character/PalBaseCreature.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"

void UPalInfomation::NativeConstruct()
{
	Super::NativeConstruct();
	SetPalCreature(nullptr);
}

void UPalInfomation::SetPalCreature(APalBaseCreature* SelectedCreature)
{
	CurrentSelectedCreature = SelectedCreature;
	InfoVerticalBox->SetVisibility(ESlateVisibility::Hidden);
	if (CurrentSelectedCreature.IsValid())
	{
		PalInventorySlot->SetPalCreature(SelectedCreature);
		FText CharacterID = FText::FromName(SelectedCreature->GetPalName());
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
