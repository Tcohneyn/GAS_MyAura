--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_ProgressBar_C
local M = UnLua.Class()

function M:Construct()
    self:HideProgressBar()
end

function M:Tick(MyGeometry, InDeltaTime)
    if self.ProgressBarVisible then
        --print("CurrentPercent",self.ProgressBar_Ghost.Percent)
        --print("GhostPercentTarget",self.GhostPercentTarget)
        local value = UE.UKismetMathLibrary.FInterpTo(self.ProgressBar_Ghost.Percent, self.GhostPercentTarget,
            InDeltaTime,
            1.0)
        --print("value",value)
        self.ProgressBar_Ghost:SetPercent(value)
        --UE.UKismetSystemLibrary.PrintString(self, "ProgressBar Percent:"..tostring(value))
    end
end

function M:UpdateGhostInterpTarget(Target)
    UE.UKismetSystemLibrary.K2_ClearAndInvalidateTimerHandle(self, self.HideTimer)
    self.HideTimer = UE.UKismetSystemLibrary.K2_SetTimerDelegate({ self, self.HideProgressBar }, 6.0, false)
    coroutine.resume(coroutine.create(
        function()
            UE.UKismetSystemLibrary.Delay(self, 1.0)
            self.GhostPercentTarget = Target
        end
    ))
end

function M:SetBarPercent(Health, MaxHealth)
    self:SetBarVisibility(true)
    local Target = UE.UKismetMathLibrary.SafeDivide(Health, MaxHealth)
    self.ProgressBar_Front:SetPercent(Target)
    self:UpdateGhostInterpTarget(Target)
end

function M:HideProgressBar()
   self:SetBarVisibility(false)
end

function M:SetBarVisibility(vi)
    self.ProgressBarVisible = vi
    if self.ProgressBarVisible then
        self:SetVisibility(UE.ESlateVisibility.Visible)
    else
        self:SetVisibility(UE.ESlateVisibility.Hidden)
    end
end

return M
