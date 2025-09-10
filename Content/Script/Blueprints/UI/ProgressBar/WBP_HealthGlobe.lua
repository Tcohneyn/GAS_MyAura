--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_HealthGlobe_C
local M = UnLua.Class("Blueprints.UI.ProgressBar.WBP_GlobeProgressBar")

function M:WidgetControllerSet()
    --设置组件控制器
    local OverlayController = self.WidgetController:Cast(UE.UBP_OverlayWidgetController_C)
    OverlayController.OnHealthChanged:Add(self, M.OnHealthChanged)
    OverlayController.OnMaxHealthChanged:Add(self, M.OnMaxHealthChanged)
end
--绑定血量变化事件
function M:OnHealthChanged(NewHealth)
    self.CurrentHealth = NewHealth
    self:SetProgressBarPercent(UE.UKismetMathLibrary.SafeDivide(self.CurrentHealth, self.MaxHealth))
end
--绑定最大血量变化事件
function M:OnMaxHealthChanged(NewMaxHealth)
    self.MaxHealth = NewMaxHealth
    self:SetProgressBarPercent(UE.UKismetMathLibrary.SafeDivide(self.CurrentHealth, self.MaxHealth))
end

return M
