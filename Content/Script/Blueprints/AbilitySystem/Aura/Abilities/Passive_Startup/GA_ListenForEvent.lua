--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type GA_ListenForEvent_C
local M = UnLua.Class()
function M:K2_ActivateAbility()
    --UE.UKismetSystemLibrary.PrintString(self, "GA_ListenForEvent Activated")
    local WaitEventTask = UE.UAbilityTask_WaitGameplayEvent.WaitGameplayEvent(self, self.EventTag, nil,
        false, false)
    WaitEventTask.EventReceived:Add(self, M.OnEventReceived)
    WaitEventTask:ReadyForActivation()
end

function M:OnEventReceived(payload)
    local ASC = self:GetAbilitySystemComponentFromActorInfo()
    UE.UKismetSystemLibrary.PrintString(self, "GA_ListenForEvent Activated" .. payload.EventMagnitude)
    local spec = ASC:MakeOutgoingSpec(self.EventBasedEffectClass, 1, ASC:MakeEffectContext())
    local TagNum = self.EventTags:Num()
    for i = 1, TagNum do
        local item = self.EventTags[i]
        if UE.UBlueprintGameplayTagLibrary.MatchesTag(item, payload.EventTag, true) then
            UE.UAbilitySystemBlueprintLibrary.AssignTagSetByCallerMagnitude(spec, payload.EventTag, payload.EventMagnitude)
        else
            UE.UAbilitySystemBlueprintLibrary.AssignTagSetByCallerMagnitude(spec, item, 0)
        end
    end
    self:GetAbilitySystemComponentFromActorInfo():BP_ApplyGameplayEffectSpecToSelf(spec)
end

return M
