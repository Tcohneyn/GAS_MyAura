--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type GA_SummonAbility_C
local M = UnLua.Class()
function M:K2_ActivateAbility()
    -- 初始化位置和索引
    self.SpawnLocations = self:GetSpawnLocations()
    UE.UAuraAbilitySystemLibrary.ShuffleSpawnLocations(self.SpawnLocations)
    self.CurrentSpawnIndex = 1 -- Lua 索引从 1 开始
    for i = 1, self.SpawnLocations:Num() do
        UE.UNiagaraFunctionLibrary.SpawnSystemAtLocation(
            self,
            self.SummonEffect,
            self.SpawnLocations:Get(i),
            UE.FRotator(0, 0, 0),
            UE.FVector(1, 1, 1),
            true,
            true,
            UE.ENCPoolMethod.AutoRelease,
            true
        )
    end
    local PlayMontageTask = UE.UAbilityTask_PlayMontageAndWait.CreatePlayMontageAndWaitProxy(self, "PlayMontageTask",
        self.MontageToPlay)
    PlayMontageTask.OnCompleted:Add(self, M.OnMontage)
    PlayMontageTask.OnInterrupted:Add(self, M.OnMontage)
    PlayMontageTask.OnCancelled:Add(self, M.OnMontage)
    PlayMontageTask:ReadyForActivation()
    local WaitEventTask = UE.UAbilityTask_WaitGameplayEvent.WaitGameplayEvent(self, self.EventTag, nil,
        false, true)
    WaitEventTask.EventReceived:Add(self, M.OnEventReceived)
    WaitEventTask:ReadyForActivation()
    -- coroutine.resume(coroutine.create(
    --     function()
    --         UE.UKismetSystemLibrary.Delay(self, 0.75)
    --         self:AsynchronousSpawnLoop()
    --     end
    -- ))
    -- 开始异步循环
end
function M:OnMontage()
    --self:K2_EndAbility()
end
function M:OnEventReceived(Payload)
    self:AsynchronousSpawnLoop()
end
function M:AsynchronousSpawnLoop()
    -- 1. 终止条件：索引超过数组长度
    if self.CurrentSpawnIndex > self.SpawnLocations:Num() then
        return
    end

    -- 2. 执行当前索引的任务
    local Location      = self.SpawnLocations:Get(self.CurrentSpawnIndex)
    --UE.UKismetSystemLibrary.DrawDebugSphere(self, Location, 25, 12, self.LineColor, 3.0, 1.0)
    -- 调用 SpawnActor
    local World         = self:GetWorld()
    -- 定义单独的值
    local AvatarActor = self:GetAvatarActorFromActorInfo()
    local Start = AvatarActor:k2_GetActorLocation()

    local SpawnLocation = Location + UE.FVector(0.0, 0.0, 70.0)
    local SpawnRotation = UE.UKismetMathLibrary.FindLookAtRotation(Start, Location)
    local SpawnScale    = UE.FVector(1.0, 1.0, 1.0)
    local Transform     = UE.UKismetMathLibrary.MakeTransform(SpawnLocation, SpawnRotation, SpawnScale)
    local NewActor      = World:SpawnActor(
        self:GetRandomMinionClass(),
        Transform,
        UE.ESpawnActorCollisionHandlingMethod.AlwaysSpawn,
        nil,
        nil
    )
    NewActor.OnDestroyed:Add(self, M.OnMinionDestroyed)
    NewActor:SpawnDefaultController()
    AvatarActor:IncremenetMinionCount(1)
    -- 3. 索引递增
    self.CurrentSpawnIndex = self.CurrentSpawnIndex + 1

    -- 4. 关键：设置定时器，在 0.2 秒后再次调用“自己”
    -- 使用定时器替代 for 循环，从而跨越帧率限制
    UE.UKismetSystemLibrary.K2_SetTimerDelegate({ self, self.AsynchronousSpawnLoop }, 0.2, false)
end

function M:OnMinionDestroyed(DestroyedActor)
    print("Minion Destroyed:" .. DestroyedActor:GetName())
    local AvatarActor = self:GetAvatarActorFromActorInfo()
    AvatarActor:IncremenetMinionCount(-1)
end
return M
