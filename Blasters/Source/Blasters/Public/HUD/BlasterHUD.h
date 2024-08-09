// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage 
{
	GENERATED_USTRUCT_BODY()

public:
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;

	//准星的浮动范围
	float CrosshairSpread;
	//准星颜色
	FLinearColor CrosshairColor;
};

UCLASS()
class BLASTERS_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
	

public:
	virtual void DrawHUD() override;

protected:
	virtual void BeginPlay() override;

public:
	void SetHUDPackage(const FHUDPackage& Package,float Spread);

	void AddCharacterOverlay();
	void AddAnnouncement();

protected:


private:
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter,FVector2D Spread, FLinearColor CrosshairColor);


public:
	UPROPERTY(EditAnywhere, Category = "Player Status")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	UPROPERTY(EditAnywhere, Category = "Player Status")
	TSubclassOf<UUserWidget> AnnouncementClass;

	class UCharacterOverlay* CharacterOverlay;
	class UAnnouncement* Announcement;

private:
	FHUDPackage HUDPackage;

	UPROPERTY(EditAnywhere)
	float CrosshaitSpreadMax = 16.f;


};
