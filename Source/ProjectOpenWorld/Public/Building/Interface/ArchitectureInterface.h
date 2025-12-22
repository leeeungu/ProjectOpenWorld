#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ArchitectureInterface.generated.h"

class ABaseBuilding;

UINTERFACE(MinimalAPI)
class UArchitectureInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IArchitectureInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Architecture")
	float GetArchitectSpeed() const ;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Architecture")
	void StartArchitect(ABaseBuilding* Building) ;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Architecture")
	void StopArchitect(ABaseBuilding* Building) ;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Architecture")
	void EndArchitect(ABaseBuilding* Building) ;
};
