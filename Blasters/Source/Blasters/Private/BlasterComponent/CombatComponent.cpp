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
#include "PlayerController/BlasterPlayerController.h"
#include "Interfaces/InteractWithCrosshairInterface.h"
#include "TimerManager.h"


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
	//只对owner复制
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		DefaultFOV = Character->GetFollowCameraFOV();
		CurFOV = DefaultFOV;
		//在服务器中
		if (Character->HasAuthority())
		{
			InitialzeCarriedAmmo();
		}
	}
	
}

void UCombatComponent::SetAiming(bool isAim)
{
	bAiming = isAim;
	//这里不需要判断在客户端时要调用服务器的函数
	// 因为动画的变化不会对游戏造成影响，不经过服务器验证也可以
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
	//characterMovement里的MaxWalkSpeed需要同步到服务器
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = isAim ? AimWaklSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{

		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}

		//使用控制器的旋转
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (Character && Character->IsLocallyControlled())
	{
		//检测准星命中点
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}

}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr || Character == nullptr)
	{
		return;
	}

	if (bAiming)
	{
		CurFOV = FMath::FInterpTo(CurFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurFOV = FMath::FInterpTo(CurFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	Character->SetFollowCameraFOV(CurFOV);
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr)
	{
		return;
	}
	Character->GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this, 
		&UCombatComponent::FireTimerFinishied, 
		EquippedWeapon->FireDelay
	);

}

void UCombatComponent::FireTimerFinishied()
{
	bCanFire = true;
	if (bFireButtonPressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr)
	{
		return false;
	}
	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::InitialzeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponTyps::EWT_AssaultRifle, StartingARAmmo);
}

//这个函数是在服务器执行
//如果是客户端拾取武器的话，那就是由客户端调用，服务器执行，所以下面的改变旋转只会在服务器生效，客户端是看不到效果的
//因此需要OnRep_EquippedWeapon函数，同步给客户端
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr)
	{
		return;
	}

	//丢弃之前的枪
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDAmmo();
	
	//武器的备弹
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	//使用控制器的旋转
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;

}

void UCombatComponent::Server_Fire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	//服务器中调用组播
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}

} 

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		Fire();
	}

}

void UCombatComponent::Fire()
{

	if (CanFire())
	{
		bCanFire = false;
		////开火前检测目标点
		//FHitResult HitResult;
		//TraceUnderCrosshairs(HitResult);

		CrosshairShootFactor = 1.f;
		//开火逻辑在服务器执行，同步到客户端
		Server_Fire(HitTarget);

		StartFireTimer();
	}

}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState != ECombatState::ECS_Reloading)
	{
		Server_Reload();
	}
}

void UCombatComponent::Server_Reload_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr)
	{
		return;
	}
	//更新武器备弹数
	int32 ReloadAmount = AmmoutToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	CombatState = ECombatState::ECS_Reloading;
	HandReload();
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr)
	{
		return;
	}
	if ( Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
	}
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			Fire();
		}
		break;
	case ECombatState::ECS_Reloading:
		HandReload();
		break;
	case ECombatState::ECS_MAX:
		break;
	default:
		break;
	}
}

void UCombatComponent::HandReload()
{
	Character->PlayReloadMontage();
}

int32 UCombatComponent::AmmoutToReload()
{
	if (EquippedWeapon == nullptr)
	{
		return 0;
	}
	// 需要添加的子弹数
	int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		//当前武器拥有的备弹数
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);

		return FMath::Clamp(RoomInMag, 0, Least);
	}
	return 0;
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	//获取屏幕大小
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	//屏幕位置转换后的世界位置
	FVector CrosshairWorldPosition;
	//屏幕位置转换后的方向
	FVector CrosshairWorldDirection;

	//DeprojectScreenToWorld将屏幕坐标转换为世界坐标
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		if (Character)
		{
			//开始位置调整为角色身体前面一点，避免有敌人出现在摄像机和角色中间位置时导致的bug
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
		//以屏幕中心在世界的位置为起点进行射线检测
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		//测试用
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
		else
		{
			//在检测到的地方绘制一个球体
			DrawDebugSphere(
				GetWorld(),
				TraceHitResult.ImpactPoint,
				12.f,
				12,
				FColor::Red
			);
		}

		//击中位置,
		HitTarget = TraceHitResult.ImpactPoint;

		//击中玩家后准星颜色改变:实现UInteractWithCrosshairInterface接口的玩家
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairInterface>())
		{
			HUDPackage.CrosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if ( Character == nullptr || Character->Controller == nullptr )
	{
		return;
	}
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}

			//准星扩散  将速度从 0~maxSpeed 映射为0~1的值
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0;
			
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());


			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime,30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootFactor = FMath::FInterpTo(CrosshairShootFactor, 0.f, DeltaTime, 30.f);

			float Spread = 
				0.5f+
				CrosshairInAirFactor + 
				CrosshairVelocityFactor -
				CrosshairAimFactor  +
				CrosshairShootFactor;

			HUD->SetHUDPackage(HUDPackage,Spread);
		}
	}
}
