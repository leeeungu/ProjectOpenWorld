#include "Pal/Character/BasePalMonster.h"
#include "Pal/Component/PalPatrolComponent.h"

ABasePalMonster::ABasePalMonster() : ABaseMonster()
{
	PalPatrolComponent = CreateDefaultSubobject<UPalPatrolComponent>(TEXT("PalPatrolComponent"));
}
