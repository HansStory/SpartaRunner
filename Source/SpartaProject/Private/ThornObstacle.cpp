#include "ThornObstacle.h"
#include "Curves/CurveFloat.h"

AThornObstacle::AThornObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	ObstacleType = "ThornObstacle";
}

void AThornObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TweenTimeline.TickTimeline(DeltaTime);
}

void AThornObstacle::OnObstacleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AThornObstacle::ActivateObstacle(AActor* Activator)
{
	ShowThorn();
	AObstacle::ActivateObstacle(Activator);
}

void AThornObstacle::ShowThorn()
{
	//if (!TweenCurve) return;
	if (bIsOverlap) return;

	// Ʈ�� Ÿ�Ӷ��� ����
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, FName("HandleTimelineProgress"));
	TweenTimeline.AddInterpFloat(TweenCurve, ProgressFunction);

	FOnTimelineEvent OnFinished;
	OnFinished.BindUFunction(this, FName("OnTimelineFinished"));
	TweenTimeline.SetTimelineFinishedFunc(OnFinished);

	// 2�� ���� �ִϸ��̼� ����
	TweenTimeline.SetLooping(false);
	TweenTimeline.SetPlayRate(1.0f / 0.5f);
	TweenTimeline.PlayFromStart();
}

void AThornObstacle::HandleTimelineProgress(float Value)
{
	FVector NewLocation = FMath::Lerp(HidePos, FVector(), Value);
	StaticMesh->SetRelativeLocation(NewLocation);
}

void AThornObstacle::OnTimelineFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("Finished Show Thron Tween Animation!"));

	GetWorld()->GetTimerManager().SetTimer(
		HideThornTimerHandle,		// Ÿ�̸� �ڵ�
		this,
		&AThornObstacle::HideThorn, // ������ �Լ�
		2.0f,						// ��� �ð� (��)
		false						// �ݺ� ���� (false = �� ���� ����)
	);
}

void AThornObstacle::HideThorn()
{
	if (!TweenCurve) return;

	// Ÿ�Ӷ��� �ʱ�ȭ
	TweenTimeline.Stop();
	TweenTimeline = FTimeline();

	// Ÿ�Ӷ��� ���� (�ݴ�� �����̰� ����)
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, FName("HandleTimelineProgressReverse"));
	TweenTimeline.AddInterpFloat(TweenCurve, ProgressFunction);

	FOnTimelineEvent FinishFunction;
	FinishFunction.BindUFunction(this, FName("OnTimelineReverseFinished"));
	TweenTimeline.SetTimelineFinishedFunc(FinishFunction);

	TweenTimeline.SetLooping(false);
	TweenTimeline.SetPlayRate(1.0f / 2.0f);
	TweenTimeline.PlayFromStart();
}


void AThornObstacle::HandleTimelineProgressReverse(float Value)
{
	FVector NewLocation = FMath::Lerp(FVector(), HidePos, Value);
	StaticMesh->SetRelativeLocation(NewLocation);
}

void AThornObstacle::OnTimelineReverseFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("Finished Hide Thron Tween Animation!"));

	bIsOverlap = false;
}

