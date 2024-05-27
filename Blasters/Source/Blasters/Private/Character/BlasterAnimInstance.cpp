// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	BlasterChanracter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (BlasterChanracter == nullptr)
	{
		BlasterChanracter = Cast<ABlasterCharacter>(TryGetPawnOwner());
		return;
	}
	//获取速度
	FVector Velocity = BlasterChanracter->GetVelocity();
	Velocity.Z = 0;
	Speed = Velocity.Size();

	bIsInAir = BlasterChanracter->GetCharacterMovement()->IsFalling();
	//GetCurrentAcceleration()：获取加速度
	bIsAccelerating = BlasterChanracter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = BlasterChanracter->IsWeaponEquipped();
	bIsCrouched = BlasterChanracter->bIsCrouched;
	bAiming = BlasterChanracter->IsAiming();
}
