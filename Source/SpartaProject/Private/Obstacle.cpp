#include "Obstacle.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"

AObstacle::AObstacle()
{
	PrimaryActorTick.bCanEverTick = false;

	ObstacleType = "DefaultObstacle";
	bIsOverlap = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Collision);

	// 이벤트 바인딩
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AObstacle::OnObstacleOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AObstacle::OnObstacleEndOverlap);

	Damage = 10.0f;
	ParticleLifeTime = 2.0f;
}

void AObstacle::OnObstacleOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Overlap!!")));
		ActivateObstacle(OtherActor);
	}
}

void AObstacle::OnObstacleEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	bIsOverlap = false;
}

void AObstacle::ActivateObstacle(AActor* Activator)
{
	if (bIsOverlap) return;
	bIsOverlap = true;

	UParticleSystemComponent* Particle = nullptr;

	// 오버랩 파티클
	if (OverlapParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			OverlapParticle,
			GetActorLocation(),
			GetActorRotation(),
			true
		);
	}

	// 파티클 있을시 삭제
	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			ParticleLifeTime,
			false
		);
	}

	if (Activator && Activator->ActorHasTag("Player"))
	{
		// 대미지 받는 부분
		UGameplayStatics::ApplyDamage(
			Activator,
			Damage,
			nullptr,
			this,
			UDamageType::StaticClass()
		);

		// LOG
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("Player damaged %f by Obstacle!!"), Damage));


		// 사운드 출력
		if (OverlapSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				OverlapSound,
				GetActorLocation()
			);
		}
	}
}

FName AObstacle::GetNameType() const
{
	return ObstacleType;
}


