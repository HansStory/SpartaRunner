#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstacleSpawnVolume.generated.h"

UCLASS()
class SPARTAPROJECT_API AObstacleSpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AObstacleSpawnVolume();

protected:
	virtual void BeginPlay() override;

public:

    UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<AActor> ObstacleClass;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    int32 GridWidth = 100;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    int32 GridHeight = 6;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    float TileSize = 200; // 1x1 Ÿ���� 100 Unreal Units�� ����

    void SpawnObstacles(int32 Count);

private:
    FVector GetUniqueRandomGridLocation();
    TSet<FIntPoint> UsedLocations; // �ߺ� ������ ���� Set
};
