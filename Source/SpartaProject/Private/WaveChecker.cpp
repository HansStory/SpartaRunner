#include "WaveChecker.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SpartaGameState.h"

// Sets default values
AWaveChecker::AWaveChecker()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AWaveChecker::OnWaveCheckerOverlap);
}

void AWaveChecker::OnWaveCheckerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!isOverlapped)
	{
		if (OtherActor && OtherActor->ActorHasTag("Player"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Overlap!!")));
			
			ASpartaGameState* GameState = GetWorld()->GetGameState<ASpartaGameState>();
			if (GameState)
			{
				GameState->ClearWave();
			}

			isOverlapped = true;
		}

	}

}


