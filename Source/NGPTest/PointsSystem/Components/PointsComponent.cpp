// Fill out your copyright notice in the Description page of Project Settings.


#include "PointsComponent.h"

#include "NGPTest.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "ObstaclesSystem/Components/ObstacleBox.h"


// Sets default values for this component's properties
UPointsComponent::UPointsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UPointsComponent::ResetCombo()
{
	//Add combo contribution to total score
	TotalScore += CurrentComboScore;

	//Reset combo
	CurrentComboScore = 0.f;
	ComboMultiplier = 1.f;
	
	OnComboCommited.Broadcast(CurrentComboScore);
}

void UPointsComponent::OnObstacleFinished(bool IsSuccess, UObstacleBox* Obstacle, float MinDistance, float MaxHeight)
{
	//If failed kill the combo
	if (!IsSuccess)
	{
		GetWorld()->GetTimerManager().ClearTimer(ComboTimer);
		ResetCombo();
		return;
	}
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Min: %f Max: %f"), MinDistance, MaxHeight));

	//Add score to reset
	float ObstacleScore = GetObstacleScore(MinDistance, MaxHeight) * ComboMultiplier;
	CurrentComboScore += ObstacleScore;
	OnComboUpdated.Broadcast(ObstacleScore, ComboMultiplier);
	ComboMultiplier++;

	//Reset Combo timer
	GetWorld()->GetTimerManager().ClearTimer(ComboTimer);
	GetWorld()->GetTimerManager().SetTimer(ComboTimer,this,  &UPointsComponent::ResetCombo, ComboCooldown);
}

float UPointsComponent::GetObstacleScore_Implementation(float MinDistance, float MaxHeight)
{
	//Arbitrary points calculation
	float MinDistanceScore = FMath::Clamp((100.f - MinDistance), 0.f, 1.f) * 300.f;
	float MaxHeightScore = MaxHeight * 1.3f;
	return MinDistanceScore + MaxHeightScore;
}

void UPointsComponent::AddObstacleToPointsSystem(UObject* WorldCtx, UObstacleBox* Obstacle)
{
	UPointsComponent* PointsComp = Cast<UPointsComponent>(
		UGameplayStatics::GetPlayerState(WorldCtx, 0)->GetComponentByClass(UPointsComponent::StaticClass()));
	if (!PointsComp)
	{
		UE_LOG(LogNGPTest, Warning,
		       TEXT("UPointsComponent::AddObstacleToPointsSystem: Failed to find points component"));
		return;
	}

	Obstacle->OnObstacleFinished.AddDynamic(PointsComp, &UPointsComponent::OnObstacleFinished);
}

UPointsComponent* UPointsComponent::GetPointsSystem(UObject* WorldCtx)
{
	return Cast<UPointsComponent>(UGameplayStatics::GetPlayerState(WorldCtx, 0)->GetComponentByClass(UPointsComponent::StaticClass()));
}
