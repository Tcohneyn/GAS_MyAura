// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
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

    /** Combat Interface */
    virtual int32 GetPlayerLevel() override;
    /** end Combat Interface */
    UPROPERTY(BlueprintAssignable)
    FOnAttributeChangedSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable)
    FOnAttributeChangedSignature OnMaxHealthChanged;
protected:
    //~ Begin APawn Interface
    virtual void BeginPlay() override;
    //~ End APawn Interface

    virtual void InitAbilityActorInfo() override;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
    int32 Level = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UWidgetComponent> HealthBar;
};