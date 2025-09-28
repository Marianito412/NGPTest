// Fill out your copyright notice in the Description page of Project Settings.


#include "PushNotify.h"

#include "NGPTestCharacter.h"

void UPushNotify::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (ANGPTestCharacter* Character = Cast<ANGPTestCharacter>(MeshComp->GetOwner()))
	{
		//Todo: Parametrize calculation of scale using a FloatCurve or a blueprint implementable function
		Character->ApplyPush(1.f);
	}
}

void UPushNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ANGPTestCharacter* Character = Cast<ANGPTestCharacter>(MeshComp->GetOwner()))
	{
		Character->CapMaxSpeed();
	}
}
