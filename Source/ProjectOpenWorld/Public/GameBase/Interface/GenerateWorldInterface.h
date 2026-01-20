#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameBase/Struct/GenerateSectionData.h"
#include "GenerateWorldInterface.generated.h"

UINTERFACE()
class UGenerateWorldInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IGenerateWorldInterface
{
	GENERATED_BODY()
public:
	virtual void  NewGenerateWorldEvent(const FGenerateSectionData& SectionData){};
	virtual void  DelGenerateWorldEvent(const FGenerateSectionData& SectionData){};
};
