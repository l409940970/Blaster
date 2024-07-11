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

bool ABlasterPlayerController::IsHUDVaild()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	return BlasterHUD && BlasterHUD->CharacterOverlay;
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

	if (IsHUDVaild())
	{
		BlasterHUD->CharacterOverlay->SetHealth(Health, MaxHealth);
	}
	
}

void ABlasterPlayerController::SetHUDScore(float Score)
{

	if (IsHUDVaild())
	{
		BlasterHUD->CharacterOverlay->SetScore(Score);
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{

	if (IsHUDVaild())
	{
		BlasterHUD->CharacterOverlay->SetDefeats(Defeats);
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{

	if (IsHUDVaild())
	{
		BlasterHUD->CharacterOverlay->SetWeaponAmmo(Ammo);
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{

	if (IsHUDVaild())
	{
		BlasterHUD->CharacterOverlay->SetCarriedAmmo(Ammo);
	}
}


