
#include "GameBase/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
//
//void ABaseCharacter::TurnRotate()
//{
//	float Yaw{};
//	if (GetMesh() && GetMesh()->GetAnimInstance() && 
//		GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("DistanceCurve"), Yaw))
//	{
//	/*	float newOrient{};
//		GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("blendOrient1"), newOrient);
//		UE_LOG(LogTemp, Warning, TEXT("Yaw : %f"), Yaw);
//		GetCharacterMovement()->bOrientRotationToMovement = static_cast<bool>(newOrient);
//		FRotator NewRot = GetActorRotation();	
//		NewRot.Yaw = static_cast<double>(Yaw) - 90.0f;
//		GetMesh()->SetRelativeRotation(NewRot);
//		NewRot.Yaw = -NewRot.Yaw;*/
//		//SetActorRotation(NewRot);
//	}
//}
//
