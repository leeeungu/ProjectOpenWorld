#include "Item/DataTable/PalItemRecipe.h"

FPalItemRecipe::FPalItemRecipe()
{
	Materials.Init(FRecipeMaterialData(), GetMaxMaterialCount());
}
