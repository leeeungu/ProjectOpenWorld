#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PalSaveGameObject.generated.h"

UINTERFACE(MinimalAPI)
class UPalSaveGameObject : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IPalSaveGameObject
{
	GENERATED_BODY()

public:
	virtual void OnPreSave() {}
	virtual void OnLoaded() {}
};
