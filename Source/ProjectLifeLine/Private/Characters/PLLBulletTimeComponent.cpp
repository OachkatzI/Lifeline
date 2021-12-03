// Project LifeLine


#include "Characters/PLLBulletTimeComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


UPLLBulletTimeComponent::UPLLBulletTimeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ActiveCurve = nullptr;
}


void UPLLBulletTimeComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);

	SetBulletTimeReady();
}


void UPLLBulletTimeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ActiveCurve && GetWorld()->GetTimerManager().IsTimerActive(BulletTimerHandle))
	{
		GetOwner()->CustomTimeDilation = GetDilationFromCurve();
	}
}


void UPLLBulletTimeComponent::StartBulletTime()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(BulletTimerHandle) || GetWorld()->GetTimerManager().IsTimerActive(BulletCooldownTimerHandle))
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(BulletTimerHandle, this, &UPLLBulletTimeComponent::ActivateMainBulletTime, Settings->BulletTimeStartDuration);
	SetComponentTickEnabled(true);

	ActiveCurve = Settings->BulletTimeStartCurve;
	
	OnBulletTimeStart.Broadcast(); 
}


void UPLLBulletTimeComponent::ActivateMainBulletTime()
{
	GetWorld()->GetTimerManager().ClearTimer(BulletTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(BulletTimerHandle, this, &UPLLBulletTimeComponent::EndBulletTime, Settings->ActualBulletTimeDuration);

	ActiveCurve = nullptr;
}


void UPLLBulletTimeComponent::EndBulletTime()
{
	GetWorld()->GetTimerManager().ClearTimer(BulletTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(BulletTimerHandle, this, &UPLLBulletTimeComponent::StopAll, Settings->BulletTimeEndDuration);

	ActiveCurve = Settings->BulletTimeEndCurve;
	
	OnBulletTimeCooldownStart.Broadcast();
}


void UPLLBulletTimeComponent::StopAll()
{
	GetWorld()->GetTimerManager().ClearTimer(BulletTimerHandle);
	GetOwner()->CustomTimeDilation = 1.f;

	GetWorld()->GetTimerManager().SetTimer(BulletCooldownTimerHandle, this, &UPLLBulletTimeComponent::SetBulletTimeReady, Settings->BulletTimeCooldown);
	
	SetComponentTickEnabled(false);

	ActiveCurve = nullptr;
	
	OnBulletTimeEnd.Broadcast();
}


float UPLLBulletTimeComponent::GetDilationFromCurve() const
{
	float TimeInputMax = ActiveCurve == Settings->BulletTimeStartCurve ? Settings->BulletTimeStartDuration : TimeInputMax = Settings->BulletTimeEndDuration;
	
	const float ElapsedTimerSeconds = GetWorld()->GetTimerManager().GetTimerElapsed(BulletTimerHandle);
	const float CurveTime = UKismetMathLibrary::MapRangeClamped(ElapsedTimerSeconds, 0.f, TimeInputMax, 0.f, 1.f);
	const float DilationInput = ActiveCurve->GetFloatValue(CurveTime);

	return UKismetMathLibrary::MapRangeClamped(DilationInput, 0.f, 1.f, Settings->TotalTimeDilation, 1.f);
}


void UPLLBulletTimeComponent::SetBulletTimeReady()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	
	OnBulletTimeReady.Broadcast();
}


float UPLLBulletTimeComponent::GetCurrentCooldownElapsedRatio() const
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(BulletCooldownTimerHandle))
	{
		// Cooldown completed, ready to activate
		OnBulletTimeCooldownEnd.Broadcast();
		return 1.f;
	}
	
	return 1 - (GetWorld()->GetTimerManager().GetTimerRemaining(BulletCooldownTimerHandle) / Settings->BulletTimeCooldown);
}
