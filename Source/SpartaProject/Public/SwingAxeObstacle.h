#pragma once

#include "CoreMinimal.h"
#include "Obstacle.h"
#include "Components/TimelineComponent.h"
#include "SwingAxeObstacle.generated.h"

UCLASS()
class SPARTAPROJECT_API ASwingAxeObstacle : public AObstacle
{
	GENERATED_BODY()
	
public:	
	ASwingAxeObstacle();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	FTimeline TweenTimeline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Component")
	UCurveFloat* TweenCurve;
	UFUNCTION()
	void StartTweenAnimation();
	UFUNCTION()
	void HandleTimelineProgress(float Value);
	UFUNCTION()
	void OnTimelineFinished();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
	float SwingDuration;
};
