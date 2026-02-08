#include "Pal/Widget/PalBox/PalBoxWidget.h"
#include "Pal/Widget/PalBox/PalInfomation.h"
#include "Pal/Widget/PalBox/PalInventoryWidget.h"
#include "Pal/Component/PalCommanderComponent.h"
#include "Pal/Actor/PalBaseCamp.h"
#include "Blueprint/GameViewportSubsystem.h"

void UPalBoxWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (UGameViewportSubsystem* Subsystem = UGameViewportSubsystem::Get(GetWorld()))
	{
		Subsystem->OnWidgetAdded.AddUFunction(this, FName("OnPalBoxWidgetAdded"));
	}
}

void UPalBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPalBoxWidget::OnPalSelectedChanged(ABaseCreature* SelectedPal)
{
	CurrentSelectedPal = SelectedPal;
	if(PalInfoWidget )
		PalInfoWidget->SetPalCreature(CurrentSelectedPal.Get());
}

void UPalBoxWidget::GetSelectedPalArray(TArray<TObjectPtr<ABaseCreature>>& OutSelectedPals) const
{
	if (CommanderComponent)
	{
		CommanderComponent->GetPalArray(OutSelectedPals);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PalBoxWidget: CommanderComponent is null"));
	}
}

void UPalBoxWidget::SetOwnerActor(AActor* NewOwner)
{
	Super::SetOwnerActor(NewOwner);
	APalBaseCamp* PalBaseCamp = Cast<APalBaseCamp>(NewOwner);
	if (!PalBaseCamp)
	{
		UE_LOG(LogTemp, Warning, TEXT("PalBoxWidget: Owner is not PalBaseCamp"));
		return;
	}
	CommanderComponent = PalBaseCamp->GetPalCommanderComponent();
	if (!CommanderComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("PalBoxWidget: PalBaseCamp has no CommanderComponent"));
		return;
	}
	CommanderComponent->OnChangePalArray.AddUniqueDynamic(this, &UPalBoxWidget::OnPalBoxChangeEvent);
}

void UPalBoxWidget::OnPalBoxWidgetAdded(UWidget* Widget, ULocalPlayer* LocalPlayer)
{
	if (Widget == this)
	{
		CurrentSelectedPal = nullptr;
		OnPalBoxChangeEvent();
	}
}

void UPalBoxWidget::OnPalBoxChangeEvent()
{
	if (PalInfoWidget)
		PalInfoWidget->SetPalCreature(CurrentSelectedPal.Get());
	if (PalInventoryWidget)
		PalInventoryWidget->SetPalSlot();

}
