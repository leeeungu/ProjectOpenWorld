#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PatternObj.generated.h"


UCLASS(Blueprintable, editinlinenew, Abstract)
class PROJECTOPENWORLD_API UPatternObj : public UObject
{
	GENERATED_BODY()
protected:
	bool bIsPatternActive{};
public:
	UPatternObj() = default;

	bool IsPatternActive() const { return bIsPatternActive; }
	virtual void InitializePattern(AActor* OwnerActor, AActor* TargetActor) PURE_VIRTUAL(UPatternObj::InitializePattern, );
	virtual void StartPattern() PURE_VIRTUAL(UPatternObj::StartPattern, );
	virtual void UpdatePattern(float DeltaTime) PURE_VIRTUAL(UPatternObj::UpdatePattern, );
	virtual void EndPattern() PURE_VIRTUAL(UPatternObj::EndPattern, );
	virtual void UpdateCondition() PURE_VIRTUAL(UPatternObj::UpdateCondition, );
};