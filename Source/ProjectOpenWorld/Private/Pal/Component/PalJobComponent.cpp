#include "Pal/Component/PalJobComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Pal/FunctionLibrary/PalAIBlackboardKeysLibrary.h"
#include "Pal/Interface/PalWorkable.h"
#include "Pal/Interface/PalWorkerInterface.h"

namespace PalJobUtils
{
    inline EPalWorkCapability ToCapability(EPalJobType Type)
    {
        switch (Type)
        {
        case EPalJobType::Architecture: return EPalWorkCapability::Architecture;
        case EPalJobType::Mining:       return EPalWorkCapability::Mining;
        case EPalJobType::Lumbering:    return EPalWorkCapability::Lumbering;
        case EPalJobType::Transport:    return EPalWorkCapability::Transport;
        default:                        return (EPalWorkCapability)0;
        }
    }
}

UPalJobComponent::UPalJobComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UPalJobComponent::BeginPlay()
{
    Super::BeginPlay();

    if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
    {
        if (AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController()))
        {
            CachedBlackboard = AIC->GetBlackboardComponent();
        }
    }
}

// ─── Capability ──────────────────────────────────────────
bool UPalJobComponent::HasCapabilityFor(EPalJobType JobType) const
{
    const EPalWorkCapability Bit = PalJobUtils::ToCapability(JobType);
    return Bit != (EPalWorkCapability)0 && (CapabilityMask & (uint8)Bit) != 0;
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
        SyncToBlackboard();
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
            SyncToBlackboard();
            OnWorkTargetChange.Broadcast(CurrentJob.pTarget.Get());
            // todo => OnJobAssigned에 할당된 bb 변경을 OnWorkTargetChange로 변경해야함 + bb 변경도 여기가 아니라 다른곳으로 이전 해야할듯?
            OnJobAssigned.Broadcast(CurrentJob);
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
    bWorking = true;
    if (OnWorkStart.IsBound())
    {
        OnWorkStart.Broadcast();
    }
 }

void UPalJobComponent::StopWorking()
{
	bWorking = false;
    if (OnWorkStop.IsBound())
    {
        OnWorkStop.Broadcast();
    }
}

void UPalJobComponent::EndWorking(bool bSuccess)
{
    bWorking = false;
    if (OnWorkEnd.IsBound())
    {
        OnWorkEnd.Broadcast();
    }
    WorkFinished(bSuccess);
}

void UPalJobComponent::ChangeTransportTarget()
{
    if (CurrentJob.JobType == EPalJobType::Transport && CurrentJob.pInstigatorActor.IsValid())
    {
        CachedBlackboard->SetValueAsObject(UPalAIBlackboardKeysLibrary::GetBBJobTarget(), CurrentJob.pInstigatorActor.Get());
        OnWorkTargetChange.Broadcast(CurrentJob.pInstigatorActor.Get());
    }

}

// ─── Validation ──────────────────────────────────────────
bool UPalJobComponent::IsCommandStillValid(const FPalWorkCommand& Command) const
{
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
void UPalJobComponent::SyncToBlackboard()
{
    if (!CachedBlackboard.IsValid())
        return;
    CachedBlackboard->SetValueAsEnum(UPalAIBlackboardKeysLibrary::GetBBJobType(), (uint8)CurrentJob.JobType);
    CachedBlackboard->SetValueAsObject(UPalAIBlackboardKeysLibrary::GetBBJobTarget(), CurrentJob.pTarget.Get());
    CachedBlackboard->SetValueAsVector(UPalAIBlackboardKeysLibrary::GetBBJobLocation(), CurrentJob.TargetLocation);
}

void UPalJobComponent::ClearCurrent()
{
    CurrentJob.Reset();
    if (CachedBlackboard.IsValid())
    {
        CachedBlackboard->SetValueAsEnum(UPalAIBlackboardKeysLibrary::GetBBJobType(), (uint8)EPalJobType::None);
        CachedBlackboard->ClearValue(UPalAIBlackboardKeysLibrary::GetBBJobTarget());
    }
}

// ─── 우선순위 비교 (max-heap: 큰 Priority가 먼저) ────────
bool UPalJobComponent::ComparePriority(const FPalWorkCommand& A, const FPalWorkCommand& B)
{
    return A.Priority > B.Priority;
}