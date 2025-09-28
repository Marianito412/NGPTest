// Fill out your copyright notice in the Description page of Project Settings.


#include "CombosHUD.h"

float UCombosHUD::GetComboRemainingTime()
{
	if (!PointsComponent) return 0.f;
	
	float TimeLeft = GetWorld()->GetTimerManager().GetTimerRemaining(PointsComponent->ComboTimer);
	return TimeLeft/PointsComponent->ComboCooldown;
}

void UCombosHUD::UpdateMultiplier(float NewMultiplier)
{
	ComboMultiplierText->SetVisibility(NewMultiplier<=1.f ? ESlateVisibility::Hidden: ESlateVisibility::Visible);
	FString Multiplier = FString::Printf(TEXT("x%i"), FMath::FloorToInt(PointsComponent->ComboMultiplier));
	ComboMultiplierText->SetText(FText::FromString(Multiplier));
}

void UCombosHUD::ComboCommit(float ComboScore)
{
	TotalScoreText->SetText(FText::AsNumber(PointsComponent->TotalScore));
	CurrentComboText->SetText(FText::AsNumber(PointsComponent->CurrentComboScore));
	UpdateMultiplier(PointsComponent->ComboMultiplier);
}

void UCombosHUD::ComboUpdate(float ComboScore, float ComboMultiplier)
{
	//Single trick widget
	FString NewTrickText = FString::Printf(TEXT("%i x%i"), FMath::FloorToInt(ComboScore), FMath::FloorToInt(ComboMultiplier));
	TrickScoreText->SetText(FText::FromString(NewTrickText));
	PlayAnimationForward(TrickShowAnim, 1.f);

	//Aggregate widgets
	CurrentComboText->SetText(FText::AsNumber(PointsComponent->CurrentComboScore));
	UpdateMultiplier(ComboMultiplier);
}

void UCombosHUD::NativeConstruct()
{
	PointsComponent = UPointsComponent::GetPointsSystem(this);

	PointsComponent->OnComboCommited.AddDynamic(this, &UCombosHUD::ComboCommit);
	PointsComponent->OnComboUpdated.AddDynamic(this, &UCombosHUD::ComboUpdate);
	ComboCountDown->PercentDelegate.BindDynamic(this, &UCombosHUD::GetComboRemainingTime);
	
	Super::NativeConstruct();
}
