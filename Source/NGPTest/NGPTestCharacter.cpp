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

	//Create PushTimeline
	PushTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PushTimeline"));
	
	
	//BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Board"));
	//BoardMesh->SetupAttachment(RootComponent);
	//GetMesh()->SetupAttachment(BoardMesh);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ANGPTestCharacter::BeginPlay()
{
	FOnTimelineFloat OnPushTimelineUpdate;
	OnPushTimelineUpdate.BindUFunction(this, FName("PushUpdate"));
	PushTimeline->AddInterpFloat(PushCurve, OnPushTimelineUpdate, FName("PushTrack"));
	
	Super::BeginPlay();
}

void ANGPTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ANGPTestCharacter::JumpLoad);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ANGPTestCharacter::JumpRelease);

		//Pushing
		EnhancedInputComponent->BindAction(PushAction, ETriggerEvent::Started, this, &ANGPTestCharacter::PushPress);
		EnhancedInputComponent->BindAction(PushAction, ETriggerEvent::Completed, this, &ANGPTestCharacter::PushRelease);
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANGPTestCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ANGPTestCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANGPTestCharacter::Look);
	}
	else
	{
		UE_LOG(LogNGPTest, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANGPTestCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ANGPTestCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ANGPTestCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// add movement
		//AddMovementInput(ForwardDirection, Forward);
		
		if (Right != 0)
		{
			bool IsRight = Right>0;
			float TurnRate = 1.f; //GetTurnRate();
			
			AddControllerYawInput(IsRight ? TurnRate: -TurnRate);
			AddMovementInput(ForwardDirection, 0.1f);
		}
		//AddMovementInput(RightDirection, Right);
	}
}

float ANGPTestCharacter::GetTurnRate()
{
	
	float Speed = GetCharacterMovement()->Velocity.Length();
	float SpeedPercentage = Speed/GetCharacterMovement()->MaxWalkSpeed;
	return FMath::Clamp(0.5 + 0.5*(1-SpeedPercentage), 0.f, 1.f);
	
}

void ANGPTestCharacter::HandlePush()
{

	PushTimeline->PlayFromStart();
	
	GetCharacterMovement()->MaxWalkSpeed = 800;
	UE_LOG(LogTemp, Warning, TEXT("Push to max speed: %f"), GetCharacterMovement()->MaxWalkSpeed);
	UKismetSystemLibrary::PrintString(this, "Push");
}

void ANGPTestCharacter::PushUpdate(float alpha)
{
	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection);
}

void ANGPTestCharacter::Tick(float DeltaSeconds)
{

	//Bleed extra speed
	if (GetCharacterMovement()->MaxWalkSpeed >= 600)
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed - SpeedBleedRate*DeltaSeconds;
		UKismetSystemLibrary::PrintString(
			this, FString::Printf(TEXT("%f"), GetCharacterMovement()->MaxWalkSpeed),
			true, true, FColor::Cyan, 0, FName("TickSpeed"));		
	}
}

void ANGPTestCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		//AddControllerYawInput(Yaw);
		//AddControllerPitchInput(Pitch);
		CameraBoom->AddRelativeRotation(FRotator(-1*Pitch, Yaw, 0.f));
	}
}

void ANGPTestCharacter::JumpLoad(const FInputActionValue& Value)
{
	IsJumping = true;
}

void ANGPTestCharacter::JumpRelease(const FInputActionInstance& ValueInstance)
{
	float Time = ValueInstance.GetElapsedTime();
	GetCharacterMovement()->JumpZVelocity =
		MinJumpZSpeed
		+ FMath::Clamp(Time / MaxJumpHold, 0.f, 1.f)
		* (MaxJumpZSpeed - MinJumpZSpeed);
	UKismetSystemLibrary::PrintString(
			this, FString::Printf(TEXT("%f"), GetCharacterMovement()->JumpZVelocity),
			true, true, FColor::Cyan, 3, FName("JumpSpeed"));
	Jump();
	IsJumping = false;
}

void ANGPTestCharacter::PushPress(const FInputActionValue& Value)
{
	GetWorld()->GetTimerManager().SetTimer(PushTimerHandle, this, &ANGPTestCharacter::HandlePush, PushFrequency, true);
}

void ANGPTestCharacter::PushRelease(const FInputActionInstance& ValueInstance)
{
	GetWorld()->GetTimerManager().ClearTimer(PushTimerHandle);
}

