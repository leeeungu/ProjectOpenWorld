#include "Building/DataTable/PalBuildObjectData.h"

FPalBuildObjectData::FPalBuildObjectData()
{
	Materials.Init(FBuildMaterialData{},GetMaxMaterialCount());
}
