// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Announcement.h"
#include "UMG/Public/Components/TextBlock.h"


void UAnnouncement::SetAnnouncementCountdown(float CountdownTime)
{
	if (CountdownTime < 0.f)
	{
		WarmupTime->SetText(FText());
		return;
	}
	int32 Minutes = FMath::FloorToInt(CountdownTime / 60);
	int32 Seconds = CountdownTime - Minutes * 60;

	FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	WarmupTime->SetText(FText::FromString(CountdownText));
}

void UAnnouncement::FinishiedGame()
{
	FString Text = TEXT("新游戏倒计时： ");
	AnnouncementText->SetText(FText::FromString(Text));
	InfoText->SetText(FText());
}
