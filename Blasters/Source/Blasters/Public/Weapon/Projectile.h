// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTERS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;



protected:
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	//µØµ¿Ãÿ–ß
	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	class UParticleSystemComponent* TracerComponent;

public:	


};
