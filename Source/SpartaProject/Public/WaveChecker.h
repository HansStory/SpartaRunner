#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveChecker.generated.h"

UCLASS()
class SPARTAPROJECT_API AWaveChecker : public AActor
{
	GENERATED_BODY()
	
public:	
	AWaveChecker();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item|Component")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item|Component")
	class UPrimitiveComponent* Collision;

	UFUNCTION()
	virtual void OnWaveCheckerOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

private:
	bool isOverlapped;
};
