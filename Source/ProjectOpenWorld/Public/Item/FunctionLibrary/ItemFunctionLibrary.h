#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Item/Object/BaseItem.h"
#include "Item/Object/BaseItemObject.h"
#include "ItemFunctionLibrary.generated.h"

class UObject;

UCLASS()
class PROJECTOPENWORLD_API UItemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Item") //, meta = (DeterminesOutputType = "ItemClass"))
	static UBaseItem* CreateBaseItem(
		FName ItemId,
		int32 ItemCount,
		UObject* Outer,
		TSubclassOf<UBaseItem> ItemClass = nullptr 
	);

	static UBaseItemObject* CreateBaseItemObject(
		UObject* Outer,
		FName ItemId,
		int32 ItemCount,
		TSubclassOf<UBaseItemObject> ItemObjectClass = UBaseItemObject::StaticClass(),
		TSubclassOf<UBaseItem> ItemClass = UBaseItem::StaticClass()
	);

	//static UBaseItem* CreateItemObject(FName ItemID, int32 ItemCount, UClass* ItemClass = UBaseItem::StaticClass());
};
