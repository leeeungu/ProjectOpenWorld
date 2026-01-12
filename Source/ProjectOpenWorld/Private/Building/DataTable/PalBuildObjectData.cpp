// Fill out your copyright notice in the Description page of Project Settings.


#include "Building/DataTable/PalBuildObjectData.h"

FPalBuildObjectData::FPalBuildObjectData()
{
	Materials.Init(FBuildMaterialData{},GetMaxMaterialCount());
}
