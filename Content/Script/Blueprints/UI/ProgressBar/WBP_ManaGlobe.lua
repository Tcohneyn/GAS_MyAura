--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_ManaGlobe_C
local M = UnLua.Class()

function M:WidgetControllerSet()
    --设置组件控制器
    local OverlayController = self.WidgetController:Cast(UE.UBP_OverlayWidgetController_C)
    OverlayController.OnManaChanged:Add(self, M.OnManaChanged)
    OverlayController.OnMaxManaChanged:Add(self, M.OnMaxManaChanged)
end
--绑定魔力变化事件
function M:OnManaChanged(NewMana)
    self.CurrentMana = NewMana
    self:SetProgressBarPercent(UE.UKismetMathLibrary.SafeDivide(self.CurrentMana, self.MaxMana))
end
--绑定最大魔力变化事件
function M:OnMaxManaChanged(NewMaxMana)
    self.MaxMana = NewMaxMana
    self:SetProgressBarPercent(UE.UKismetMathLibrary.SafeDivide(self.CurrentMana, self.MaxMana))
end

return M
