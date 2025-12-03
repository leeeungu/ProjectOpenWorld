#include "Creature/Character/BaseCreature.h"
#include "Navigation/PathFollowingComponent.h"
#include "Pal/Component/PalAllyCommandComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Controller/PalAIController.h"

void ABaseCreature::BeginPlay()
{
	ABaseCharacter::BeginPlay();
}

ABaseCreature::ABaseCreature() : ABaseCharacter{}
{
	AIControllerClass = APalAIController::StaticClass();
	ArchitectureMeshComponent = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("ArchitectureMesh"));
	if (ArchitectureMeshComponent)
	{
		ArchitectureMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("SWeapon_R"));
		ArchitectureMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
		ArchitectureMeshComponent->SetVisibility(false);
	}
	CommandComponent = CreateDefaultSubobject<UPalAllyCommandComponent>(TEXT("AllyCommand"));
	AttackComponent  = CreateDefaultSubobject<UPalAttackComponent>(TEXT("AttackComponent"));
}

void ABaseCreature::ReceiveCommand_Implementation(FPalCommand Command)
{
	if (CommandComponent)
	{
		CommandComponent->PushCommand(Command);
	}
}

//void ABaseCreature::ResetActionMode()
//{
//	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
//}
//
//void ABaseCreature::TransportActionMode()
//{
//	GetCharacterMovement()->MaxWalkSpeed = 50.0f;
//}
//void ABaseCreature::SetArchitectureVisibility(bool bValue)
//{
//	SetActionStarted(bValue);
//	if (ArchitectureMeshComponent)
//	{
//		ArchitectureMeshComponent->SetVisibility(bValue);
//	}
//}
//
//void ABaseCreature::SetTransfortStart(bool bValue)
//{
//	SetActionStarted(bValue);
//	if (bValue)
//	{
//		TransportActionMode();
//	}
//	else
//	{
//		ResetActionMode();
//	}
//}
//
//void ABaseCreature::SetMining(bool bValue)
//{
//	SetActionStarted(bValue);
//}

float ABaseCreature::GetDefaultSpeed()
{
	return 300.0f;
}

void ABaseCreature::SetActionStarted(bool bValue)
{
	bActionStarted = bValue;
}
//
//void ABaseCreature::SetAttackData(bool bValue, AActor* TargetActor, uint8 AttackSlot)
//{
//	SetActionStarted(bValue);
//	if (AttackComponent && TargetActor && 0 != AttackSlot)
//	{
//		FPalAttackData Data{};
//		Data.TargetActor = TargetActor;
//		Data.AttackSlot = AttackSlot;
//		AttackComponent->SetAttackData(Data);
//	}
//}
//
//void ABaseCreature::StartAttack(AActor* TargetActor, uint8 AttackSlot)
//{
//}
//
//void  ABaseCreature::EndAttack()
//{
//	if (AttackComponent)
//	{
//		AttackComponent->EndAttack();
//	}
//}

bool ABaseCreature::DamagedCharacter_Implementation(const TScriptInterface<IAttackInterface>& Other)
{
	return false;
}

float ABaseCreature::GetAttackValue_Implementation() const
{
	return Attack;
}

void ABaseCreature::SetAttackValue_Implementation(float NewValue)
{
	Attack = NewValue;
}

void ABaseCreature::RetAttackValue_Implementation()
{
	Attack = 1.0f;
}
