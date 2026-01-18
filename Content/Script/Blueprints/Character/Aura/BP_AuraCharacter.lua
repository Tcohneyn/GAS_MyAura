--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_AuraCharacter_C
local M = UnLua.Class()

function M:ReceiveBeginPlay()
    -- 给Sphere绑定Overlap事件
    self.Box.OnComponentBeginOverlap:Add(self, M.OnBoxBeginOverlap)
    self.Box.OnComponentEndOverlap:Add(self, M.OnBoxEndOverlap)
end

function M:OnBoxBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult)
    local value = UE.UKismetSystemLibrary.DoesImplementInterface(OtherActor, self.Interface)
    if value then
        OtherActor:FadeOut()
    end
end

function M:OnBoxEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult)
       local value = UE.UKismetSystemLibrary.DoesImplementInterface(OtherActor, self.Interface)
    if value then
        OtherActor:FadeIn()
    end
end
function M:UpdateFacingTarget(Target)
   self.MotionWarping:AddOrUpdateWarpTargetFromLocation("FaceTarget", Target)
end


return M
