// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Announcement.generated.h"

/**
 *
 */
UCLASS()
class BLASTERS_API UAnnouncement : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetAnnouncementCountdown(float CountdownTime);
	void FinishiedGame();

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WarmupTime;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AnnouncementText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoText;

};
