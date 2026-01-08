// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameBase/MetaData/AMD_MontageChangeEvent.h"
#include "AMD_BossPatternStartEnd.generated.h"

UCLASS(Abstract)
class PROJECTOPENWORLD_API UAMD_BossPatternStartEnd : public UAMD_MontageChangeEvent
{
	GENERATED_BODY()
protected:
	int32 Index = INDEX_NONE;

	virtual void InitializeIndex() {}
public:
	virtual void StartEvent(UAnimInstance* Animinstance) override;
	virtual void EndEvent(UAnimInstance* Animinstance) override;
};

enum class EAnubisPatternType : uint8;

UCLASS()
class PROJECTOPENWORLD_API UAMD_BossPatternStartEnd_Anubis : public UAMD_BossPatternStartEnd
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnubisPattern")
	EAnubisPatternType PatternType{};

	virtual void InitializeIndex() override;
};
