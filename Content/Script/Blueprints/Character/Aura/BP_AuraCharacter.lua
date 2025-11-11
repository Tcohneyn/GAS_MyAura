--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_AuraCharacter_C
local M = UnLua.Class()

function M:UpdateFacingTarget(Target)
   self.MotionWarping:AddOrUpdateWarpTargetFromLocation("FaceTarget", Target)
end


return M
