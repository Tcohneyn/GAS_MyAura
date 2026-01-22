--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_SpellGlobe_C
local M = UnLua.Class()

function M:WidgetControllerSet()
    --Set Widget Controller
    self.BPOverlayWidgetController = self.WidgetController:Cast(UE.UBP_OverlayWidgetController_C)
    --Bind to Ability Info Delegate
    self.BPOverlayWidgetController.AbilityInfoDelegate:Add(self, M.ReceiveAbilityInfo)
end

function M:ReceiveAbilityInfo(AbilityInfo)
    if not AbilityInfo then
        print("AbilityInfo is nil")
        return
    end
    if UE.UBlueprintGameplayTagLibrary.MatchesTag(self.InputTag, AbilityInfo.InputTag, true) then
        self.CooldownTag = AbilityInfo.CooldownTag
        self:SetSlateBrush(AbilityInfo.Icon, AbilityInfo.BackgroundMaterial)
        self:ListenCooldownChange()
    end
end

function M:ListenCooldownChange()
    local ASC = self.BPOverlayWidgetController.AbilitySystemComponent
    if ASC then
        if self.Task then
            self.Task:EndTask()
        end
        self.Task = UE.UWaitCooldownChange.WaitForCooldownChange(ASC, self.CooldownTag)
        self.Task.CooldownStart:Add(self, M.OnCooldownStart)
        self.Task.CooldownEnd:Add(self, M.OnCooldownEnd)
    end
end

function M:OnCooldownStart(TimeRemaining)
    --UE.UKismetSystemLibrary.PrintString(self, "Cooldown Started:" .. TimeRemaining)
    self:HandleCooldownTimer(TimeRemaining)
end

function M:OnCooldownEnd(TimeRemaining)
    --UE.UKismetSystemLibrary.PrintString(self, "Cooldown Ended:" .. TimeRemaining)
    self:HandleCooldownTimer(TimeRemaining)
end

function M:HandleCooldownTimer(TimeRemaining)
    self:SetCooldownState()
    self.CooldownTimerHandle = UE.UKismetSystemLibrary.K2_SetTimerDelegate({ self, self.UpdateTimer },
        self.TimerFrequency, true)
    self.TimeRemaining = TimeRemaining
end

function M:UpdateTimer()
    self.TimeRemaining = self.TimeRemaining - self.TimerFrequency
    local returnval = UE.UKismetMathLibrary.FClamp(self.TimeRemaining, 0, self.TimeRemaining)
    self.Text_Cooldown:SetText(UE.UKismetTextLibrary.Conv_DoubleToText(returnval, UE.ERoundingMode.HalfToEven, false,
        true, 1, 324, 1, 1))
    if self.TimeRemaining <= 0 then
        UE.UKismetSystemLibrary.K2_ClearAndInvalidateTimerHandle(self, self.CooldownTimerHandle)
        self:SetDefaultState()
    end
end

-- function M:SetIconandBackground(IconBrush, BackgroundBrush)
--     self.Image_SpellIcon:SetBrush(IconBrush)
--     self.Image_Background:SetBrush(BackgroundBrush)
-- end
return M
