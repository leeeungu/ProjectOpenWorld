#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PalStoreInterface.generated.h"

struct FPalStoredData
{
    //FName       PalName{};      // 어떤 종/템플릿인지 (예: Pal_BluePlatypus)
    int32       Level{};
    float       Exp{};
    float       HpRate{};         // CurrentHP / MaxHP 비율

    // 스탯
    int32       Attack{};
    int32       Defense{};
    int32       Speed{};
    // ...

    // 장비/아이템
    //TArray<FItemData> EquippedItems;

    // 성격, AI 관련 태그
    //FGameplayTagContainer PersonalityTags;

    // 기타 필요한 커스텀 필드들...
};

UINTERFACE(MinimalAPI)
class UPalStoreInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IPalStoreInterface
{
	GENERATED_BODY()
public:
	virtual void BuildStoredData(FPalStoredData& OutData) const = 0;
	virtual void ApplyStoredData(const FPalStoredData& InData) = 0;
};
