#include "GameBase/Notify/AttackNotify.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Styling/CoreStyle.h"

void UAttackNotify::PostInitProperties()
{
	Super::PostInitProperties();

	// 에디터 전용: 노티 인스턴스가 생성될 때 디자이너에게 한 번만 경고 알림을 보여줍니다.
	if (GIsEditor && !HasAnyFlags(RF_ClassDefaultObject))
	{
		FNotificationInfo Info(FText::FromString(TEXT("AttackNotify is deprecated. Use UAnimNotify_Attack instead.")));
		Info.ExpireDuration = 8.0f;
		Info.bUseLargeFont = false;
		Info.Image = FCoreStyle::Get().GetBrush(TEXT("MessageLog.Warning"));

		//FSlateNotificationManager::Get().AddNotification(Info);
	}
}
