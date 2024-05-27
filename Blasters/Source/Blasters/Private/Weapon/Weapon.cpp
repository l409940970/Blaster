// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/BlasterCharacter.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	//是否进行网络复制
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	//WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);

	//阻挡所有通道
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//单独忽略pawn
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//默认关闭碰撞
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSpere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSpere->SetupAttachment(RootComponent);
	AreaSpere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSpere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	ShowPickupWidget(false);

	//HasAuthority（）:判断是否是服务端
	//在服务端开启碰撞
	if (HasAuthority())
	{
		AreaSpere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSpere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSpere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSpere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
	
}
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter )
	{
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}
void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickUpWidget)
	{
		PickUpWidget->SetVisibility(bShowWidget);
	}

}

