#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseItemObject.generated.h"

class UBaseItem;

UCLASS(Abstract, Blueprintable)
class PROJECTOPENWORLD_API UBaseItemObject : public UObject, public IInterface_AssetUserData
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr< UBaseItem> ItemDataObject{};
public:
	void SetBaseItem(UBaseItem* NewItemDataObject) { ItemDataObject = NewItemDataObject; }
	UBaseItem* GetBaseItem() const { return ItemDataObject; }
	FORCEINLINE FName GetItemID() const;

	virtual void RegisterItemObject(AActor* TargetActor) PURE_VIRTUAL(UBaseItemObject::RegisterItemObject, );
	virtual void UnregisterItemObject(AActor* TargetActor) PURE_VIRTUAL(UBaseItemObject::UnregisterItemObject, );
};
