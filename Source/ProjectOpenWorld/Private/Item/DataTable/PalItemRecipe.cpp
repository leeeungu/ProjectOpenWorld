// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DataTable/PalItemRecipe.h"

FPalItemRecipe::FPalItemRecipe()
{
	Materials.Init(FRecipeMaterialData(), GetMaxMaterialCount());
}
