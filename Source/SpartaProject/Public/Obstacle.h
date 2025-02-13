#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "Components/TimelineComponent.h"
#include "Obstacle.generated.h"

class UBoxComponent;

UCLASS()
class SPARTAPROJECT_API AObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	AObstacle();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
	FName ObstacleType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Obstacle|Component")
	USceneComponent* Scene;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Component")
	UPrimitiveComponent* Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Obstacle|Component")
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Effects")
	UParticleSystem* OverlapParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Effects")
	USoundBase* OverlapSound;

	UFUNCTION()
	virtual void OnObstacleOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category = "Obstacle")
	virtual void OnObstacleEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	virtual void ActivateObstacle(AActor* Activator);
	virtual FName GetNameType() const;

	bool bIsOverlap;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Effects")
	float ParticleLifeTime;

};
