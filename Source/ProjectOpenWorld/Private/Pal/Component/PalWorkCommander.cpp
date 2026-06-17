#include "Pal/Component/PalWorkCommander.h"
#include "Pal/Interface/PalWorkable.h"
#include "Pal/Interface/PalWorkerInterface.h"
#include "Pal/Component/PalJobComponent.h"

void UPalWorkCommander::RegisterWorker(AActor* WorkerActor)
{
	if (TScriptInterface<IPalWorkerInterface> Worker = WorkerActor)
	{
		UPalJobComponent* Com = Worker->GetPalJobComponent();
		FWorkerData& WorkData = WorkerRegister.FindOrAdd(WorkerActor);
		if (!WorkData.WorkEndHandle.IsValid() && Com)
		{
			WorkData.WorkEndHandle = Com->OnWorkEnd.AddUObject(this, &UPalWorkCommander::OnCreatureWorkFinished, WorkerActor);
			WorkData.WorkEndHandle = Com->OnWorkStop.AddUObject(this, &UPalWorkCommander::OnCreatureWorkFinished, WorkerActor);
		}
		if (Com && !Com->IsWorking())
		{
			uint8 Capability = Com->GetWorkCapability();
			const EPalWorkCapability WorkCapability = static_cast<EPalWorkCapability>(Capability);
			for (int32 i = 0; i < JobTypeCount; ++i)
			{
				const EPalJobType JobType = static_cast<EPalJobType>(i);
				const EPalWorkCapability Bit = PalJobUtils::ToCapability(JobType);
				if (EnumHasAnyFlags(WorkCapability, Bit))
				{
					WorkersByJob[i].FindOrAdd(WorkerActor);
				}
			}
		}
		Work();
	}
}

void UPalWorkCommander::UnRegisterWorker(AActor* WorkerActor)
{
	if (TScriptInterface<IPalWorkerInterface> Worker = WorkerActor)
	{
		UPalJobComponent* Com = Worker->GetPalJobComponent();
		if (FWorkerData* pWorkData = WorkerRegister.Find(WorkerActor))
		{
			Com->OnWorkEnd.Remove(pWorkData->WorkEndHandle);
			if (!Com->IsWorking())
			{
				uint8 Capability = Com->GetWorkCapability();
				const EPalWorkCapability WorkCapability = static_cast<EPalWorkCapability>(Capability);
				for (int32 i = 0; i < JobTypeCount; ++i)
				{
					const EPalJobType JobType = static_cast<EPalJobType>(i);
					const EPalWorkCapability Bit = PalJobUtils::ToCapability(JobType);
					if (EnumHasAnyFlags(WorkCapability, Bit))
					{
						WorkersByJob[i].Remove(WorkerActor);
					}
				}
			}
			WorkerRegister.Remove(WorkerActor);
		}
	}
}

void UPalWorkCommander::RegisterWorkable(AActor* WorkableActor)
{
	if (TScriptInterface<IPalWorkable > Workable = WorkableActor)
	{
		WorkableRegister.FindOrAdd(WorkableActor);
		WorkablesByJob[(uint8)Workable->GetWorkJobType()].FindOrAdd(WorkableActor);
		Work();
	}
}

void UPalWorkCommander::UnRegisterWorkable(AActor* WorkableActor)
{
	if (TScriptInterface<IPalWorkable > Workable = WorkableActor)
	{
		WorkableRegister.Remove(WorkableActor);
		WorkablesByJob[(uint8)Workable->GetWorkJobType()].Remove(WorkableActor);
	}
}

void UPalWorkCommander::BeginPlay()
{
	Super::BeginPlay();
}

void UPalWorkCommander::OnCreatureWorkFinished(AActor* WorkerActor)
{
	if (TScriptInterface<IPalWorkerInterface> Worker = WorkerActor)
	{
		RegisterWorker(WorkerActor);
		Work();
	}
}

AActor* UPalWorkCommander::FindBestWorkableFor(EPalJobType JobType)  const
{
	if (WorkablesByJob[(uint8)JobType].IsEmpty())
		return nullptr;
	TArray<TWeakObjectPtr<AActor>> WorkerArray = WorkablesByJob[(uint8)JobType].Array();
	AActor* Workerctor{};
	for (TWeakObjectPtr<AActor> Worker: WorkerArray)
	{
		Workerctor = Worker.Get();
		IPalWorkable* IWorkable = Cast< IPalWorkable>(Workerctor);
		if (IWorkable && IWorkable->IsWorkable())
		{
			break;
		}
	}
	return Workerctor;
}

void UPalWorkCommander::RemoveWorkable(AActor* WorkableActor, EPalJobType JobType)
{
	WorkablesByJob[(uint8)JobType].Remove(WorkableActor);
	WorkableRegister.Remove(WorkableActor);
}

AActor* UPalWorkCommander::FindBestWorkerFor(EPalJobType JobType) const
{
	if (WorkersByJob[(uint8)JobType].IsEmpty())
		return nullptr;
	TArray<TWeakObjectPtr<AActor>> WorkableArray = WorkersByJob[(uint8)JobType].Array();
	AActor* WorkableActor{};
	for (TWeakObjectPtr<AActor> Workable: WorkableArray)
	{
		WorkableActor = Workable.Get();
		IPalWorkerInterface* IWorkable = Cast< IPalWorkerInterface>(WorkableActor);
		if(IWorkable && IWorkable->IsCanWork())
		{
			break;
		}
	}
	return WorkableActor;
}

void UPalWorkCommander::RemoveWorker(AActor* WorkerActor)
{
	for (int32 i = 0; i < JobTypeCount; ++i)
	{
		WorkersByJob[i].Remove(WorkerActor);
	}
}


void UPalWorkCommander::Work()
{
	for (int32 i = 1; i < JobTypeCount; ++i)
	{
		EPalJobType JobType = (EPalJobType)i;
		AActor* WorkableActor = FindBestWorkableFor(JobType);
		AActor* WorkerActor = FindBestWorkerFor(JobType);
		IPalWorkable* Workable = Cast< IPalWorkable>(WorkableActor);
		IPalWorkerInterface* Worker = Cast< IPalWorkerInterface>(WorkerActor);
		if (Workable && Worker)
		{
			UPalJobComponent* Com = Worker->GetPalJobComponent();
			if (Workable->IsWorkable())
			{
				if (Com && Com->TryPushJob(Workable->GetWorkCommand(GetOwner(), WorkableActor)))
				{
					//UE_LOG(LogTemp, Warning, TEXT("PalWorkCommander::OnCreatureWorkFinished %s"), *WorkerActor->GetName());
					RemoveWorker(WorkerActor);
					RemoveWorkable(WorkableActor, JobType);
				}
			}
		}
		else if (!Worker)
		{
			RemoveWorker(WorkerActor);
		}
		else if (!Workable)
		{
			RemoveWorkable(WorkableActor, JobType);
		}
	}
}

void UPalWorkCommander::OnBeginSearch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetOwner() == OtherActor)
		return;
	RegisterWorker(OtherActor);
	RegisterWorkable(OtherActor);
}

void UPalWorkCommander::OnEndSearch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetOwner() == OtherActor)
		return;
	UnRegisterWorker(OtherActor);
	UnRegisterWorkable(OtherActor);
}