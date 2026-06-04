#include "Pal/Actor/Test/PalMontageTestActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameBase/Animation/BaseAnimInstance.h"

APalMontageTestActor::APalMontageTestActor() : Super{}
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	if (Mesh)
	{
		Mesh->SetupAttachment(RootComponent);
		Mesh->SetAnimationMode(EAnimationMode::AnimationCustomMode);
	}
	if (Sphere)
	{
		Sphere->SetupAttachment(RootComponent);
		Sphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
		Sphere->SetSphereRadius(200.f);
		Sphere->SetHiddenInGame(false);
	}
}

void APalMontageTestActor::BeginPlay()
{
	Super::BeginPlay();
	if (Sphere)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &APalMontageTestActor::OnBeginOverlap);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &APalMontageTestActor::OnEndOverlap);
	}
	if(Mesh)
	{
		AnimInstance = Cast< UBaseAnimInstance>(Mesh->GetAnimInstance());
		if (AnimInstance)
		{
			AnimInstance->ChangeMontageArray(TargetMontage);
			AnimInstance->OnMontageQueueEnd.AddDynamic(this, &APalMontageTestActor::PlayMontage);
		}
	}
}


void APalMontageTestActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Mesh && AnimInstance)
	{
		bIsPlayingMontage = false;
		AnimInstance->StopMontageQueue();
	}
	bIsPlayingMontage = true;
	PlayMontage();
}

void APalMontageTestActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsPlayingMontage = false;
}

void APalMontageTestActor::PlayMontage()
{
	if (!bIsPlayingMontage || !AnimInstance)
		return;
	AnimInstance->ChangeMontageArray(TargetMontage);
	AnimInstance->PlayMontageQueue();
}
