// Fill out your copyright notice in the Description page of Project Settings.


#include "PushNotify.h"

#include "NGPTestCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPushNotify::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (ANGPTestCharacter* Character = Cast<ANGPTestCharacter>(MeshComp->GetOwner()))
	{
		const FRotator Rotation = Character->GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		Character->AddMovementInput(ForwardDirection);

		Character->GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	}
}

void UPushNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ANGPTestCharacter* Character = Cast<ANGPTestCharacter>(MeshComp->GetOwner()))
	{
		float CurrentSpeed = Character->GetCharacterMovement()->Velocity.Size();
		Character->GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
	}
}
