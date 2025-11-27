--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_Goblin_Spear_C
local M = UnLua.Class("Blueprints.Character.Enemy.BP_EnemyBase")

function M:StartDissolveTimeline(DynamicMaterialInstance)
   self.Super.StartDissolveTimeline(self, DynamicMaterialInstance)
end

function M:StartWeaponDissolveTimeline(DynamicMaterialInstance)
    self.Super.StartWeaponDissolveTimeline(self, DynamicMaterialInstance)
end
return M
