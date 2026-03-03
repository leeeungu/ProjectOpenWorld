#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StringTableFunctionLibrary.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UStringTableFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "PalItem")
	static FName GetItemNameStringTable() { return FName("/Game/Global/StringTable/ST_PalItemName"); }
	UFUNCTION(BlueprintCallable, Category = "PalItem")
	static FText GetItemNameFromStringTable(const FString& InKey) ;
	UFUNCTION(BlueprintCallable, Category = "PalItem")
	static FName GetItemDescStringTable() { return FName("/Game/Global/StringTable/ST_PalItemDesc"); }
	UFUNCTION(BlueprintCallable, Category = "PalItem")
	static FText GetItemDescFromStringTable(const FString& InKey) ;

	UFUNCTION(BlueprintCallable, Category = "PalBuilding")
	static FName GetBuildingNameStringTable() { return FName("/Game/Global/StringTable/ST_BuildingName"); }
	UFUNCTION(BlueprintCallable, Category = "PalBuilding")
	static FText GetBuildingNameFromStringTable(const FString& InKey);
	UFUNCTION(BlueprintCallable, Category = "PalBuilding")
	static FName GetBuildingDescStringTable() { return FName("/Game/Global/StringTable/ST_BuildingDesc"); }
	UFUNCTION(BlueprintCallable, Category = "PalBuilding")
	static FText GetBuildingDescFromStringTable(const FString& InKey);

};
