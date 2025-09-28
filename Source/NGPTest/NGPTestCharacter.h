// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NGPTestCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ANGPTestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	//UTimelineComponent* PushTimeline;
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	//UStaticMeshComponent* BoardMesh; 
	
protected:
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* BrakeAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* PushAction;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Pushing")
	float SpeedBleedRate = 10.f;

	//Push
	UPROPERTY(BlueprintReadOnly, Category="Pushing")
	FTimerHandle PushTimerHandle;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Pushing")
	UAnimMontage* PushMontage;
	
	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Pushing")
	//UCurveFloat* PushCurve;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Pushing")
	float PushFrequency = 1.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Pushing")
	float PushStrength = 400.f;

	//Jump
	UPROPERTY(BlueprintReadOnly)
	bool IsJumping = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Jump")
	UAnimMontage* JumpMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Jump")
	float MaxJumpHold = 2.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Jump")
	float MaxJumpZSpeed = 600.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Jump")
	float MinJumpZSpeed = 300.f;
	
	/** Constructor */
	ANGPTestCharacter();	

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	float GetTurnRate();
	void HandlePush();

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void JumpLoad(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void JumpRelease(const FInputActionInstance& ValueInstance);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void PushPress(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void PushRelease(const FInputActionInstance& ValueInstance);
	
public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

