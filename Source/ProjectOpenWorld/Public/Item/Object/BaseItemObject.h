#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseItemObject.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTOPENWORLD_API UBaseItemObject : public UObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (ExposeOnSpawn = "TRUE"))
	FName ItemID = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (ExposeOnSpawn = "TRUE"))
	int itemCount = 1;
private:
	int RowIndex = -1;
	int ColIndex = -1;

public:

	virtual void RegisterItemObject(AActor* TargetActor) PURE_VIRTUAL(UBaseItemObject::RegisterItemObject, );
	virtual void UnregisterItemObject(AActor* TargetActor) PURE_VIRTUAL(UBaseItemObject::UnregisterItemObject, );
};
