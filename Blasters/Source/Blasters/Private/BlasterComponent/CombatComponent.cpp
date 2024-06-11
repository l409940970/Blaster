// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterComponent/CombatComponent.h"
#include "Weapon/Weapon.h"
#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


UCombatComponent::UCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWaklSpeed = 400.f;

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
	
}

void UCombatComponent::SetAiming(bool isAim)
{
	bAiming = isAim;
	//���ﲻ��Ҫ�ж��ڿͻ���ʱҪ���÷������ĺ���
	// ��Ϊ�����ı仯�������Ϸ���Ӱ�죬��������������֤Ҳ����
	//if (!Character->HasAuthority())
	//{
	//	Server_SetAiming(isAim);

	//}
	Server_SetAiming(isAim);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = isAim ? AimWaklSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::Server_SetAiming_Implementation(bool isAim)
{
	bAiming = isAim;
	//characterMovement���MaxWalkSpeed��Ҫͬ����������
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = isAim ? AimWaklSpeed : BaseWalkSpeed;
	}
}
void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		//ʹ�ÿ���������ת
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}



void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult HitResult;
	TraceUnderCrosshairs(HitResult);
}

//����������ڷ�����ִ��
//����ǿͻ���ʰȡ�����Ļ����Ǿ����ɿͻ��˵��ã�������ִ�У���������ĸı���תֻ���ڷ�������Ч���ͻ����ǿ�����Ч����
//�����ҪOnRep_EquippedWeapon������ͬ�����ͻ���
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr)
	{
		return;
	}

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);

	//ʹ�ÿ���������ת
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;

}

void UCombatComponent::Server_Fire_Implementation()
{
	//�������е����鲥
	MulticastFire();
}

void UCombatComponent::MulticastFire_Implementation()
{
	Character->PlayFireMontage(bAiming);
	EquippedWeapon->Fire(HitTarget);
} 

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		//�����߼��ڷ�����ִ�У�ͬ�����ͻ���
		Server_Fire();
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	//��ȡ��Ļ��С
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	//��Ļλ��ת���������λ��
	FVector CrosshairWorldPosition;
	//��Ļλ��ת����ķ���
	FVector CrosshairWorldDirection;

	//DeprojectScreenToWorld����Ļ����ת��Ϊ��������
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
		//����Ļ�����������λ��Ϊ���������߼��
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
		else
		{
			//�ڼ�⵽�ĵط�����һ������
			DrawDebugSphere(
				GetWorld(),
				TraceHitResult.ImpactPoint,
				12.f,
				12,
				FColor::Red
			);
		}
		//����λ��
		HitTarget = TraceHitResult.ImpactPoint;
	}
}
