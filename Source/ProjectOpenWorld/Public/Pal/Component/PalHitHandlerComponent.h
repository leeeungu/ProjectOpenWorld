#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalDamageType.h"
#include "Pal/Data/PalCollisionFactory.h"
#include "PalHitHandlerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageTaken, const FPalDamagePayload&, Payload);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalHitHandlerComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

	TWeakObjectPtr<AActor> DamageInstigator{};
public:	
	UPROPERTY(BlueprintAssignable, Category = "Hitboxes")
	FOnDamageTaken OnDamageTaken{};

	void TakeDamage(const FPalDamagePayload& Payload);

	AActor* GetDamageInstigator() const { return DamageInstigator.Get(); }
public:
#if WITH_EDITOR
	// 컴파일/로드/배치/리컨스트럭션 시점 — 실제 인스턴스 기준 검사(BP에서 붙인 컴포넌트도 보임).
	virtual void OnRegister() override;
#endif
protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Collision Guard")
	TEnumAsByte<ECollisionChannel> RequiredObjectType =  PalDamage::GetDamageCollisionChannel();
#endif
#if WITH_EDITOR
	// Owner에 조건을 만족하는 PrimitiveComponent가 하나라도 있는지. 실패 시 OutError를 채운다.
	bool HasRequiredCollisionPrimitive(FText& OutError) const;
#endif
};
