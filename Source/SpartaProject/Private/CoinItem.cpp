#include "CoinItem.h"
#include "Engine/World.h"
#include "SpartaGameState.h"

ACoinItem::ACoinItem()
{
	PrimaryActorTick.bCanEverTick = true;

	PointValue = 0;
	SpinSpeed = 180.0f;
	ItemType = "DefaultCoin";
}

void ACoinItem::BeginPlay()
{
	Super::BeginPlay();

	OriginalSpawnLocation = GetActorLocation();
}

void ACoinItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSpin(DeltaTime);
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (ASpartaGameState* GameState = World->GetGameState<ASpartaGameState>())
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		}

		DestroyItem();
	}
}

void ACoinItem::UpdateSpin(float DeltaTime)
{
	AddActorLocalRotation(FRotator(0.f, SpinSpeed * DeltaTime, 0.f));
}
