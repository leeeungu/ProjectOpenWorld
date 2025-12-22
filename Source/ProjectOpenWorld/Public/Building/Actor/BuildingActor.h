// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/BaseBuilding.h"
#include "Interaction/InteractionInterface.h"
#include "BuildingActor.generated.h"

class UBuildingStateWidget;

UCLASS()
class PROJECTOPENWORLD_API ABuildingActor : public ABaseBuilding, public IInteractionInterface
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TSoftObjectPtr<ACharacter> Player{};
	
public: // IInteractionInterface
	virtual void OnBeginDetected_Implementation(ACharacter* pOther) override;
	virtual void OnEndDetected_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
	virtual void OnInteraction_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionCanceled_Implementation(ACharacter* pOther) override;
};

// 여기에 각 building 마다 특수 widget과 기능들이 들어 있음
// 제작 building 제작 ui 또는 제작 시간 / 만들기 /  취소하기 같은 ui가 뜨고
// 채집 building은 아이템 체집 정도?
// 상자는 아이템 인벤토리 ui 정도 (참고로 아이템 어느곳에서든 참조 가능한거 주의해야함
// 너무 다양하다 확장에 열려있어야 하고 하나 컴포두고 상속으로 해야하지 않을까?
// 아주 하나의 공통점을 뽑으라고 하면 
// "모든 완성된 건축물은 각자의 UW를 가지고 있다는 점"?
// battle 건축물은 내구도를 보여줘야하고
// 인테리어 건축물도 내구도가 필요하며
// 상자 / 제작 / 채집 / 스파 등의 건축물도 각자의 UW가 필요하다.
// widgetcomponent 하나 상속받아 서 여러 ui 가지고 있다가 각자 필요한 거 상황에 맞게 띄우면 될것 같은데
 

// 제작 할때 시간에 따른 완성도는? 매번은 별로라고 생각이 들어서 시간에 따른 완성도가 있는 경우
// 1. 유저가 직접 볼때 2. 전체 완성도를 달성 했을 때 말고는 실시간으로 업데이트는 불필요 하지 않을까?
// 1.은 유저가 uw 열때 계산 + 실시간 포여주면 되고
// 2.는 유저가 작업을 등록하고 uw 닫을 때 몇시간 뒤 알려주는 방식은 어떨 까? 
//          펠 등이 작업에 영향을 끼치면 timer를 다시 설정하는 거지
// 

/*
* 건축 완료 시 => 이번주에 좀더 관찰 하자
OnBeginDetected 에서 widgetcomponent의 위젯 오픈

OnInteractionStart 하면 건축물의 wiget 오픈
				=> 다른 widget 으로 전환
				=> 기존 widget만 보여줌


OnInteractionEnd 하면 위젯 닫기 인데


OnEndDetected 에서 widgetcomponent의 닫기
*/




