// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"
#include "Components/SphereComponent.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	//�Ƿ�������縴��
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	//WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);

	//�赲����ͨ��
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//��������pawn
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//Ĭ�Ϲر���ײ
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSpere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSpere->SetupAttachment(RootComponent);
	AreaSpere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSpere->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//HasAuthority����:�ж��Ƿ��Ƿ����
	//�ڷ���˿�����ײ
	if (HasAuthority())
	{
		AreaSpere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSpere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	
}
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

