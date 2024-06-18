// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/Weapon.h"


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
	EquippedWeapon = BlasterChanracter->GetEquippedWeapon();
	bIsCrouched = BlasterChanracter->bIsCrouched;
	bAiming = BlasterChanracter->IsAiming();
	TurningInPlace = BlasterChanracter->GetTurningInPlace();

	//Rotator在底层已经实现了复制
	//yawoffset 用于扫射  
	FRotator AimRotation = BlasterChanracter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterChanracter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;


	//倾斜和角色本身与其自身旋转之间的delta Air旋转有关
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterChanracter->GetActorRotation();
	//当前帧旋转到前一帧的差量
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	//插值
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	
	//瞄准偏移
	AO_Yaw = BlasterChanracter->GetAO_Yaw();
	AO_Pitch = BlasterChanracter->GetAO_Pitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterChanracter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		//将位置从世界空间转换为骨骼相对空间
		//将LeftHandTransform在世界的位置转换到hand_r在骨骼中的相对位置
		BlasterChanracter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (BlasterChanracter->IsLocallyControlled())
		{
			isLocallyCharacter = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World);
			RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation()+ (RightHandTransform.GetLocation()- BlasterChanracter->GetHitTarget()));

			//枪口绘制一条射线
			FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
			FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red);
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), BlasterChanracter->GetHitTarget(), FColor::Blue);
		}


	}


	//BlasterChanracter->HasAuthority() == true:服务器 // BlasterChanracter->IsLocallyControlled() == true 本地玩家 
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
