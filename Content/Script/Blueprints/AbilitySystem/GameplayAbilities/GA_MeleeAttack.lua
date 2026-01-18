--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type GA_MeleeAttack_C
local M = UnLua.Class()

function M:K2_ActivateAbility()
    local AvatarActor = self:GetAvatarActorFromActorInfo()
    local CombatTarget = AvatarActor:GetCombatTarget()
    AvatarActor:Cast(UE.UCombatInterface).UpdateFacingTarget(CombatTarget:K2_GetActorLocation())
    self.HasAttackedMontage = false
    local FTaggedMontage = self:GetFTaggedMontage(AvatarActor)
    if self.HasAttackedMontage then
        local PlayMontageTask = UE.UAbilityTask_PlayMontageAndWait.CreatePlayMontageAndWaitProxy(self, "PlayMontageTask",
            FTaggedMontage.Montage)
        PlayMontageTask.OnCompleted:Add(self, M.OnMontage)
        PlayMontageTask.OnInterrupted:Add(self, M.OnMontage)
        PlayMontageTask.OnCancelled:Add(self, M.OnMontage)
        PlayMontageTask:ReadyForActivation()
        local WaitEventTask = UE.UAbilityTask_WaitGameplayEvent.WaitGameplayEvent(self, FTaggedMontage.MontageTag, nil,
            false, true)
        WaitEventTask.EventReceived:Add(self, M.OnEventReceived)
        WaitEventTask:ReadyForActivation()
    else
        self:K2_EndAbility()
    end
end

function M:OnMontage()
    self:K2_EndAbility()
end

function M:OnEventReceived(Payload)
    local AvatarActor = self:GetAvatarActorFromActorInfo()
    local FTaggedMontage = self:GetFTaggedMontage(AvatarActor)
    local CombatSocketLocation = AvatarActor:GetCombatSocketLocation(FTaggedMontage.SocketTag)
    local OutOverlappingActors = UE.UAuraAbilitySystemLibrary.GetLivePlayersWithinRadius(AvatarActor, AvatarActor, 45.0,
        CombatSocketLocation)
    for i = 1, OutOverlappingActors:Num() do
        local Actor = OutOverlappingActors:Get(i) -- UnLua 中 TArray 的 Get 索引通常从 1 开始
        if Actor then
            -- 这里相当于蓝图循环中的 Loop Body
            if UE.UAuraAbilitySystemLibrary.IsNotFriend(AvatarActor, Actor) then
                print("检测到目标: " .. Actor:GetName())
                self:CauseDamage(Actor)
                UE.UKismetSystemLibrary.DrawDebugSphere(self, Actor:K2_GetActorLocation(), 15, 12, self.LineColor, 3.0,
                1.0)
                self.HasHitTarget = true
            end
        end
    end
    if self.HasHitTarget then
        local params = UE.FGameplayCueParameters()
        params.Location = CombatSocketLocation
        params.SourceObject = AvatarActor:GetCombatTarget()
        params.EffectCauser = AvatarActor
        params.AggregatedSourceTags = UE.UBlueprintGameplayTagLibrary.MakeGameplayTagContainerFromTag(FTaggedMontage.MontageTag)
        self:K2_ExecuteGameplayCueWithParams(self.GameplayCueTag,params)
        self:K2_EndAbility()
    else
        self:K2_EndAbility()
    end
end

function M:GetFTaggedMontage(AvatarActor)
    local AttackMontages = AvatarActor:GetAttackMontages()
    local length = AttackMontages:Num()
    if length > 0 then
        self.HasAttackedMontage = true
    else
        return nil
    end
    local choice = math.random(1, length)
    local AttackMontage = AttackMontages[choice]
    return AttackMontage
end

return M
