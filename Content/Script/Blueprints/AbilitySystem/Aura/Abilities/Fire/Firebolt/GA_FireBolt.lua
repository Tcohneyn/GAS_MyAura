--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type GA_FireBolt_C
local M = UnLua.Class()
function M:K2_ActivateAbility()
    --UE.UKismetSystemLibrary.PrintString(self, self:GetAbilityLevel())
    local value = self:K2_CommitAbility()
    --print("Commit Ability:", value)
    local Task = UE.UTargetDataUnderMouse.CreateTargetDataUnderMouse(self)
    Task.ValidData:Add(self, M.OnTargetDataReady)
    Task:ReadyForActivation()
    local PlayMontageTask = UE.UAbilityTask_PlayMontageAndWait.CreatePlayMontageAndWaitProxy(self, "PlayMontageTask",
        self.MontagetoPlay,1.0,"",false)
    PlayMontageTask.OnCompleted:Add(self, M.OnMontage)
    PlayMontageTask.OnBlendOut:Add(self, M.OnMontage)
    PlayMontageTask.OnInterrupted:Add(self, M.OnMontage)
    PlayMontageTask.OnCancelled:Add(self, M.OnMontage)
    PlayMontageTask:ReadyForActivation()
    local WaitEventTask = UE.UAbilityTask_WaitGameplayEvent.WaitGameplayEvent(self, self.EventTag)
    WaitEventTask.EventReceived:Add(self, M.OnEventReceived)
    WaitEventTask:ReadyForActivation()
end

function M:OnMontage()
    self:K2_EndAbility()
end

function M:OnTargetDataReady(DataHandle)
    local HitResult = UE.UAbilitySystemBlueprintLibrary.GetHitResultFromTargetData(DataHandle, 0)
    --UE.UKismetSystemLibrary.DrawDebugSphere(self, HitResult.Location, 5, 12, UE.FLinearColor.Green, 5.0, 1.0)
    self.Location = HitResult.Location
    local Interface = self:GetAvatarActorFromActorInfo():Cast(UE.UCombatInterface)
    Interface:UpdateFacingTarget(HitResult.Location)
end

function M:OnEventReceived(Payload)
    --UE.UKismetSystemLibrary.PrintString(self, "Firebolt Hit Event Received")
    self:SpawnProjectile(self.Location, self.SocketTag)
end

return M
