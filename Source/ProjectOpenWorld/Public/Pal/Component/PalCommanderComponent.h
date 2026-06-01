#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "Creature/Character/BaseCreature.h"
#include <set>
#include "PalCommanderComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangePalArray);

UCLASS(
	Abstract,                  // SpawnActor / NewObject 불가
	NotBlueprintable,          // BP 자식 클래스 못 만듦
	NotPlaceable,              // 레벨 드래그 배치 못함
	HideDropdown,              // 클래스 드롭다운에서 숨김
	meta = (
		DeprecationMessage = "Use UPalJobComponent instead. This class will be removed."
		)
)
class PROJECTOPENWORLD_API UPalCommanderComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	std::set<TObjectPtr<ABaseCreature>> pals{};
	
	TQueue< TWeakObjectPtr<AActor>> WorkQueue{};
	TQueue< TWeakObjectPtr<ABaseCreature>> TargetActorQueue{};

	TSet< TWeakObjectPtr<AActor>> RegisteredWorks{};

	UPROPERTY()
	TWeakObjectPtr<AActor> TargetWorkActor{};

	std::set<TObjectPtr<ABaseCreature>>::iterator ArrayIter{};
	int QueSize{};
public:	
	UPalCommanderComponent();
	//UPROPERTY(BlueprintAssignable, Category = "Commander")
	//FOnChangePalArray OnChangePalArray{};
protected:
	virtual void BeginPlay() override;

	void UpdateWorkQueue();

	void WorkEndEvent(AActor* WorkActor, FPalCommand WorkCommand);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Commander")
	void StopAllPal();
	void StorePal(AActor* NewPal);
	void RemovePal(AActor* targetPal);

	UFUNCTION(BlueprintCallable, Category = "Commander")
	void RegisterWork(AActor* WorkActor);
	UFUNCTION(BlueprintCallable, Category = "Commander")
	void UnRegisterWork(AActor* WorkActor);

	UFUNCTION(BlueprintCallable, Category = "Commander")
	bool WorkAllPal(const FPalCommand& Command);

	UFUNCTION(BlueprintCallable, Category = "Commander")
	bool WorkOnePal(const FPalCommand& Command);

	void GetPalArray(TArray<TObjectPtr<ABaseCreature>>& OutPalArray);
};
