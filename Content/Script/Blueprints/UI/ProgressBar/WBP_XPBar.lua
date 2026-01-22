--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_XPBar_C
local M = UnLua.Class()

function M:WidgetControllerSet()
    --设置组件控制器
    local OverlayController = self.WidgetController:Cast(UE.UBP_OverlayWidgetController_C)
    OverlayController.OnXPPercentChangedDelegate:Add(self, M.OnXPPercentChangedDelegate_Event)
end

function M:OnXPPercentChangedDelegate_Event(NewValue)
    self.ProgressBar_XP:SetPercent(NewValue)
end
return M
