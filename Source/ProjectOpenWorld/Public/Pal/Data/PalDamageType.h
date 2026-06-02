#pragma once

#include "CoreMinimal.h"
#include "PalDamageType.generated.h"

//UENUM(BlueprintType)
//enum class EBodyPart : uint8
//{
//    None,
//    Head,
//    Torso,
//    Arm,
//    Leg,
//    Weakpoint,
//};

//UENUM(BlueprintType)
//enum class EDamageCategory : uint8
//{
//    Physical,
//    Magical,
//    Static,    // 방어 무시
//    Fire,
//    Ice,
//    Lightning,
//};

USTRUCT(BlueprintType)
struct FPalDamagePayload
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, Category = "Damage")
    float BaseDamage = 0.f;

    //UPROPERTY(BlueprintReadWrite, Category = "Damage")
    //EDamageCategory Category = EDamageCategory::Physical;

    // 가해자
    UPROPERTY(VisibleAnywhere, Category = "Damage")
    TObjectPtr<AActor> Instigator = nullptr;   

    // 무기/투사체
    UPROPERTY(VisibleAnywhere, Category = "Damage")
    TObjectPtr<AActor> Causer = nullptr;       

    //UPROPERTY(BlueprintReadWrite, Category = "Damage")
    const FHitResult* HitResult{};

    //UPROPERTY(BlueprintReadWrite, Category = "Damage")
    //EBodyPart BodyPart = EBodyPart::None;

    //UPROPERTY(BlueprintReadWrite, Category = "Damage")
    //float Multiplier = 1.f;

    UPROPERTY(VisibleAnywhere, Category = "Damage")
    bool bIsCritical = false;
};

USTRUCT(BlueprintType)
struct FProjectileLaunchParams
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
    FVector ProjectileDirection = FVector::ForwardVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
    float ProjectileSpeed = 100.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
    float ProjectileLifeTime = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
    float ProjectileDistanceTraveled = 100.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float GravityScale = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float LifeSpan = 5.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
    bool bUseMeshRotation = true;
};


// dt 형식으로 하게 되니깐 mesh에 붙이는지 root에 붙이는 지 판별이 어려움
//USTRUCT(BlueprintType)
//struct FPalHitboxData : public FTableRowBase
//{
//    GENERATED_BODY()
//public:
//    // 디자이너가 액터에 미리 추가해둔 셰이프를 드래그로 연결
//    UPROPERTY(EditAnywhere, Category = "Hitbox")
//    TSubclassOf<UShapeComponent> Shape = nullptr;
//    UPROPERTY(EditAnywhere, Category = "Hitbox")
//	FName SocketName = NAME_None;
//
//    //UPROPERTY(EditAnywhere, Category = "Hitbox")
//    //EBodyPart BodyPart = EBodyPart::Torso;
//
//    //UPROPERTY(EditAnywhere, Category = "Hitbox", meta = (ClampMin = "0.0"))
//    //float Multiplier = 1.f;
//};