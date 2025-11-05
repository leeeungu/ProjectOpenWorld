#include "Creature/Component/CreatureActionComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Creature/Interface/CreatureMessageInterface.h"

UCreatureActionComponent::UCreatureActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCreatureActionComponent::BeginPlay()
{
	Super::BeginPlay();	
	TScriptInterface<ICreatureMessageInterface> Interface = TScriptInterface<ICreatureMessageInterface>(GetOwner());
	if (!Interface || !Interface.GetObject() || !Interface.GetInterface())
	{
		// 오작동 및 메모리 오버 방지
		DestroyComponent();
	}
}