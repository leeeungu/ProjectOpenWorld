#include "Pal/Component/PalMonsterInteractionComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
//#include "Engine/Engine.h"
#include "EngineUtils.h"

UPalMonsterInteractionComponent::UPalMonsterInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPalMonsterInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	MonsterInteractionInterfaceList.Reserve(20);
	FOnActorSpawned::FDelegate del{};
	//del.BindUObject(this, &UPalMonsterInteractionComponent::CommandActorSpawned);
	if (GetWorld())
		GetWorld()->AddOnActorSpawnedHandler(del);
	if (GetWorld()->PersistentLevel)
	{
		for (AActor* actor : GetWorld()->PersistentLevel->Actors)
		{
			if (!actor)
				continue;

			TScriptInterface<IMonsterInteractionInterface> monsterInteractionInterface = TScriptInterface<IMonsterInteractionInterface>(actor);
			if (!monsterInteractionInterface && !actor->Implements<UMonsterInteractionInterface>())
				continue;
			RegisterMonsterInteractionInterface(IMonsterInteractionInterface::Execute_GetInteractionID(actor), monsterInteractionInterface, actor);
		}
	}
}

void UPalMonsterInteractionComponent::RegisterMonsterInteractionInterface(uint8 InteractionID, TScriptInterface<IMonsterInteractionInterface> MonsterInteractionInterface, AActor* Interaction)
{
	TArray < TObjectPtr<AActor>>* TargetList = nullptr;
	while (!MonsterInteractionInterfaceList.IsValidIndex(InteractionID))
	{
		MonsterInteractionInterfaceList.Push(TArray<TObjectPtr<AActor>>());
		MonsterInteractionInterfaceList.Last().Reserve(20);
	}
	TargetList = &MonsterInteractionInterfaceList[InteractionID];
	if (TargetList->Find(Interaction) == INDEX_NONE)
	{
		MonsterInteractionInterfaceList[InteractionID].Add(Interaction);
	}
}

TArray< AActor*> UPalMonsterInteractionComponent::GetMonsterInteractionInterfaceListByID(uint8 InteractionID) const
{
	if (MonsterInteractionInterfaceList.IsValidIndex(InteractionID))
	{
		return MonsterInteractionInterfaceList[InteractionID];
	}
	return TArray< AActor*>();
}

void UPalMonsterInteractionComponent::InvokeInteractionEvent(uint8 InteractionID, ACharacter* TargetMonster, EMonsterInteractionEvent Type)
{
	if (MonsterInteractionInterfaceList.IsValidIndex(InteractionID))
	{
		for (AActor* InterfaceItem : MonsterInteractionInterfaceList[InteractionID])
		{
			if (InterfaceItem)
			{
				UE_LOG(LogTemp, Log, TEXT("InvokeInteractionEvent ID : %d , TargetMonster : %s"), InteractionID, *InterfaceItem->GetName());
				switch (Type)
				{
				case EMonsterInteractionEvent::Default:
					IMonsterInteractionInterface::Execute_OnInteractionEvent(InterfaceItem, TargetMonster);
					//mapActiveInteraction.Add(InterfaceItem, TargetMonster);
					break;
				case EMonsterInteractionEvent::InteractionStart:
					IMonsterInteractionInterface::Execute_OnInteractionStartEvent(InterfaceItem, TargetMonster);
					setActiveInteraction.Add({ InterfaceItem, TargetMonster });
					break;
				case EMonsterInteractionEvent::InteractionEnd:
				{
					IMonsterInteractionInterface::Execute_OnInteractionEndEvent(InterfaceItem, TargetMonster);
					INT32 index = setActiveInteraction.Find({ InterfaceItem ,TargetMonster });
					if (index != INDEX_NONE)
					{
						setActiveInteraction[index] = { nullptr, nullptr };
					}
				}
					break;
				default:
					break;
				}
			}
		}
		if (MonsterInteractionInterfaceList[InteractionID].IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("InvokeInteractionEvent No registered interfaces for InteractionID : %d"), InteractionID);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InvokeInteractionEvent Invalid InteractionID : %d"), InteractionID);
	}
}

void UPalMonsterInteractionComponent::EndActiveInteraction()
{
	int nSize = setActiveInteraction.Num();
	for (int i = nSize - 1; i >= 0; --i)
	{
		auto& ActiveActor = setActiveInteraction[i];
		if (ActiveActor.Key && ActiveActor.Value)
		{
			IMonsterInteractionInterface::Execute_OnInteractionEndEvent(ActiveActor.Key, ActiveActor.Value);
		}
	}
	setActiveInteraction.Empty(400);
}

//void UPalMonsterInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}

