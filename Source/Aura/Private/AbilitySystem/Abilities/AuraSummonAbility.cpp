// Tcohneyn All Rights Reserved


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

/**
 * 获取随从的生成位置列表
 * 算法逻辑：在角色前方定义的扇形角度（SpawnSpread）内，平均分配方向，并在每个方向的随机距离内选取点。
 */
TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
    // 1. 获取基础信息：召唤者（萨满）的前方向量和当前世界坐标
    const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
    const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
    
    // 2. 计算每个随从之间的角度增量（步长）
    // 例如：总张角 90 度，召唤 5 个随从，则每个随从间隔 18 度
    const float DeltaSpread = SpawnSpread / NumMinions;

    // 3. 计算扇形的起始旋转向量（最左侧的边界）
    // 将前方向量绕 Z 轴（UpVector）向左旋转总张角的一半
    const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
    
    TArray<FVector> SpawnLocations; // 用于存储最终计算出的位置数组

    // 4. 循环计算每一个随从的具体生成点
    for (int32 i = 0; i < NumMinions; i++)
    {
        // 计算当前随从的方向向量：从最左侧开始，根据索引 i 逐步向右旋转步长角度
        const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
        
        // 计算最终的世界坐标点：
        // 起点位置 + 方向向量 * 随机距离（在设定的最小和最大距离之间随机抽样）
        FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
        
        FHitResult Hit;
        GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);
        if (Hit.bBlockingHit)
        {
            ChosenSpawnLocation = Hit.ImpactPoint;
        }
        // 将计算好的坐标添加到数组中
        SpawnLocations.Add(ChosenSpawnLocation);
       
        // /* --- 调试绘图部分 (Debug Visualization) --- */
        //
        // // 绘制青色球体：代表最终选中的生成点位置
        // DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 18.f, 12, FColor::Cyan, false, 3.f );
        //
        // // 绘制绿色箭头：从萨满位置指向最大生成距离处，表示该随从分配到的方向线
        // UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.f, FLinearColor::Green, 3.f );
        //
        // // 绘制红色小球：标记最小距离边界（防止随从生成在萨满身上）
        // DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 5.f, 12, FColor::Red, false, 3.f );
        //
        // // 绘制红色小球：标记最大距离边界
        // DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 5.f, 12, FColor::Red, false, 3.f );
    }
    
    return SpawnLocations; // 返回计算完毕的所有坐标数组
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
    const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
    return MinionClasses[Selection];
}