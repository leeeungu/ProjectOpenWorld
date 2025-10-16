#include "GameBase/Widget/ConfirmationWidgetBase.h"
#include "Components/TextBlock.h"

void UConfirmationWidgetBase::SetAlertMessage(FText Message)
{
	if (AlertMessage && !Message.IsEmpty())
	{
		AlertMessage->SetText(Message);
	}
}
