--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_HealthCrystal_C
local M = UnLua.Class()

function M:ReceiveBeginPlay()
    -- 给Sphere绑定Overlap事件
    self.Capsule.OnComponentBeginOverlap:Add(self, M.OnCapsuleOverlap)
end

function M:OnCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult)
    self:ApplyEffectToTarget(OtherActor, self.DurationGameplayEffectClass)
end

return M
