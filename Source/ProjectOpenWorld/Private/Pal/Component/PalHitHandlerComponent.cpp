#include "Pal/Component/PalHitHandlerComponent.h"
#include "Components/ShapeComponent.h"	

#if WITH_EDITOR
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Misc/DataValidation.h"   
#endif
DEFINE_LOG_CATEGORY_STATIC(LogHitHandlerComponent, Warning, All);

void UPalHitHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPalHitHandlerComponent::TakeDamage(const FPalDamagePayload& Payload)
{
	OnDamageTaken.Broadcast(Payload);
}

#if WITH_EDITOR
void UPalHitHandlerComponent::OnRegister()
{
	Super::OnRegister();

	// 에디터 전용. PIE/게임 월드에서는 검사하지 않는다.
	const UWorld* World = GetWorld();
	if (!World || World->IsGameWorld())
	{
		return;
	}

	FText Error{};
	if (!HasRequiredCollisionPrimitive(Error))
	{
		UE_LOG(LogHitHandlerComponent, Warning, TEXT("%s"), *Error.ToString());
	}
}

bool UPalHitHandlerComponent::HasRequiredCollisionPrimitive(FText& OutError) const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
		return true;

	TArray<UPrimitiveComponent*> Primitives;
	Owner->GetComponents<UPrimitiveComponent>(Primitives);

	for (const UPrimitiveComponent* Primitive : Primitives)
	{
		if (Primitive && Primitive->GetCollisionObjectType() == RequiredObjectType)
		{
			return true;
		}
	}

	// ObjectType의 사람이 읽을 이름(예: ECC_WorldDynamic). 커스텀 채널의 프로필 이름이 필요하면
	// UCollisionProfile::Get()->ReturnChannelNameFromContainerIndex(...) 로 교체하면 된다.
	const UEnum* ChannelEnum = StaticEnum<ECollisionChannel>();
	const FString ChannelName = ChannelEnum
		? ChannelEnum->GetNameStringByValue(RequiredObjectType.GetValue())
		: FString::FromInt(RequiredObjectType.GetValue());

	OutError = FText::Format(
		NSLOCTEXT("CollisionGuard", "MissingObjectType",
			"[{0}] Owner '{1}'에 ObjectType '{2}'(으)로 설정된 PrimitiveComponent가 없습니다."),
		FText::FromString(GetName()),
		FText::FromString(Owner->GetName()),	
		FText::FromString(ChannelName));

	return false;
}
#endif