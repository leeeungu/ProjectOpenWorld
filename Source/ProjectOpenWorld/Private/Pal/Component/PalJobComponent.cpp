#include "Pal/Component/PalJobComponent.h"
#include "Pal/Interface/PalWorkable.h"
#include "Pal/Interface/PalWorkerInterface.h"

UPalJobComponent::UPalJobComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UPalJobComponent::BeginPlay()
{
    Super::BeginPlay();
    //if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
    //{
    //    if (AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController()))
    //    {
    //        CachedBlackboard = AIC->GetBlackboardComponent();
    //    }
    //}
}

// ─── Capability ──────────────────────────────────────────
bool UPalJobComponent::HasCapabilityFor(EPalJobType JobType) const
{
    const EPalWorkCapability Bit = PalJobUtils::ToCapability(JobType);
    return Bit != (EPalWorkCapability)0 && (CapabilityMask & (uint8)Bit) != 0;
}

AActor* UPalJobComponent::GetTransportActor() const
{
    if (CurrentJob.JobType != EPalJobType::Transport)
        return nullptr;
    return CurrentJob.pTarget.Get();
}

AActor* UPalJobComponent::GetInsigatorActor() const
{
    if (CurrentJob.pInstigatorActor.IsValid())
        return CurrentJob.pInstigatorActor.Get();
    return nullptr;
}

// ─── Push ────────────────────────────────────────────────
bool UPalJobComponent::TryPushJob(const FPalWorkCommand& Command)
{
    if (Command.JobType == EPalJobType::None ||
        Command.JobType == EPalJobType::Max_JobType)
    {
		UE_LOG(LogTemp, Warning, TEXT("TryPushJob failed: Invalid job type %s"), *UEnum::GetValueAsString(Command.JobType));
        return false;
    }
    if (!HasCapabilityFor(Command.JobType))
    {
		UE_LOG(LogTemp, Warning, TEXT("TryPushJob failed: No capability for %s"), *UEnum::GetValueAsString(Command.JobType));
        return false;
    }

    // 인터럽트 — 현재보다 Priority 높으면 즉시 교체
    if (HasCurrentJob() && bAllowInterruptByHigherPriority &&
        Command.Priority > CurrentJob.Priority)
    {
        // 현재 작업은 큐로 되돌려서 끝나면 재개 시도
        PendingQueue.HeapPush(CurrentJob, &UPalJobComponent::ComparePriority);

        CurrentJob = Command;
        OnJobAssigned.Broadcast(CurrentJob);
        return true;
    }

    // 큐 오버플로 체크
    if (PendingQueue.Num() >= MaxQueueSize)
    {
        return false;
    }

    PendingQueue.HeapPush(Command, &UPalJobComponent::ComparePriority);

    // 현재 작업 없으면 바로 끌어옴
    if (!HasCurrentJob())
    {
        PullNext();
    }

    return true;
}

// ─── Pull (다음 유효 작업 꺼내기) ────────────────────────
bool UPalJobComponent::PullNext()
{
    while (PendingQueue.Num() > 0)
    {
        FPalWorkCommand Next{};
        PendingQueue.HeapPop(Next, &UPalJobComponent::ComparePriority);

        if (IsCommandStillValid(Next))
        {
            CurrentJob = Next;
            OnJobAssigned.Broadcast(CurrentJob);
            OnJobTargetChange.Broadcast(CurrentJob.pTarget.Get());
            if (CurrentJob.pTarget.IsValid())
            {
                OnJobLocationChange.Broadcast(CurrentJob.pTarget->GetActorLocation());
            }
            // todo => OnJobAssigned에 할당된 bb 변경을 OnWorkTargetChange로 변경해야함 + bb 변경도 여기가 아니라 다른곳으로 이전 해야할듯?
            return true;
        }
    }

    ClearCurrent();
    return false;
}

// ─── Finish (BT가 호출) ──────────────────────────────────
void UPalJobComponent::WorkFinished(bool bSuccess)
{
    if (!HasCurrentJob())
        return;

    const FPalWorkCommand Finished = CurrentJob;
    ClearCurrent();
    CurrentJob.Reset();

    OnJobFinished.Broadcast(Finished, bSuccess);
    PullNext();
}

// ─── Cancel all ──────────────────────────────────────────
void UPalJobComponent::ClearQueueAll()
{
    if (HasCurrentJob())
    {
        const FPalWorkCommand Cancelled = CurrentJob;
        ClearCurrent();
        OnJobFinished.Broadcast(Cancelled, /*bSuccess=*/false);
    }
    PendingQueue.Reset();
}

void UPalJobComponent::RegisterWorker()
{
    if (!IsCommandStillValid(CurrentJob))
    {
        ClearCurrent();
        PullNext();
        return;
    }
    IPalWorkable* Workable = Cast<IPalWorkable>(CurrentJob.pTarget.Get());
    if (Workable)
    {
        Workable->ResiterWorker(GetOwner());
    }
}

void UPalJobComponent::UnRegisterWorker()
{
    if (CurrentJob.pTarget.IsValid())
    {
        IPalWorkable* Workable = Cast<IPalWorkable>(CurrentJob.pTarget.Get());
        if (Workable)
        {
            Workable->UnregisterWorker(GetOwner());
        }
    }
}

void UPalJobComponent::StartWorking()
{
    if (!bWorking && HasCurrentJob())
    {
        bWorking = true;
        if (OnWorkStart.IsBound())
        {
            OnWorkStart.Broadcast();
        }
    }
 }

void UPalJobComponent::StopWorking()
{
    if (bWorking && HasCurrentJob())
    {
        bWorking = false;
        if (OnWorkStop.IsBound())
        {
            OnWorkStop.Broadcast();
        }
        WorkFinished(false);
    }
}

void UPalJobComponent::EndWorking(bool bSuccess)
{
    if (bWorking && HasCurrentJob())
    {
        bWorking = false;
        if (OnWorkEnd.IsBound())
        {
            OnWorkEnd.Broadcast();
        }
        WorkFinished(bSuccess);
    }
}

void UPalJobComponent::ChangeTransportTarget()
{
    if (CurrentJob.JobType == EPalJobType::Transport )
    {
        if (CurrentJob.pInstigatorActor.IsValid())
        {
            OnJobLocationChange.Broadcast(CurrentJob.pInstigatorActor->GetActorLocation());
            OnJobTargetChange.Broadcast(CurrentJob.pInstigatorActor.Get());
        }
        else
        {
            EndWorking(false);
        }
    }

}

// ─── Validation ──────────────────────────────────────────
bool UPalJobComponent::IsCommandStillValid(const FPalWorkCommand& Command) const
{
    if (Command.JobType == EPalJobType::Attack)
        return true;
    if (Command.pTarget.IsValid())
    {
        IPalWorkable* Workable = Cast<IPalWorkable>(Command.pTarget.Get());
        if (Workable)
        {
            return Workable->IsWorkable();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UPalJobComponent::IsCommandStillValid : not Interface %s"), *Command.pTarget->GetClass()->GetName());
        }
    }
    return false;
}

// ─── BB sync ─────────────────────────────────────────────

void UPalJobComponent::ClearCurrent()
{
    CurrentJob.Reset();
}

// ─── 우선순위 비교 (max-heap: 큰 Priority가 먼저) ────────
bool UPalJobComponent::ComparePriority(const FPalWorkCommand& A, const FPalWorkCommand& B)
{
    return A.Priority > B.Priority;
}