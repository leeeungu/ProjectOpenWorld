#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalDropItemDatabaseRow.generated.h"

USTRUCT(BlueprintType) 
struct FPalItemDropData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	FName ItemId{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	float Rate{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	int32 min{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	int32 Max{};
};

USTRUCT(BlueprintType)
struct FPalDropItemDatabaseRow :public FTableRowBase
{
	GENERATED_BODY()
public:
	FPalDropItemDatabaseRow();
	static int32 GetDropItemListSize() { return 10; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	FText CharacterID{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	int32 Level{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	TArray<FPalItemDropData> DropItemList{};
};
