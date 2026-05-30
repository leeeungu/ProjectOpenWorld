#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "StatusTextBlock.generated.h"

class UStatComponent;
enum class EStatusType :uint8;

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

	void BindStatWidget(UStatComponent* StatCom, EStatusType StatusType);
	void UnBindStatWidget(UStatComponent* StatCom, EStatusType StatusType);
};
