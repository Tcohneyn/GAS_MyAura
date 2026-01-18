--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type GA_RangedAttack_C
local M = UnLua.Class()
function M:K2_ActivateAbility()
    local AvatarActor = self:GetAvatarActorFromActorInfo()
    local CombatTarget = AvatarActor:GetCombatTarget()
    AvatarActor:Cast(UE.UCombatInterface).UpdateFacingTarget(CombatTarget:K2_GetActorLocation())
    local AttackMontages = AvatarActor:GetAttackMontages()
    local FTaggedMontage = self:GetRandomTaggedMontageFromArray(AttackMontages)
    local PlayMontageTask = UE.UAbilityTask_PlayMontageAndWait.CreatePlayMontageAndWaitProxy(self, "PlayMontageTask",
        FTaggedMontage.Montage)
    PlayMontageTask.OnCompleted:Add(self, M.OnMontage)
    PlayMontageTask.OnInterrupted:Add(self, M.OnMontage)
    PlayMontageTask.OnCancelled:Add(self, M.OnMontage)
    PlayMontageTask:ReadyForActivation()
    local WaitEventTask = UE.UAbilityTask_WaitGameplayEvent.WaitGameplayEvent(self, FTaggedMontage.MontageTag, nil,
        true, true)
    WaitEventTask.EventReceived:Add(self, M.OnEventReceived)
    WaitEventTask:ReadyForActivation()
end
function M:OnMontage()
    self:K2_EndAbility()
end

function M:OnEventReceived(Payload)
    local AvatarActor = self:GetAvatarActorFromActorInfo()
    local AttackMontages = AvatarActor:GetAttackMontages()
    local FTaggedMontage = self:GetRandomTaggedMontageFromArray(AttackMontages)
    local CombatSocketLocation = AvatarActor:GetCombatSocketLocation(FTaggedMontage.MontageTag)
    local CombatTarget = AvatarActor:GetCombatTarget()
    --local SocketTagName = UE.UBlueprintGameplayTagLibrary.GetTagName(FTaggedMontage.SocketTag)
    --print("FTaggedMontage.SocketTag:" .. SocketTagName)
    self:SpawnProjectile(CombatTarget:K2_GetActorLocation(), FTaggedMontage.SocketTag,self.bShouldOverridePitch,self.PitchOverride)
    coroutine.resume(coroutine.create(
     function()    
            UE.UKismetSystemLibrary.Delay(self,0.5)
            self:K2_EndAbility()
        end
    ))
end
return M
