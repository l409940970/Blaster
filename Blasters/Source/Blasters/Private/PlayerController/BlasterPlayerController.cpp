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

void ABlasterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckTimeSync(DeltaTime);
	SetHUDTime();
}

float ABlasterPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
	{
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;

	}

}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
	}
}
void ABlasterPlayerController::SetHUDTime()
{
	uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());
	if (CountdownInt != SecondsLeft)
	{
		SetHUDMatchCountdown(MatchTime - GetServerTime());
	}
	CountdownInt = SecondsLeft;
}

void ABlasterPlayerController::Server_RequestServerTime_Implementation(float TimeOfClientRequest)
{
	//当前server的时间
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	Client_ReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerController::Client_ReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	//client 与server 之间通信的时间
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	//server的时间为当前加上通信时间
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();

}

bool ABlasterPlayerController::IsHUDVaild()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	return BlasterHUD && BlasterHUD->CharacterOverlay;
}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0;
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

void ABlasterPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	if (IsHUDVaild())
	{
		BlasterHUD->CharacterOverlay->SetMatchCountdown(CountdownTime);
	}
}


