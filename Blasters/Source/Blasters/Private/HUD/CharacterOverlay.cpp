// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CharacterOverlay.h"
#include "UMG/Public/Components/ProgressBar.h"
#include "UMG/Public/Components/TextBlock.h"

void UCharacterOverlay::SetHealth(float Health, float MaxHealth)
{
	const float HealthPercent = Health / MaxHealth;
	HealthBar->SetPercent(HealthPercent);

	FString TextHealth = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
	HealthText->SetText(FText::FromString(TextHealth));

}

void UCharacterOverlay::SetScore(float Score)
{
	FString TextScore = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
	ScoreAmount->SetText(FText::FromString(TextScore));
}

void UCharacterOverlay::SetDefeats(int32 Defeats)
{
	FString TextDefeats = FString::Printf(TEXT("%d"), Defeats);
	DefeateAmount->SetText(FText::FromString(TextDefeats));
}

void UCharacterOverlay::SetWeaponAmmo(int32 Ammo)
{
	FString TextAmmo = FString::Printf(TEXT("%d"), Ammo);
	WeaponAmmoAmount->SetText(FText::FromString(TextAmmo));
}

void UCharacterOverlay::SetCarriedAmmo(int32 Ammo)
{
	FString TextCarried = FString::Printf(TEXT("%d"), Ammo);
	CarriedAmmoAmount->SetText(FText::FromString(TextCarried));
}

void UCharacterOverlay::SetMatchCountdown(float CountdownTime)
{
	int32 Minutes = FMath::FloorToInt(CountdownTime / 60);
	int32 Seconds = CountdownTime - Minutes * 60;

	FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	MatchCountdownText->SetText(FText::FromString(CountdownText));
}

