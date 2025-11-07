#include "PalSystem/BlueprintFunction/PalSphereBFL.h"

FVector UPalSphereBFL::GetCurveLocation(UWorld* WorldContext, FVector PreLocation, UCurveFloat* pHeightCurve, float Time, FVector Velocity)
{
	float Height = 0;
	if (pHeightCurve)
	{
		Height = pHeightCurve->GetFloatValue(Time);
	}
	return PreLocation + FVector(Velocity.X * Time, Velocity.Y * Time, Height);
}
