#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalJobTypes.h"
#include "PalJobComponent.generated.h"


class UBlackboardComponent;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWorkEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPalWorkTargetChange, AActor*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPalJobAssigned, const FPalWorkCommand&, Job);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPalJobFinished, const FPalWorkCommand&, Job, bool, bSuccess);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UPalJobComponent : public UActorComponent
{
    GENERATED_BODY()
private:
    /** 대기 큐 — 우선순위 max-heap */
    UPROPERTY(VisibleAnywhere, Category = "Pal|Job")
    TArray<FPalWorkCommand> PendingQueue{};

    UPROPERTY(VisibleAnywhere, Category = "Pal|Job")
    FPalWorkCommand CurrentJob{};

    UPROPERTY(Transient)
    TWeakObjectPtr<UBlackboardComponent> CachedBlackboard{};

protected:
    UPROPERTY(EditAnywhere, Category = "Pal|Job", meta = (Bitmask, BitmaskEnum = "/Script/ProjectOpenWorld.EPalWorkCapability"))
    uint8 CapabilityMask = 0;

    UPROPERTY(EditAnywhere, Category = "Pal|Job", meta = (ClampMin = "1", ClampMax = "8"))
    int32 MaxQueueSize = 8;

    UPROPERTY(EditAnywhere, Category = "Pal|Job")
    bool bAllowInterruptByHigherPriority = true;

    UPROPERTY(EditAnywhere, Category = "Pal|Job")
    bool bWorking{};

public:
    UPalJobComponent();
    /** 외부에서 명령 푸시. 능력 미달이면 false. */
    UFUNCTION(BlueprintCallable, Category = "Pal|Job")
    bool TryPushJob(const FPalWorkCommand& Command);

    /** BT Task에서 호출. 완료 후 다음 작업 자동 Pull. */
    UFUNCTION(BlueprintCallable, Category = "Pal|Job")
    void WorkFinished(bool bSuccess);

    /** 강제 클리어 (현재 + 대기 큐 모두). 현재 작업은 cancelled로 OnJobFinished 발행. */
    UFUNCTION(BlueprintCallable, Category = "Pal|Job")
    void ClearQueueAll();

    UFUNCTION(BlueprintCallable, Category = "Pal|Job")
    void RegisterWorker();
    UFUNCTION(BlueprintCallable, Category = "Pal|Job")
    void UnRegisterWorker();

    void StartWorking() ;
    void StopWorking() ;
    void EndWorking(bool bSuccess) ;

    UFUNCTION(BlueprintCallable, Category = "Pal|Job")
    void ChangeTransportTarget();

    UFUNCTION(BlueprintPure, Category = "Pal|Job")
    bool HasCapabilityFor(EPalJobType JobType) const;

    UFUNCTION(BlueprintPure, Category = "Pal|Job")
    bool HasCurrentJob() const { return CurrentJob.JobType != EPalJobType::None; }

    UFUNCTION(BlueprintPure, Category = "Pal|Job")
    const FPalWorkCommand& GetCurrentJob() const { return CurrentJob; }
    UFUNCTION(BlueprintPure, Category = "Pal|Job")
	EPalJobType GetCurrentJobType() const { return CurrentJob.JobType; }
    UFUNCTION(BlueprintPure, Category = "Pal|Job")
	bool IsWorking() const { return bWorking; }

    UFUNCTION(BlueprintPure, Category = "Pal|Job")
    int32 GetPendingCount() const { return PendingQueue.Num(); }

    // ─── Events ────────────────────────────────────────────

    UPROPERTY(BlueprintAssignable, Category = "Pal|Job")
    FOnWorkEvent OnWorkStart{};
    UPROPERTY(BlueprintAssignable, Category = "Pal|Job")
    FOnWorkEvent OnWorkStop{};
    UPROPERTY(BlueprintAssignable, Category = "Pal|Job")
    FOnWorkEvent OnWorkEnd{};

    UPROPERTY(BlueprintAssignable, Category = "Pal|Job")
    FOnPalWorkTargetChange OnWorkTargetChange{};
    UPROPERTY(BlueprintAssignable, Category = "Pal|Job")
    FOnPalJobAssigned OnJobAssigned{};
    UPROPERTY(BlueprintAssignable, Category = "Pal|Job") 
    FOnPalJobFinished OnJobFinished{};

protected:
    virtual void BeginPlay() override;

    static bool ComparePriority(const FPalWorkCommand& A, const FPalWorkCommand& B);

    bool PullNext();
    bool IsCommandStillValid(const FPalWorkCommand& Command) const;
    void SyncToBlackboard();
    void ClearCurrent();
};