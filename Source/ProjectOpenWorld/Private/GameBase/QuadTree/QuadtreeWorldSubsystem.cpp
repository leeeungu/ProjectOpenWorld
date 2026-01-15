#include "GameBase/QuadTree/QuadtreeWorldSubsystem.h"
#include "GameFramework/Character.h"
#include "Landscape/Actor/WorldGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

TObjectPtr<UQuadtreeWorldSubsystem> UQuadtreeWorldSubsystem::Instance{};

void UQuadtreeWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Instance = this;
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(&InWorld, 0);
	if (PlayerCharacter)
	{
		PlayerCharacter->GetCapsuleComponent()->TransformUpdated.AddUObject(this, &UQuadtreeWorldSubsystem::OnUpdatePlayerLocation);
	}
	WorldGenerator = Cast<AWorldGenerator>(UGameplayStatics::GetActorOfClass(&InWorld, AWorldGenerator::StaticClass()));
}


void UQuadtreeWorldSubsystem::OnUpdatePlayerLocation(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	// 왜 쿼드 트리? 
	// WorldGenerator에서 의 방법 (section 방법)
	// 1 player 이동에 따른 section 변화 => 추가된 section의 actor 들에 event 혹인 제거된 section의 actor에게 event 
	//                                     여 이벤트도 tick에서 나누면 더 좋음 심지어
	//         그러면 계산이 section 계산 O(1) + section radius^2 만큼 탐색
	//         section radius^2 통해 나온 section 갯수 ^2 만큼 해서 제거랑 추가 분리
	//         제거 추가 된 actor 들에 event 전달 
	// 
	// 예를 들어보자 
	// 
	// 
	// => O(1)의 index 찾기 + section radius^2 만큼 탐색
	// 각 액터가 어느 section 인지 선 등록 해두면 O(1) + actor의 수 만큼 만에 특정 도 가능
	// player section 업데이트에 따라 일부 actor만 update 가능 
	// 
	// 쿼드는 각 객체가 어느 노드에 속하는 지 재귀적 계산을 해야함
	
	
	//WorldGenerator->GetSectionIndex(UpdatedComponent->GetComponentLocation());
	//int PlayerSectionX{};
	//int PlayerSectionY{};
}