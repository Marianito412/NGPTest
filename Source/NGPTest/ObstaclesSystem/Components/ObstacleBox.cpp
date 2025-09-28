// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstacleBox.h"

#include "NGPTest.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PointsSystem/Components/PointsComponent.h"


// Sets default values for this component's properties
UObstacleBox::UObstacleBox()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}

// Called when the game starts
void UObstacleBox::BeginPlay()
{
	Super::BeginPlay();
	//Register self on points system
	UPointsComponent::AddObstacleToPointsSystem(this, this);

	//Add delegates for overlap events
	OnComponentBeginOverlap.AddDynamic(this, &UObstacleBox::OnObstacleEntered);
	OnComponentEndOverlap.AddDynamic(this, &UObstacleBox::OnObstacleExited);
	
	UStaticMeshComponent* Comp = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (Comp)
	{
		Comp->SetNotifyRigidBodyCollision(true);
		Comp->OnComponentHit.AddDynamic(this, &UObstacleBox::OnObstacleHit);
	}
	//GetOwner()->OnActorHit.AddDynamic(this, &UObstacleBox::OnObstacleHit);
	
}

void UObstacleBox::CollectStats()
{
	if (!Character) return;

	auto Channel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	FVector CharacterLocation = Character->GetActorLocation(); 
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore = {GetOwner()};
	
	UKismetSystemLibrary::SphereTraceMulti(
		this,
		CharacterLocation,
		CharacterLocation,
		300.f,
		Channel,
		true,
		ActorsToIgnore,
		DebugVisibility,
		HitResults,
		false);
	
	for (const auto& HitResult : HitResults)
	{
		//Set Minimun Distance
		float Distance = FVector::Dist(HitResult.ImpactPoint, CharacterLocation);
		if (Distance< MinDistance)
		{
			MinDistance = Distance;
		}

		//Set Maximum Height
		float Height = CharacterLocation.Z - GetOwner()->GetActorLocation().Z;
		if (Height> MaxHeight)
		{
			MaxHeight = Height;
		}
	}
}

void UObstacleBox::OnObstacleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Landed = true;
}

void UObstacleBox::OnObstacleEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((Character = Cast<ACharacter>(OtherActor)))
	{
		//Landed = false;
		MinDistance = 100000.f;
		MaxHeight = 0.f;
		
		GetWorld()->GetTimerManager().SetTimer(ObstacleStatsTimerHandle, this, &UObstacleBox::CollectStats, ObstacleEvaluationRate, true);	
	}
}

void UObstacleBox::OnObstacleExited(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Character) return;
	
	//Obstacle Success
	bool Success = CanLand || !Landed; 

	OnObstacleFinished.Broadcast(Success, this, MinDistance, MaxHeight);
	GetWorld()->GetTimerManager().ClearTimer(ObstacleStatsTimerHandle);
	Character = nullptr;
	Landed = false;
}
