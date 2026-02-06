#pragma once

#include "CoreMinimal.h"
#include "GameBase/Component/StatComponent.h"
#include "StatComponent_Level.generated.h"

class UDataTable;

USTRUCT(BlueprintType)
struct FLevelStatData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	double MaxExp{};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelUp, int32, Level, bool, IsMaxLevel);

UCLASS()
class PROJECTOPENWORLD_API UStatComponent_Level : public UStatComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	TSoftObjectPtr<UDataTable> LevelStatDataTable{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", meta = (ClampMin = 1))
	int32 CurrentLevel{ 1 };

	TArray<FLevelStatData*> LevelStatDataArray{};
protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(BlueprintAssignable, Category = "Stat")
	FOnLevelUp OnLevelUp{};

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual double AddCurrentStat(double Value) override;

	UFUNCTION(BlueprintPure, Category = "Stat")
	int32 GetCurrentLevel() const { return CurrentLevel; }

	UFUNCTION(BlueprintPure, Category = "Stat")
	bool IsMaxLevel() const { return !LevelStatDataArray.IsValidIndex(CurrentLevel); }
};
