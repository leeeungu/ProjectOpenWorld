#pragma once

#include "CoreMinimal.h"
#include "GameBase/Notify/AnimNotify_Attack.h"
#include "AttackNotify.generated.h"

// 컴파일 시 경고를 발생시켜 사용을 줄이도록 유도
UCLASS(meta=(DisplayName="AttackNotify (Deprecated)"))
class PROJECTOPENWORLD_API UAttackNotify : public UAnimNotify_Attack
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	// 에디터의 애니메이션 노티 트랙에서 보이는 이름을 변경하여 명확히 표시
	virtual FString GetNotifyName_Implementation() const override
	{
		return TEXT("AttackNotify (Deprecated) - Do NOT use. Use UAnimNotify_Attack instead.");
	}
#endif
	virtual void PostInitProperties() override;
	
};
