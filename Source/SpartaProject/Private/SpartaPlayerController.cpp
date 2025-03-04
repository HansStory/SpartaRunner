#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

ASpartaPlayerController::ASpartaPlayerController()
	:InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr),
	GameEndWidgetClass(nullptr),
	GameEndWidgetInstance(nullptr)
{
}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				SubSystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->UpdateHUD();
	}
	
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu();
	}
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void ASpartaPlayerController::ShowMainMenu()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (GameEndWidgetInstance)
	{
		GameEndWidgetInstance->RemoveFromParent();
		GameEndWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}
}

void ASpartaPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (GameEndWidgetInstance)
	{
		GameEndWidgetInstance->RemoveFromParent();
		GameEndWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
		if (SpartaGameState)
		{
			SpartaGameState->UpdateHUD();
			SpartaGameState->UpdateScoreHUD();
			SpartaGameState->UpdateWaveHUD();
		}

		UFunction* PlayAnimFunc = HUDWidgetInstance->FindFunction(FName("AnimTimer"));
		if (PlayAnimFunc)
		{
			HUDWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
		}
	}
}

void ASpartaPlayerController::ShowGameEndMenu(bool isClear)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (GameEndWidgetInstance)
	{
		GameEndWidgetInstance->RemoveFromParent();
		GameEndWidgetInstance = nullptr;
	}

	GameEndWidgetInstance = CreateWidget<UUserWidget>(this, GameEndWidgetClass);
	if (GameEndWidgetInstance)
	{
		GameEndWidgetInstance->AddToViewport();

		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
	}

	if (UTextBlock* PlayTimeText = Cast<UTextBlock>(GameEndWidgetInstance->GetWidgetFromName("PlayTimeText")))
	{
		ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
		if (SpartaGameState)
		{
			float playTime = SpartaGameState->PlayTime;
			int32 Minutes = static_cast<int32>(playTime) / 60;
			int32 Seconds = static_cast<int32>(playTime) % 60;
			int32 Milliseconds = static_cast<int32>((playTime - static_cast<int32>(playTime)) * 100);

			PlayTimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, Milliseconds)));
		}
	}

	if (UTextBlock* TotalScoreText = Cast<UTextBlock>(GameEndWidgetInstance->GetWidgetFromName("TotalScoreText")))
	{
		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
		{
			TotalScoreText->SetText(FText::FromString(
				FString::Printf(TEXT("%d"), SpartaGameInstance->TotalScore)
			));
		}
	}

	if (UTextBlock* GameOverText = Cast<UTextBlock>(GameEndWidgetInstance->GetWidgetFromName("TextGameOver")))
	{
		if (isClear)
		{
			GameOverText->SetText(FText::FromString(FString::Printf(TEXT("Clear Game!!"))));
		}
		else
		{
			GameOverText->SetText(FText::FromString(FString::Printf(TEXT("Game Over"))));
		}

		UFunction* PlayAnimFunc = GameEndWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
		if (PlayAnimFunc)
		{
			GameEndWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
		}

	}
}

void ASpartaPlayerController::StartGame()
{
	if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SpartaGameInstance->CurrentLevelIndex = 0;
		SpartaGameInstance->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("NumericLevel"));
	SetPause(false);
}