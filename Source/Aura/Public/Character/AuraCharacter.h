// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class AAuraPlayerState;
class UCameraComponent;
class USpringArmComponent;
/**
 * 
 */
UCLASS

()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
    AAuraCharacter();
    //~ Begin APawn Interface
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;
    //~ End APawn Interface
private:
    virtual void InitAbilityActorInfo() override;
    
    UPROPERTY(VisibleAnywhere, Category = "Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> FollowCamera;
};
