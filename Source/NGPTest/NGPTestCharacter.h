// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NGPTestCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

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

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Pushing")
	float IdleSpeedBleedRate = 10.f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Pushing")
	float SpeedBleedRate = IdleSpeedBleedRate;
	
	//Brake
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Braking")
	float BrakeSpeedBleedRate = 50.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Braking")
	float BrakeDeceleration = 700.f;
	
	//Push
	UPROPERTY(BlueprintReadOnly, Category="Pushing")
	FTimerHandle PushTimerHandle;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Pushing")
	UAnimMontage* PushMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Pushing")
	float PushFrequency = 1.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Pushing")
	float MaxPushSpeed = 1000.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Pushing")
	float PushStrength = 400.f;

	//Jump
	UPROPERTY(BlueprintReadOnly)
	bool IsJumpLoading = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Jump")
	UAnimMontage* JumpMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Jump")
	float MaxJumpHold = 2.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Jump")
	float MaxJumpZSpeed = 600.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Jump")
	float MinJumpZSpeed = 300.f;
	
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
	
public:
	/** Constructor */
	ANGPTestCharacter();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void JumpPress(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void JumpRelease(const FInputActionInstance& ValueInstance);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void PushPress(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void PushRelease(const FInputActionInstance& ValueInstance);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void BrakePress(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void BrakeRelease(const FInputActionValue& Value);
	
	void HandlePush();

	UFUNCTION(BlueprintCallable)
	void ApplyPush(float Scale);
	
	UFUNCTION(BlueprintCallable)
	void CapMaxSpeed();

	

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;
	
	void Move(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);
	
	float GetTurnRate();
	
};

