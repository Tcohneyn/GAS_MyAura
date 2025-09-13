--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_FireArea_C
local M = UnLua.Class()

function M:ReceiveBeginPlay()
    -- 给Sphere绑定Overlap事件
    self.Box.OnComponentBeginOverlap:Add(self, M.OnBoxBeginOverlap)
    self.Box.OnComponentEndOverlap:Add(self, M.OnBoxEndOverlap)
end

function M:OnBoxBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult)
    self:OnOverlap(OtherActor)
end

function M:OnBoxEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult)
    self:OnEndOverlap(OtherActor)
end
return M
