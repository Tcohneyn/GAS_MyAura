--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_GlobeProgressBar_C
local M = UnLua.Class()

function M:Tick(MyGeometry, InDeltaTime)
   --print("CurrentPercent",self.ProgressBar_Ghost.Percent)
   --print("GhostPercentTarget",self.GhostPercentTarget)
   local value = UE.UKismetMathLibrary.FInterpTo(self.ProgressBar_Ghost.Percent,self.GhostPercentTarget,InDeltaTime,self.GhostInterpSpeed)
   --print("value",value)
   self:SetGhostProgressBarPercent(value)
end

return M
