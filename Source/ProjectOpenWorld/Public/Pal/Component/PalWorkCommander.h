#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalJobTypes.h"
#include "PalWorkCommander.generated.h"

USTRUCT(BlueprintType)
struct FWorkerData
{
	GENERATED_USTRUCT_BODY()
public:
	FDelegateHandle WorkEndHandle{};
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UPalWorkCommander : public UActorComponent
{
	GENERATED_BODY()
private:
	static constexpr int32 JobTypeCount = static_cast<int32>(EPalJobType::Max_JobType);

	// JobType별 Workable 버킷 (각 Workable은 1개 버킷에만 존재)
	TSet<TWeakObjectPtr<AActor>> WorkablesByJob[JobTypeCount];

	// Capability 비트별 Worker 버킷 (한 Worker가 여러 버킷에 중복 등록 가능)
	TSet<TWeakObjectPtr<AActor>> WorkersByJob[JobTypeCount];

protected:
	UPROPERTY(VisibleAnywhere, Category = "Search")
	TSet<TWeakObjectPtr<AActor>> WorkableRegister{};

	UPROPERTY(VisibleAnywhere, Category = "Search")
	TMap<TWeakObjectPtr<AActor>, FWorkerData> WorkerRegister{};

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnCreatureWorkFinished(AActor* WorkerActor);

	/** Worker가 수행 가능한 JobType들을 순회하며 가장 적합한 Workable을 찾음 */
	AActor* FindBestWorkableFor(EPalJobType JobType) const;
	void RemoveWorkable(AActor* WorkableActor, EPalJobType JobType);

	/** 특정 JobType을 처리할 수 있는 가장 가까운 Worker를 찾음 */
	AActor* FindBestWorkerFor(EPalJobType JobType) const;
	void RemoveWorker(AActor* WorkerActor);

public:
	UFUNCTION(BlueprintCallable)
	void Work();

	void RegisterWorker(AActor* WorkerActor);
	void UnRegisterWorker(AActor* WorkerActor);

	void RegisterWorkable(AActor* WorkableActor);
	void UnRegisterWorkable(AActor* WorkableActor);

	UFUNCTION()
	void OnBeginSearch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndSearch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};