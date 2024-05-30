// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	//��ȡ�ٶ�
	FVector Velocity = BlasterChanracter->GetVelocity();
	Velocity.Z = 0;
	Speed = Velocity.Size();

	bIsInAir = BlasterChanracter->GetCharacterMovement()->IsFalling();
	//GetCurrentAcceleration()����ȡ���ٶ�
	bIsAccelerating = BlasterChanracter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = BlasterChanracter->IsWeaponEquipped();
	bIsCrouched = BlasterChanracter->bIsCrouched;
	bAiming = BlasterChanracter->IsAiming();

	//Rotator�ڵײ��Ѿ�ʵ���˸���
	//yawoffset ����ɨ��  
	FRotator AimRotation = BlasterChanracter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterChanracter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;


	//��б�ͽ�ɫ��������������ת֮���delta Air��ת�й�
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterChanracter->GetActorRotation();
	//��ǰ֡��ת��ǰһ֡�Ĳ���
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	//��ֵ
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	
	//��׼ƫ��
	AO_Yaw = BlasterChanracter->GetAO_Yaw();
	AO_Pitch = BlasterChanracter->GetAO_Pitch();


	//BlasterChanracter->HasAuthority() == true:������ // BlasterChanracter->IsLocallyControlled() == true ������� 
	//if (!BlasterChanracter->HasAuthority() && !BlasterChanracter->IsLocallyControlled())
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		2.f,
	//		FColor::Yellow,
	//		FString::Printf(TEXT("AimRotation Yaw : %f  // MovementRotation Yaw : %f  "), AimRotation.Yaw, MovementRotation.Yaw)
	//	);
	//}
}
