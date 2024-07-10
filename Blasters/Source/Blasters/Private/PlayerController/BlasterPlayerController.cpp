// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/BlasterPlayerController.h"
#include "HUD/BlasterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Character/BlasterCharacter.h"


void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());

}


void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
}
void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->SetHealth(Health, MaxHealth);
	}
	
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->ScoreAmount;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->SetScore(Score);
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{

	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->SetDefeats(Defeats);
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->SetWeaponAmmo(Ammo);
	}
}


