--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_HealthPotion_C
local M = UnLua.Class()


function M:ReceiveBeginPlay()
    -- 给Sphere绑定Overlap事件
    self.Sphere.OnComponentBeginOverlap:Add(self, M.OnSphereOverlap)
end

function M:OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult)
    self:ApplyEffectToTarget(OtherActor, self.InstantGameplayEffectClass)
end

return M
