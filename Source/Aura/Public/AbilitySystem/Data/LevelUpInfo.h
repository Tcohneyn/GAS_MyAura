// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
    GENERATED_BODY()
    //升级所需的经验值
    UPROPERTY(EditDefaultsOnly)
    int32 LevelUpRequirement = 0;
    //升级获得的属性分配点
    UPROPERTY(EditDefaultsOnly)
    int32 AttributePointAward = 1;
    //升级获得的技能点
    UPROPERTY(EditDefaultsOnly)
    int32 SpellPointAward = 1;
};
/**
 * 
 */
UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
public:

    UPROPERTY(EditDefaultsOnly)
    TArray<FAuraLevelUpInfo> LevelUpInformation;

    int32 FindLevelForXP(int32 XP) const;
};
