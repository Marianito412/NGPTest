// Copyright Epic Games, Inc. All Rights Reserved.

#include "NGPTestCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "NGPTest.h"
#include "Kismet/KismetSystemLibrary.h"

ANGPTestCharacter::ANGPTestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ANGPTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Braking
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ANGPTestCharacter::BrakePress);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ANGPTestCharacter::BrakeRelease);
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ANGPTestCharacter::JumpPress);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ANGPTestCharacter::JumpRelease);

		//Pushing
		EnhancedInputComponent->BindAction(PushAction, ETriggerEvent::Started, this, &ANGPTestCharacter::PushPress);
		EnhancedInputComponent->BindAction(PushAction, ETriggerEvent::Completed, this, &ANGPTestCharacter::PushRelease);
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANGPTestCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANGPTestCharacter::Look);
	}
	else
	{
		UE_LOG(LogNGPTest, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

//Braking
void ANGPTestCharacter::BrakePress(const FInputActionValue& Value)
{
	SpeedBleedRate = BrakeSpeedBleedRate;
	GetCharacterMovement()->BrakingDecelerationWalking += BrakeDeceleration;  
}

void ANGPTestCharacter::BrakeRelease(const FInputActionValue& Value)
{
	SpeedBleedRate = IdleSpeedBleedRate;
	GetCharacterMovement()->BrakingDecelerationWalking -= BrakeDeceleration;
}

//Jumping
void ANGPTestCharacter::JumpPress(const FInputActionValue& Value)
{
	IsJumpLoading = true;
}

void ANGPTestCharacter::JumpRelease(const FInputActionInstance& ValueInstance)
{
	float Time = ValueInstance.GetElapsedTime();
	GetCharacterMovement()->JumpZVelocity =
		MinJumpZSpeed
		+ FMath::Clamp(Time / MaxJumpHold, 0.f, 1.f)
		* (MaxJumpZSpeed - MinJumpZSpeed);
	Jump();
	IsJumpLoading = false;
}

//Pushing
void ANGPTestCharacter::HandlePush()
{
	float Speed = GetCharacterMovement()->Velocity.Length();
	GetMesh()->GetAnimInstance()->Montage_Play(PushMontage);
}

void ANGPTestCharacter::ApplyPush(float Scale)
{
	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, Scale);

	GetCharacterMovement()->MaxWalkSpeed = MaxPushSpeed;
}

void ANGPTestCharacter::CapMaxSpeed()
{
	float CurrentSpeed = GetCharacterMovement()->Velocity.Size();
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
}

void ANGPTestCharacter::PushPress(const FInputActionValue& Value)
{
	GetWorld()->GetTimerManager().SetTimer(PushTimerHandle, this, &ANGPTestCharacter::HandlePush, PushFrequency, true, 0.f);
}

void ANGPTestCharacter::PushRelease(const FInputActionInstance& ValueInstance)
{
	GetWorld()->GetTimerManager().ClearTimer(PushTimerHandle);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.05f);
}

//Movement (Leaning)
void ANGPTestCharacter::Move(const FInputActionValue& Value)
{

	if (!GetController())
	{
		UE_LOG(LogNGPTest, Warning, TEXT("NGPTestCharacter::Move: Failed to find controller"));
		return;
	}

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	// find out which way is forward
	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	if (MovementVector.X != 0.f)
	{
		bool IsRight = MovementVector.X>0.f;
		float TurnRate = GetTurnRate();
			
		AddControllerYawInput(IsRight ? TurnRate: -TurnRate);
		AddMovementInput(ForwardDirection, 1.f);
	}
}

//Looking (Camera movement)
void ANGPTestCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (!GetController())
	{
		UE_LOG(LogNGPTest, Warning, TEXT("NGPTestCharacter::Move: Failed to find controller"));
		return;
	}

	CameraBoom->AddRelativeRotation(FRotator(LookAxisVector.Y, LookAxisVector.X, 0.f));
}


float ANGPTestCharacter::GetTurnRate()
{
	if (GetCharacterMovement()->IsFalling()) return 0.8;
	
	float Speed = GetCharacterMovement()->Velocity.Length();
	float SpeedPercentage = Speed/GetCharacterMovement()->MaxWalkSpeed;
	return FMath::Clamp(0.5 + 0.5*(1-SpeedPercentage), 0.f, 1.f);
}

void ANGPTestCharacter::Tick(float DeltaSeconds)
{
	float MaxSpeed = GetCharacterMovement()->MaxWalkSpeed; 
	if (MaxSpeed<=0) return Super::Tick(DeltaSeconds);
	
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed - SpeedBleedRate*DeltaSeconds;
	
	Super::Tick(DeltaSeconds);
}