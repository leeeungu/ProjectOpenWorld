#include "Creature/Character/BaseCreature.h"
#include "Navigation/PathFollowingComponent.h"
#include "Pal/Component/PalAllyCommandComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Controller/PalAIController.h"

void ABaseCreature::BeginPlay()
{
	ABaseCharacter::BeginPlay();
	GetCharacterMovement()->SetUpdateNavAgentWithOwnersCollisions(true);
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
	Hp = 100.0f;
	Attack = 10.0f;
}

bool ABaseCreature::ReceiveCommand_Implementation(FPalCommand Command)
{
	if (CommandComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseCreature::ReceiveCommand_Implementation"));
		return CommandComponent->PushCommand(Command);
	}
	return false;
}

UPalCommandComponent* ABaseCreature::GetCommandComponent() const
{ 
	return CommandComponent;
}

float ABaseCreature::GetDefaultSpeed()
{
	return 300.0f;
}

void ABaseCreature::SetActionStarted(bool bValue)
{
	bActionStarted = bValue;
}

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
