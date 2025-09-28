// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "PointsSystem/Components/PointsComponent.h"
#include "CombosHUD.generated.h"

/**
 * 
 */
UCLASS()
class NGPTEST_API UCombosHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* TotalScoreText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* CurrentComboText;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* ComboMultiplierText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UProgressBar* ComboCountDown;

	//Single trick widgets

	UPROPERTY(EditAnywhere, Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* TrickShowAnim;

	//UPROPERTY(EditAnywhere, meta=(BindWidgetAnim))
	//UWidgetAnimation* TrickHideAnim;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* TrickText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* TrickScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPointsComponent* PointsComponent;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetComboRemainingTime();

	UFUNCTION()
	void UpdateMultiplier(float NewMultiplier);
	
	UFUNCTION()
	void ComboCommit(float ComboScore);

	UFUNCTION()
	void ComboUpdate(float ComboScore, float ComboMultiplier);
	
	virtual void NativeConstruct() override;
};
