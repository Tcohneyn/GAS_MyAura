--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_TestActor_C
local M = UnLua.Class()

function M:ReceiveBeginPlay()
    -- 给Sphere绑定Overlap事件
    self.Box.OnComponentBeginOverlap:Add(self, M.OnBoxOverlap)
end

function M:OnBoxOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult)
    self:ApplyEffectToTarget(OtherActor, self.InstantGameplayEffectClass)
    self:K2_DestroyActor()
end

return M
