#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "StatusTextBlock.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UStatusTextBlock : public UTextBlock
{
	GENERATED_BODY()
private:
	double Cur{};
	double Max = 1;

public:
	UFUNCTION()
	void OnCurrentStatusChanged(double PreCurrentStat, double CurrentStat);

	UFUNCTION()
	void OnMaxStatusChanged(double PreMaxStat, double MaxStat);

	void SetInitial(double CurrentStat, double MaxStat);
};
