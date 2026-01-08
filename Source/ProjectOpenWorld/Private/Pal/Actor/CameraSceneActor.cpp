#include "Pal/Actor/CameraSceneActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Character/BasePlayer.h"
#include "Components/SplineComponent.h"

ACameraSceneActor::ACameraSceneActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetActorTickEnabled(false);
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	SplineComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));	
	CameraComp->SetupAttachment(RootComponent);

	LookAtTargetComp = CreateDefaultSubobject<USceneComponent>(TEXT("LookAtTargetComp"));
	LookAtTargetComp->SetupAttachment(RootComponent);
}

void ACameraSceneActor::BeginPlay()
{
	Super::BeginPlay();
	SplineComp->Duration = SceneTime;
}

void ACameraSceneActor::OnInteractionEvent_Implementation(ACharacter* TargetMonster)
{
	IMonsterInteractionInterface::Execute_OnInteractionStartEvent(this, TargetMonster);
}

void ACameraSceneActor::OnInteractionStartEvent_Implementation(ACharacter* TargetMonster)
{
	if (bIsCameraMoving)
		return;
	CachedPlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!CachedPlayerCharacter)
		return;
	Monster = TargetMonster;
	ABasePlayer* Player = Cast< ABasePlayer>(CachedPlayerCharacter);
	APlayerController* Controller = Cast<APlayerController>(CachedPlayerCharacter->GetController());

	CustomTargetComp = LookAtTargetComp;
	if (TargetType == ETargetType::Monster && Monster)
	{
		CustomTargetComp = Monster->GetMesh();
	}
	else if (TargetType == ETargetType::Player && Player)
	{
		CustomTargetComp = Player->GetMesh();
	}
	if (Player && Controller)
	{
		PreviousViewTarget = Controller->GetViewTarget();

		// 이 액터로 뷰 전환 (블렌드)
		Controller->SetViewTargetWithBlend(this, FadeInTime, EViewTargetBlendFunction::VTBlend_EaseIn);

		// 카메라 컴포넌트 위치를 플레이어 위치로 맞춰 시작 (원하면 스플라인 시작점으로 설정)
		FVector playerLocation = Player->GetActorLocation();
		if (CameraComp)
		{
			CameraComp->SetWorldLocation(playerLocation);
		}

		// 이동 시작 설정
		SetActorTickEnabled(true);
		bIsCameraMoving = true;
		SplinePointIndex = 0;
	}
	currentTime = 0;
	SetActorTickEnabled(true);
	bIsCameraMoving = true;
	SplinePointIndex = 0;
}

void ACameraSceneActor::OnInteractionEndEvent_Implementation(ACharacter* TargetMonster)
{
	if (currentTime <= 0)
		return;
	APlayerController* Controller = Cast<APlayerController>(CachedPlayerCharacter->GetController());
	if (Controller)
	{
		currentTime = 0;
		// 이전 뷰타겟이 유효하면 복귀, 
		// 아니면 플레이어에게 복귀
		//Controller->EndViewTarget(Controller);
		AActor* TargetToRestore = PreviousViewTarget ? PreviousViewTarget : CachedPlayerCharacter;
		CachedPlayerCharacter->EnableInput(Controller);
		Controller->SetViewTargetWithBlend(TargetToRestore, FadeOutTime);
	}
}

void ACameraSceneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsCameraMoving || !CachedPlayerCharacter)
		return;

	if (currentTime >= SceneTime)
	{
		if (bEndWait)
		{
			bIsCameraMoving = false;
			SetActorTickEnabled(false);
			return;
		}

		if (bLoop)
		{
			currentTime = 0;
			return;
		}
	
		// 완료 처리
		bIsCameraMoving = false;
		SetActorTickEnabled(false);

		// 원래 뷰로 복귀
		if (CachedPlayerCharacter && SplineComp->IsClosedLoop())
		{
			IMonsterInteractionInterface::Execute_OnInteractionEndEvent(this, Monster);
		}
		PreviousViewTarget = nullptr;
		Monster = nullptr;
		return;
	}
	currentTime += DeltaTime;
	FVector targetLocation = SplineComp->GetLocationAtTime(currentTime, ESplineCoordinateSpace::World);
	CameraComp->SetWorldLocation(targetLocation);
	FVector Direction = (CustomTargetComp->GetComponentLocation() - targetLocation).GetSafeNormal();
	FRotator targetRotation = FRotator(Direction.Rotation().Pitch, Direction.Rotation().Yaw, 0.0f);
	CameraComp->SetWorldRotation(targetRotation);

	// 포인트에 충분히 근접하면 다음 포인트로
	//if (currentTime >= SceneTime)
	/*if (FVector::DistSquared(newLocation, targetLocation) <= FMath::Square(10.0f))
	{
		SplinePointIndex++;
	}*/
}

