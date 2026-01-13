--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BTT_Attack_C
local M = UnLua.Class()
function M:ReceiveExecuteAI(_OwnerController, ControlledPawn)
    local CTSelector = UE.UBTFunctionLibrary.GetBlackboardValueAsActor(self, self.CombatTargetSelector)
if CTSelector then
    ControlledPawn:SetCombatTarget(CTSelector)
    local AbilitySystemComponent = UE.UAbilitySystemBlueprintLibrary.GetAbilitySystemComponent(ControlledPawn)
    if AbilitySystemComponent then
        local TagContainer = UE.UBlueprintGameplayTagLibrary.MakeGameplayTagContainerFromTag(self.AttackTag)
        AbilitySystemComponent:TryActivateAbilitiesByTag(TagContainer, true)
        self:FinishExecute(true)
    end
else
    self:FinishExecute(true)
end
end
return M