// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterComponent/CombatComponent.h"
#include "Weapon/Weapon.h"
#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"



UCombatComponent::UCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

	BaseWalkSpeed = 600.f;
	AimWaklSpeed = 400.f;

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

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);

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

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
}




