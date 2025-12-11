#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GPTResponseInterface.generated.h"

class UVaRestRequestJSON;

UINTERFACE(MinimalAPI)
class UGPTResponseInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IGPTResponseInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category = "GPTInterface")
	void OnRequestComplete(UVaRestRequestJSON* Request);

	UFUNCTION(BlueprintNativeEvent, Category = "GPTInterface")
	void OnRequestFail(UVaRestRequestJSON* Request);
};