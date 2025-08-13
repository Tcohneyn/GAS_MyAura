--
-- DESCRIPTION
--
-- @COMPANY 
-- @AUTHOR 
-- @DATE 2025/08/12 23:16:25
--

---@type ABP_AuraCharacter_C
local M = UnLua.Class()

function M:BlueprintInitializeAnimation()
   local PawnOwner = self:TryGetPawnOwner()
   self.AuraCharacter = PawnOwner:Cast(UE.AAuraCharacter)
   self.CharacterMovement = self.AuraCharacter.CharacterMovement
end
function M:BlueprintUpdateAnimation(_DeltaTimeX)
    if not self.AuraCharacter then
        return
    end
    
    local Velocity = self.AuraCharacter:GetVelocity()
    local Speed = UE.UKismetMathLibrary.VSizeXY(Velocity)
    self.GroundSpeed = Speed
    if self.GroundSpeed > 3.0 then
       self.ShouldMove = true
    end
end

return M
