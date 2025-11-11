--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_EnemyHealthBar_C
local M = UnLua.Class("Blueprints.UI.ProgressBar.WBP_ProgressBar")

function M:WidgetControllerSet()
   self:Sequence()
end
function M:Sequence()
   self:Task1()
end

function M:Task1()
   local BP_EnemyBase = self.WidgetController:Cast(UE.ABP_EnemyBase_C)
   BP_EnemyBase.OnHealthChanged:Add(self, M.OnHealthChanged)
   BP_EnemyBase.OnMaxHealthChanged:Add(self, M.OnMaxHealthChanged)
end

function M:OnHealthChanged(NewValue)
    self.Health = NewValue
    --UE.UKismetSystemLibrary.PrintString(self, "EnemyHealthBar Health:"..tostring(self.Health))
    self:SetBarPercent(self.Health, self.MaxHealth)
end

function M:OnMaxHealthChanged(NewValue)
    self.MaxHealth = NewValue
    self:SetBarPercent(self.Health, self.MaxHealth)
end

function M:Tick(MyGeometry, InDeltaTime)
   self.Super.Tick(self, MyGeometry, InDeltaTime)
end

function M:SetBarPercent(Health,MaxHealth)
   self.Super.SetBarPercent(self, Health,MaxHealth)
end
return M
