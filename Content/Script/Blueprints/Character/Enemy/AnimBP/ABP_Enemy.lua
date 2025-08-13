--
-- DESCRIPTION
--
-- @COMPANY 
-- @AUTHOR 
-- @DATE 2025/08/12 23:55:49
--

---@type ABP_Enemy_C
local M = UnLua.Class()

function M:BlueprintInitializeAnimation()
   local PawnOwner = self:TryGetPawnOwner()
   local EnemyCharacter = PawnOwner:Cast(UE.AAuraEnemy)
   self.CharacterMovement = EnemyCharacter.CharacterMovement
end
function M:BlueprintUpdateAnimation(_DeltaTimeX)
    if not self.CharacterMovement then
        return
    end
    
    local velocity = self.CharacterMovement.Velocity
    local Speed = UE.UKismetMathLibrary.VSizeXY(velocity)
    self.GroundSpeed = Speed
end

return M
