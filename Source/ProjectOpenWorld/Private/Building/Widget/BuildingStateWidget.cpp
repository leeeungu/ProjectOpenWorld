#include "Building/Widget/BuildingStateWidget.h"

void UBuildingStateWidget::SetBuildPercent(const float* BuildPercent)
{
	buildPercent = BuildPercent;
}

void UBuildingStateWidget::SetBuildTime(const float* BuildTime)
{
	buildTime = BuildTime;
}

FText UBuildingStateWidget::GetBuildTime() const
{
	if (!buildTime || !buildPercent)
		return FText{};

	int Time = (*buildTime) * (1-(*buildPercent));
	FString TimeString = FString(TEXT("00:00:00"));
	if (Time >= 3600)
	{
		int Hour = Time / 3600;
		TimeString[0] = Hour / 10 + '0';
		TimeString[1] = Hour % 10 + '0';
		if (Hour / 10 > 9)
		{
			TimeString[0] = '-';
			TimeString[1] = '-';
		}
	}
	if (Time >= 60)
	{
		int Min = (Time % 3600) / 60.0f;
		TimeString[3] = Min / 10 + '0';
		TimeString[4] = Min % 10 + '0';
	}
	if (Time > 0)
	{
		int Sec = (Time % 3600) % 60;
		TimeString[6] = Sec / 10 + '0';
		TimeString[7] = Sec % 10 + '0';
	}
	return FText::FromString(TimeString);
}