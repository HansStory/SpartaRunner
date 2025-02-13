#pragma once

#include "CoreMinimal.h"
#include "Obstacle.h"
#include "Components/TimelineComponent.h"
#include "ThornObstacle.generated.h"

UCLASS()
class SPARTAPROJECT_API AThornObstacle : public AObstacle
{
	GENERATED_BODY()
	
public:	
	AThornObstacle();

	virtual void Tick(float DeltaTime) override;

	virtual void OnObstacleEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;
	virtual void ActivateObstacle(AActor* Activator) override;


	void ShowThorn();
	void HideThorn();

	UPROPERTY()
	FTimeline TweenTimeline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Component")
	UCurveFloat* TweenCurve;
	UFUNCTION()
	void HandleTimelineProgress(float Value);
	UFUNCTION()
	void OnTimelineFinished();
	UFUNCTION()
	void HandleTimelineProgressReverse(float Value);
	UFUNCTION()
	void OnTimelineReverseFinished();

private:
	FTimerHandle HideThornTimerHandle;
	FVector HidePos = FVector(0.0f, 0.0f, -100.0f);

};
