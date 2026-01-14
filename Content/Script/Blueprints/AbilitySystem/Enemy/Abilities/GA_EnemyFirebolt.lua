--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type GA_EnemyFirebolt_C
local M = UnLua.Class("Blueprints.AbilitySystem.Enemy.Abilities.GA_RangedAttack")
function M:K2_ActivateAbility()
    self.Super.K2_ActivateAbility(self)
end

function M:OnMontage()
    self.Super.OnMontage(self)
end

function M:OnEventReceived(Payload)
    self.Super.OnEventReceived(self, Payload)
end
return M