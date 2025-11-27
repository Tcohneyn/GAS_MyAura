--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type GA_HitReact_C
local M = UnLua.Class()
function M:K2_ActivateAbility()
   self.EffectHandle = self:BP_ApplyGameplayEffectToOwner(self.GameplayEffectClass,1,1)
   local CombatInter = self:GetAvatarActorFromActorInfo():Cast(UE.UCombatInterface)
   if CombatInter then
      local Montage = CombatInter:GetHitReactMontage()
      local PlayMontageTask = UE.UAbilityTask_PlayMontageAndWait.CreatePlayMontageAndWaitProxy(self, "PlayMontageTask",
        Montage)
      PlayMontageTask.OnCompleted:Add(self, M.OnMontage)
      PlayMontageTask.OnInterrupted:Add(self, M.OnMontage)
      PlayMontageTask.OnCancelled:Add(self, M.OnMontage)
      PlayMontageTask:ReadyForActivation()
   
   end
end

function M:OnMontage()
    self:BP_RemoveGameplayEffectFromOwnerWithHandle(self.EffectHandle,1)
    self:K2_EndAbility()
end
return M