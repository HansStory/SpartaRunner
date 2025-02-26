#include "SwingAxeObstacle.h"

ASwingAxeObstacle::ASwingAxeObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	ObstacleType = "SwingAxeObstacle";
	SwingDuration = 2.0f;

	EmptyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EmptyMesh"));
	EmptyMesh->SetupAttachment(Scene);

	Collision->SetupAttachment(EmptyMesh);
	StaticMesh->SetupAttachment(Collision);

	StartRotate = FRotator(0, 0, -90);
	EndRotate = FRotator(0, 0, 90);
}

void ASwingAxeObstacle::BeginPlay()
{
	Super::BeginPlay();

	StartTweenAnimation();;
}

void ASwingAxeObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TweenTimeline.TickTimeline(DeltaTime);
}

void ASwingAxeObstacle::StartTweenAnimation()
{
	if (!TweenCurve) return;

	// Ʈ�� Ÿ�Ӷ��� ����
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, FName("HandleTimelineProgress"));
	TweenTimeline.AddInterpFloat(TweenCurve, ProgressFunction);

	FOnTimelineEvent OnFinished;
	OnFinished.BindUFunction(this, FName("OnTimelineFinished"));
	TweenTimeline.SetTimelineFinishedFunc(OnFinished);

	// 2�� ���� �ִϸ��̼� ����
	TweenTimeline.SetLooping(false);
	TweenTimeline.SetPlayRate(1.0f / SwingDuration);
	TweenTimeline.PlayFromStart();

}

void ASwingAxeObstacle::HandleTimelineProgress(float Value)
{
	FRotator NewRotation = FMath::Lerp(StartRotate, EndRotate, Value);
	EmptyMesh->SetRelativeRotation(NewRotation);
}

void ASwingAxeObstacle::OnTimelineFinished()
{
	TweenTimeline.Reverse();

	FOnTimelineEvent OnFinished;
	OnFinished.BindUFunction(this, FName("StartTweenAnimation"));
	TweenTimeline.SetTimelineFinishedFunc(OnFinished);
}

