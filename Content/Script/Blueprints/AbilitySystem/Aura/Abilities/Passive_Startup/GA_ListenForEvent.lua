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
   UE.UKismetSystemLibrary.PrintString(self, "GA_ListenForEvent Activated".. payload.EventMagnitude)
   local spec = ASC:MakeOutgoingSpec(self.EventBasedEffectClass, 1, ASC:MakeEffectContext())
   UE.UAbilitySystemBlueprintLibrary.AssignTagSetByCallerMagnitude(spec, payload.EventTag, payload.EventMagnitude)
   self:GetAbilitySystemComponentFromActorInfo():BP_ApplyGameplayEffectSpecToSelf(spec)
end
return M
