--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_DamageText_C
local M = UnLua.Class()

function M:Construct()
   self:PlayAnimation(self.DamageAnim)
   self:PlayAnimation(self.HitMessageAnim)
end

function M:UpdateDamageText(Damage, bBlockedHit, bCriticalHit)
   local Color = self:GetColorBasedOnBlockAndCrit(bBlockedHit, bCriticalHit)
   local HitMessage = self:GetHitMessageBasedOnBlockAndCrit(bBlockedHit, bCriticalHit)
   self.Text_Damage:SetText(UE.UKismetTextLibrary.Conv_DoubleToText(Damage,UE.ERoundingMode.HalfToEven,false,true,1,324,0,0))
   self.Text_Damage:SetColorAndOpacity(Color)
   self.Text_HitMessage:SetText(UE.UKismetTextLibrary.Conv_StringToText(HitMessage))
   self.Text_HitMessage:SetColorAndOpacity(Color)
end

function M:GetColorBasedOnBlockAndCrit(bBlockedHit, bCriticalHit)
    if bBlockedHit and not bCriticalHit then
        return self.Color_0 -- Blue for Blocked, No Hit
    elseif not bBlockedHit and bCriticalHit then
        return self.Color_1 -- Red for Critical, No Block
    elseif bBlockedHit and bCriticalHit then
        return self.Color_2 -- Orange for Blocked Critical Hit
    elseif not bBlockedHit and not bCriticalHit then
        return self.Color -- White for Normal Hit
    else
        return self.Color -- Default to White
    end
end

function M:GetHitMessageBasedOnBlockAndCrit(bBlock, bCrit)
    if bBlock and not bCrit then
        return "Blocked"
    elseif not bBlock and bCrit then
        return "Critical Hit"
    elseif bBlock and bCrit then
        return "Blocked Critical Hit"
    elseif not bBlock and not bCrit then
        return "Normal Hit"
    else
        return "Normal Hit"
    end
end
return M
