#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SnapRule.generated.h"


// 400x400x400 기준 스냅 위치
UENUM(BlueprintType)
enum class ESnapAnchor : uint8
{
    NONE UMETA(DisplayName = "Custom"),

    // Up Layer
    flu UMETA(DisplayName = "Front Left Up"),
    fcu UMETA(DisplayName = "Front Center Up"),
    fru UMETA(DisplayName = "Front Right Up"),
    clu UMETA(DisplayName = "Center Left Up"),
    ccu UMETA(DisplayName = "Center Center Up"),
    cru UMETA(DisplayName = "Center Right Up"),
    blu UMETA(DisplayName = "Back Left Up"),
    bcu UMETA(DisplayName = "Back Center Up"),
    bru UMETA(DisplayName = "Back Right Up"),

    // Center Layer
    flc UMETA(DisplayName = "Front Left Center"),
    fcc UMETA(DisplayName = "Front Center Center"),
    frc UMETA(DisplayName = "Front Right Center"),
    clc UMETA(DisplayName = "Center Left Center"),
    ccc UMETA(DisplayName = "Center Center Center"),
    crc UMETA(DisplayName = "Center Right Center"),
    blc UMETA(DisplayName = "Back Left Center"),
    bcc UMETA(DisplayName = "Back Center Center"),
    brc UMETA(DisplayName = "Back Right Center"),

    // Down Layer
    fld UMETA(DisplayName = "Front Left Down"),
    fcd UMETA(DisplayName = "Front Center Down"),
    frd UMETA(DisplayName = "Front Right Down"),
    cld UMETA(DisplayName = "Center Left Down"),
    ccd UMETA(DisplayName = "Center Center Down"),
    crd UMETA(DisplayName = "Center Right Down"),
    bld UMETA(DisplayName = "Back Left Down"),
    bcd UMETA(DisplayName = "Back Center Down"),
    brd UMETA(DisplayName = "Back Right Down")
};

UENUM(BlueprintType)
enum class ESnapYaw: uint8
{
    d0     UMETA(DisplayName = "0"),
    d30     UMETA(DisplayName = "+30"),
    d60     UMETA(DisplayName = "+60"),
    d90     UMETA(DisplayName = "+90"),
    d120     UMETA(DisplayName = "+120"),
    d150     UMETA(DisplayName = "+150"),
    d180     UMETA(DisplayName = "+180"),
    d210     UMETA(DisplayName = "+210"),
    d240     UMETA(DisplayName = "+240"),
    d270     UMETA(DisplayName = "+270"),
    d300     UMETA(DisplayName = "+300"),
    d330     UMETA(DisplayName = "+330"),
};


USTRUCT(BlueprintType)
struct FSnapAnchorData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESnapAnchor ParentAnchor{};
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESnapAnchor  ChildAnchor{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESnapYaw ChildYaw{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector ChildOffset = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FSnapRule : public  FTableRowBase
{
    GENERATED_BODY()

    // 월드에 있는 메시 ptr
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr < UStaticMesh> ParentMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> ChildMesh = nullptr;


    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSnapAnchorData>  ArrayAnchors{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform ParentAnchorLocal{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform ChildAnchorLocal{};

    //// 나중을 위한 플래그/그룹 (지금은 안 써도 됨)
    //UPROPERTY(EditAnywhere, BlueprintReadWrite)
    //int32 Flags = 0;

   static FORCEINLINE FVector AnchorToOffset(ESnapAnchor  Anchor, const FVector& Extent)
    {
       if (ESnapAnchor::NONE == Anchor)
           return FVector::ZeroVector; // NONE or invalid
        auto GetAxis = [](TCHAR c, float HalfExtent)
            {
                switch (c)
                {
                case 'l': return -HalfExtent;
                case 'r': return +HalfExtent;
                case 'c': return 0.f;
                case 'u': return +HalfExtent;
                case 'd': return 0.f;
                case 'f': return +HalfExtent;
                case 'b': return -HalfExtent;
                default: return 0.f;
                }
            };

        FString Name = StaticEnum<ESnapAnchor >()->GetNameStringByValue(static_cast<int64>(Anchor)).ToLower();
        float X = GetAxis(Name[0], Extent.X * 0.5f); // l / c / r → X
        float Y = GetAxis(Name[1], Extent.Y * 0.5f); // d / c / u → Z
        float Z = GetAxis(Name[2], Extent.Z); // b / c / f → Y
        if (Name[2] == 'c')
        {
            Z = Extent.Z * 0.5f;
        }
        return FVector(X, Y, Z);
    }
    
   static FORCEINLINE float AnchorYaw(ESnapYaw  YawType)
   {
       return (uint8)(YawType) * 30.f;
   }

};