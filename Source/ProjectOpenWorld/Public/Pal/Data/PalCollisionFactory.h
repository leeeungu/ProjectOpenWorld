#pragma once

#include "CoreMinimal.h"


enum ECollisionChannel : int;

namespace PalJob
{
	ECollisionChannel GetHarvestCollisionChannel();
}

namespace PalDamage
{
	ECollisionChannel GetDamageCollisionChannel();
}

//
//ENGINE_API const FName UCollisionProfile::NoCollision_ProfileName = FName(TEXT("NoCollision"));
//ENGINE_API const FName UCollisionProfile::BlockAll_ProfileName = FName(TEXT("BlockAll"));
//ENGINE_API const FName UCollisionProfile::PhysicsActor_ProfileName = FName(TEXT("PhysicsActor"));
//ENGINE_API const FName UCollisionProfile::BlockAllDynamic_ProfileName = FName(TEXT("BlockAllDynamic"));
//ENGINE_API const FName UCollisionProfile::Pawn_ProfileName = FName(TEXT("Pawn"));
//ENGINE_API const FName UCollisionProfile::Vehicle_ProfileName = FName(TEXT("Vehicle"));
//ENGINE_API const FName UCollisionProfile::DefaultProjectile_ProfileName = FName(TEXT("DefaultProjectile"));