--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type AN_MontageEvent_C
local M = UnLua.Class()

function M:Received_Notify(MeshComp, Animation, EventReference)
    UE.UAbilitySystemBlueprintLibrary.SendGameplayEventToActor(MeshComp:GetOwner(), self.EventTag, nil)
    return false
end

return M