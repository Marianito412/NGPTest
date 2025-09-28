// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PointsComponent.generated.h"

class UObstacleBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComboCommitedSignature, float, ComboScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FComboUpdatedSignature, float, Score, float, Mutiplier);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NGPTEST_API UPointsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	float TotalScore = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float CurrentComboScore = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ComboCooldown = 5.f;
	
	UPROPERTY(BlueprintReadWrite)
	float ComboMultiplier = 1.f;

	UPROPERTY(BlueprintReadWrite)
	FTimerHandle ComboTimer;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FComboCommitedSignature OnComboCommited;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FComboUpdatedSignature OnComboUpdated;
public:
	// Sets default values for this component's properties
	UPointsComponent();

	UFUNCTION()
	void ResetCombo();
	
	UFUNCTION()
	void OnObstacleFinished(bool IsSuccess, UObstacleBox* Obstacle, float MinDistance, float MaxHeight);
	
	UFUNCTION(BlueprintNativeEvent)
	float GetObstacleScore(float MinDistance, float MaxHeight);
	
	UFUNCTION(BlueprintCallable)
	static void AddObstacleToPointsSystem(UObject* WorldCtx, UObstacleBox* Obstacle);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UPointsComponent* GetPointsSystem(UObject* WorldCtx);
	
};
