// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Announcement.h"
#include "UMG/Public/Components/TextBlock.h"


void UAnnouncement::SetAnnouncementCountdown(float CountdownTime)
{
	int32 Minutes = FMath::FloorToInt(CountdownTime / 60);
	int32 Seconds = CountdownTime - Minutes * 60;

	FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	WarmupTime->SetText(FText::FromString(CountdownText));
}
