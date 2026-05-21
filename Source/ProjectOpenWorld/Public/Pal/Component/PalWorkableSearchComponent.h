#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PalWorkableSearchComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorkableChanged, AActor*, Workable);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalWorkableSearchComponent : public USphereComponent
{
	GENERATED_BODY()
public:	
	UPalWorkableSearchComponent();
	UPROPERTY(BlueprintCallable, Category = "Search")
	FOnWorkableChanged OnAddWorkables{};
	UPROPERTY(BlueprintCallable, Category = "Search")
	FOnWorkableChanged OnRemoveWorkables{};
	UPROPERTY(VisibleAnywhere, Category = "Search")
	TSet<AActor*> OutPut{};


	UFUNCTION(BlueprintCallable, Category = "Search")
	void PauseSearch();
	UFUNCTION(BlueprintCallable, Category = "Search")
	void StartSearch();

protected:
	virtual void BeginPlay() override;

	UFUNCTION() 
	void OnBeginSearch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() 
	void OnEndSearch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
