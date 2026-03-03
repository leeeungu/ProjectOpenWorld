#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseGameInstance.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TSoftObjectPtr<UWorld> m_pTargetLevel{};
	int m_nCount{};

public:
	UFUNCTION(BlueprintCallable)
	static void LoadLevelStatic(UObject* pWorldContext);
	UFUNCTION(BlueprintCallable)
	static void OpenLevelStatic(UObject* pWorldContext, TSoftObjectPtr<UWorld> newLevel);

private:
	UFUNCTION()
	void OpenLoadedLevel();
	UFUNCTION()
	void ProgressFunction();
};