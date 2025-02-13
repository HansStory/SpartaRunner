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

	// 트윈 타임라인 설정
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, FName("HandleTimelineProgress"));
	TweenTimeline.AddInterpFloat(TweenCurve, ProgressFunction);

	FOnTimelineEvent OnFinished;
	OnFinished.BindUFunction(this, FName("OnTimelineFinished"));
	TweenTimeline.SetTimelineFinishedFunc(OnFinished);

	// 2초 동안 애니메이션 실행
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
		HideThornTimerHandle,		// 타이머 핸들
		this,
		&AThornObstacle::HideThorn, // 실행할 함수
		2.0f,						// 대기 시간 (초)
		false						// 반복 여부 (false = 한 번만 실행)
	);
}

void AThornObstacle::HideThorn()
{
	if (!TweenCurve) return;

	// 타임라인 초기화
	TweenTimeline.Stop();
	TweenTimeline = FTimeline();

	// 타임라인 설정 (반대로 움직이게 설정)
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

