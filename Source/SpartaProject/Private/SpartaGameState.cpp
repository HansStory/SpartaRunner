#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	LevelDuration = 30.0f;
	UpdateRepeatRate = 0.01f;

	PlayTime = 0.0f;
	WaveRemainTime = 0.0f;

	CurrentLevelIndex = 0;
	MaxLevels = 3;

	CurrentWaveIndex = 1;
	MaxWaves = 3;

	MaxLevelLength = 82720.0f;
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateGameState,
		UpdateRepeatRate,
		true
	);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScroe(Amount);
			UpdateScoreHUD();
		}
	}
}

void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), ItemSpawnVolumes);

	CurrentWaveIndex = 1;

	StartWave(CurrentWaveIndex);
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
		CollectedCoinCount,
		SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndLevel();
	}
}

void ASpartaGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	CurrentLevelIndex++;

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}

}

void ASpartaGameState::ClearWave()
{
	switch (CurrentWaveIndex)
	{
	case 1:
	case 2:
		CurrentWaveIndex++;
		UpdateWaveHUD();
		StartWave(CurrentWaveIndex);
		break;
	case 3:
		EndWave();
		break;
	default:
		break;
	}

}

void ASpartaGameState::StartWave(int index)
{
	WaveRemainTime += 30.0f;

	DisplayWaveQuizHUD(index);

	SpawnItem(index);
}

void ASpartaGameState::DisplayWaveQuizHUD(int index)
{
	//UUserWidget* HUDWidgetInstance = nullptr;
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidgetInstance = SpartaPlayerController->HUDWidgetInstance)
			{
				FString animNameStr = FString::Printf(TEXT("AnimQuiz%d"), index);
				FName animName = FName(*animNameStr);

				UFunction* PlayAnimFunc = HUDWidgetInstance->FindFunction(FName(animName));// "AnimQuiz1"));
				if (PlayAnimFunc)
				{
					HUDWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
				}
			}
		}
	}
}

void ASpartaGameState::SpawnItem(int index)
{
	const int32 ItemToSpawn = 30 + (index * 10);

	if (ItemSpawnVolumes.Num() > 0)
	{
		if (index - 1 < 0 || index - 1 > ItemSpawnVolumes.Num()) return;

		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(ItemSpawnVolumes[index - 1]);
		if (SpawnVolume)
		{
			for (int32 i = 0; i < ItemToSpawn; i++)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
			}
		}
	}
}

void ASpartaGameState::EndWave()
{
	OnGameOver(true);
}


void ASpartaGameState::OnGameOver(bool isClear)
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowGameEndMenu(isClear);
		}
	}
}


void ASpartaGameState::UpdateGameState()
{
	// Player 시간 업데이트
	PlayTime += UpdateRepeatRate;

	// Wave 남은시간 업데이트
	WaveRemainTime -= UpdateRepeatRate;

	UpdateHUD();

	if (WaveRemainTime <= 0.0f)
	{
		OnGameOver();
	}
}

void ASpartaGameState::UpdateHUD()
{
	UUserWidget* HUDWidget = GetHudWidget();
	if (HUDWidget == nullptr) return;

	// 웨이브 남은시간 업데이트
	if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
	{
		//float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
		TimeText->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), WaveRemainTime)));
	}

	// 총 게임 진행시간 표시
	if (UTextBlock* PlayTimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TotalTime"))))
	{
		int32 Minutes = static_cast<int32>(PlayTime) / 60;
		int32 Seconds = static_cast<int32>(PlayTime) % 60;
		int32 Milliseconds = static_cast<int32>((PlayTime - static_cast<int32>(PlayTime)) * 100);

		PlayTimeText->SetText(FText::FromString(FString::Printf(TEXT("TIME %02d:%02d:%02d"), Minutes, Seconds, Milliseconds)));
	}

	// 현재 내위치 스크롤 바로 표시
	if (UProgressBar* MyPosProgressBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("MyPosProgressBar"))))
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			APawn* PlayerPawn = PlayerController->GetPawn();
			if (PlayerPawn)
			{
				float PlayerX = PlayerPawn->GetActorLocation().X;
				float ClampedX = FMath::Clamp(PlayerX, 0.0f, MaxLevelLength);

				float Percent = ClampedX / MaxLevelLength;

				MyPosProgressBar->SetPercent(Percent);
			}
		}
	}
}

void ASpartaGameState::UpdateScoreHUD()
{
	UUserWidget* HUDWidget = GetHudWidget();
	if (HUDWidget == nullptr) return;

	if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
			if (SpartaGameInstance)
			{
				ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Socre : %d"), SpartaGameInstance->TotalScore)));
			}
		}
	}
}

void ASpartaGameState::UpdateWaveHUD()
{
	UUserWidget* HUDWidget = GetHudWidget();
	if (HUDWidget == nullptr) return;

	if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
	{
		LevelText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentWaveIndex, MaxWaves)));
	}
}

void ASpartaGameState::UpdateCharacterHP(float CurrentHP, float MaxHP)
{
	UUserWidget* HUDWidget = GetHudWidget();
	if (HUDWidget == nullptr) return;

	// Set HP TExt
	if (UTextBlock* HPText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Text_HP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.f / %.f"), CurrentHP, MaxHP)));
	}

	if (UProgressBar* HPBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HealthBar"))))
	{
		const float HPPercent = (MaxHP > 0.f) ? CurrentHP / MaxHP : 0.f;
		HPBar->SetPercent(HPPercent);
	}
}


UUserWidget* ASpartaGameState::GetHudWidget()
{
	UUserWidget* HUDWidget = nullptr;

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			HUDWidget = SpartaPlayerController->GetHUDWidget();
		}
	}

	return HUDWidget;
}



