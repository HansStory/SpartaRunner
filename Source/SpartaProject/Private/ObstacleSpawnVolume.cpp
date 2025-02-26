#include "ObstacleSpawnVolume.h"

AObstacleSpawnVolume::AObstacleSpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AObstacleSpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	SpawnObstacles(30);
}

void AObstacleSpawnVolume::SpawnObstacles(int32 Count)
{
    if (!ObstacleClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ObstacleClass가 설정되지 않았습니다!"));
        return;
    }

    Count = FMath::Min(Count, GridWidth * GridHeight);

    for (int32 i = 0; i < Count; i++)
    {
        FVector SpawnLocation = GetUniqueRandomGridLocation();
        FRotator SpawnRotation = FRotator::ZeroRotator;

        GetWorld()->SpawnActor<AActor>(ObstacleClass, SpawnLocation, SpawnRotation);
    }
}

FVector AObstacleSpawnVolume::GetUniqueRandomGridLocation()
{
    FIntPoint GridPoint;
    do
    {
        int32 X = FMath::RandRange(0, GridWidth - 1);
        int32 Y = FMath::RandRange(0, GridHeight - 1);
        GridPoint = FIntPoint(X, Y);
    } while (UsedLocations.Contains(GridPoint)); // 중복 방지

    UsedLocations.Add(GridPoint); // 사용된 위치 저장

    return FVector(GridPoint.X * TileSize + GetActorLocation().X, GridPoint.Y * TileSize + GetActorLocation().Y, 0.0f);
}