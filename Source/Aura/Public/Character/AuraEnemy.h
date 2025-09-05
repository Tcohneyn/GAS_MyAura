// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
    GENERATED_BODY()

public:
    AAuraEnemy();

    //** Enemy Interface **//
    virtual void HighlightActor() override;
    virtual void UnHighlightActor() override;
    //** End Enemy Interface **//
protected:
    //~ Begin APawn Interface
    virtual void BeginPlay() override;
    //~ End APawn Interface
};