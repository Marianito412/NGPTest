// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ObstacleBox.generated.h"

class UObstacleBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FObstacleResultDelegate, bool, IsSuccess, UObstacleBox*, Obstacle, float, MinDistance, float, MaxHeight);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NGPTEST_API UObstacleBox : public UBoxComponent
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDrawDebugTrace::Type> DebugVisibility;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool CanLand = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ObstacleEvaluationRate = 0.01;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintAssignable, BlueprintCallable)
	FObstacleResultDelegate OnObstacleFinished;

private:
	FTimerHandle ObstacleStatsTimerHandle;

	UPROPERTY()
	ACharacter* Character;
	
	//Obstacle Stats
	bool Landed = false;
	float MinDistance = 1000000.f;
	float MaxHeight = 0.f;
public:
	// Sets default values for this component's properties
	UObstacleBox();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void CollectStats();

	UFUNCTION()
	void OnObstacleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnObstacleEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                       int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnObstacleExited( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
