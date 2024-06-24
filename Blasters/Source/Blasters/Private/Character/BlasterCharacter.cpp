// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "BlasterComponent/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/BlasterAnimInstance.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//将角色朝向设置为运动，这两步要一起设置
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//添加头部ui组件
	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(RootComponent);

	//战斗组件，设置组件可被复制
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	//设置是否可下蹲
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	//后转向速度
	GetCharacterMovement()->RotationRate = FRotator(0.f, 800.f,0.f);

	//忽略相机
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	//网络更新频率
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//设置需要同步的变量

	//仅同步给Actor的owner客户端
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);

	//同步给所有客户端
	//DOREPLIFETIME(ABlasterCharacter, OverlappingWeapon);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
}


void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed,this, &ABlasterCharacter::Jump);
	PlayerInputComponent->BindAxis("MoveForward", this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ThisClass::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ThisClass::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ThisClass::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ThisClass::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ThisClass::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ThisClass::FireButtonReleased);

}


void ABlasterCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}

	//还有一种是直接拿角色的朝向,但是获取的是根组件的
	//AddMovementInput(GetActorForwardVector(), Value);
}

void ABlasterCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}

	//AddMovementInput(GetActorRightVector(), Value);
}

void ABlasterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABlasterCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABlasterCharacter::EquipButtonPressed()
{
	//装配武器要在服务器执行
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			//客户端调用，服务器执行
			Server_EquipButtonPressed();
		}

	}

}

void ABlasterCharacter::Server_EquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABlasterCharacter::AimButtonPressed()
{
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABlasterCharacter::FireButtonPressed()
{
	if (IsWeaponEquipped())
	{
		Combat->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased()
{
	if (IsWeaponEquipped())
	{
		Combat->FireButtonPressed(false);
	}
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr)
	{
		return;
	}

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool isInAir = GetCharacterMovement()->IsFalling();


	//GetBaseAimRotation()获取没有经过任何调整比如瞄准误差、自动锁定、附着等的Pawn的瞄准旋转值Rotator
	if (Speed == 0.f && !isInAir)
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartAimRotator);
		AO_Yaw = DeltaAimRotation.Yaw;
		
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}

	if (Speed > 0.f || isInAir)
	{
		bUseControllerRotationYaw = true;
		StartAimRotator = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;  
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;
	//这一段是用于处理网络同步时角度不一致问题
	//因为这同步的时候，角度会被无符号的值。也就是在本地玩家中角度是-90度，但是同步到其他玩家那里时，角度会变成270度
	//因此这一段是本地玩家处理其他玩家同步过来的pitch值，使本地玩家看到的其他玩家动画 和 其他玩家自己看到的动画一致
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		//[270,360) to [-90,0);
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);

	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	UE_LOG(LogTemp, Display, TEXT("YAW:%f"), AO_Yaw);
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (TurningInPlace != ETurningInPlace :: ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw)<15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartAimRotator = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}



void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;

	//服务器执行
	//判断当前controller下的pawn是客户端的 还是 listen服务器操作的pawn or 单机运行的pawn
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);

}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr)
	{
		return nullptr;
	}
	return Combat->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat)
	{
		return Combat->HitTarget;
	}
	return FVector();
}

float ABlasterCharacter::GetFollowCameraFOV() const
{
	return FollowCamera->FieldOfView;
}

void ABlasterCharacter::SetFollowCameraFOV(float FOV) const
{
	FollowCamera->SetFieldOfView(FOV);
}

//客户端执行
void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (IsWeaponEquipped())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && FireWeaponMontage)
		{
			AnimInstance->Montage_Play(FireWeaponMontage);
			FName SectionName;
			SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
			//通过蒙太奇片段名称播放不同的动画
			AnimInstance->Montage_JumpToSection(SectionName);
		}
	}
}



