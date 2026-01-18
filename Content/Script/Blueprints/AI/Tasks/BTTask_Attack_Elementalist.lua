--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BTTask_Attack_Elementalist_C
local M = UnLua.Class()
function M:ReceiveExecuteAI(_OwnerController, ControlledPawn)
    local CTSelector = UE.UBTFunctionLibrary.GetBlackboardValueAsActor(self, self.CombatTargetSelector)
if CTSelector then
    ControlledPawn:SetCombatTarget(CTSelector)
    local MinionCount = ControlledPawn:GetMinionCount()
    if MinionCount < self.MinionSpawnThreshold then
       self.AbilityTag = self.SummonTag
    else
       self.AbilityTag = self.AttackTag
    end
    local AbilitySystemComponent = UE.UAbilitySystemBlueprintLibrary.GetAbilitySystemComponent(ControlledPawn)
    if AbilitySystemComponent then
        local TagContainer = UE.UBlueprintGameplayTagLibrary.MakeGameplayTagContainerFromTag(self.AbilityTag)
        AbilitySystemComponent:TryActivateAbilitiesByTag(TagContainer, true)
        self:FinishExecute(true)
    end
else
    self:FinishExecute(true)
end
end
return M