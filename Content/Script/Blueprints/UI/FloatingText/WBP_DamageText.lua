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
end

function M:UpdateDamageText(Damage)
   self.Text_Damage:SetText(UE.UKismetTextLibrary.Conv_DoubleToText(Damage,UE.ERoundingMode.HalfToEven,false,true,1,324,0,0))
end
   
return M
