--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_AttributeMenu_C
local M = UnLua.Class()

function M:Construct()
   self.ClosedButton.Button.OnClicked:Add(self, M.OnClosedButtonClicked)
   self:Sequnece()
end

function M:Sequnece()
   self:Task1();
   self:Task2();
end

function M:Task1()
   self:SetAttributeTags()
end

function M:Task2()
   local AMController = UE.UAuraAbilitySystemLibrary.GetAttributeMenuWidgetController(self)
   self:SetWidgetController(AMController)
   AMController:BroadcastInitialValues()
end

function M:OnClosedButtonClicked()
   --self.CloseButton.Button:SetIsEnabled(false)
   self:RemoveFromParent()
end

function M:Destruct()
   self.AttributeMenuClosed:Broadcast()
end

function M:SetAttributeTags()
    -- 获取 GameplayTagsManager 单例
    local GameplayTagsManager = UE.UAuraAbilitySystemLibrary.GetTag
   self.Row_Strength.AttributeTag = GameplayTagsManager("Attributes.Primary.Strength")
   self.Row_Intelligence.AttributeTag = GameplayTagsManager("Attributes.Primary.Intelligence")
   self.Row_Resilience.AttributeTag = GameplayTagsManager("Attributes.Primary.Resilience")
   self.Row_Vigor.AttributeTag = GameplayTagsManager("Attributes.Primary.Vigor")
   --派生属性
   self.Row_Armor.AttributeTag = GameplayTagsManager("Attributes.Secondary.Armor")
   self.Row_ArmorPenetration.AttributeTag = GameplayTagsManager("Attributes.Secondary.ArmorPenetration")
   self.Row_BlockChance.AttributeTag = GameplayTagsManager("Attributes.Secondary.BlockChance")
   self.Row_CriticalHitChance.AttributeTag = GameplayTagsManager("Attributes.Secondary.CriticalHitChance")
   self.Row_CriticalHitDamage.AttributeTag = GameplayTagsManager("Attributes.Secondary.CriticalHitDamage")
   self.Row_CriticalHitResistance.AttributeTag = GameplayTagsManager("Attributes.Secondary.CriticalHitResistance")
   self.Row_HealthRegeneration.AttributeTag = GameplayTagsManager("Attributes.Secondary.HealthRegeneration")
   self.Row_ManaRegeneration.AttributeTag = GameplayTagsManager("Attributes.Secondary.ManaRegeneration")
   self.Row_MaxHealth.AttributeTag = GameplayTagsManager("Attributes.Secondary.MaxHealth")
   self.Row_MaxMana.AttributeTag = GameplayTagsManager("Attributes.Secondary.MaxMana")
end

-- function M:WidgetControllerSet()
--    local objectname = UE.UKismetSystemLibrary.GetObjectName( self.WidgetController)
--    UE.UKismetSystemLibrary.PrintString(self, "AttributeMenuWidgetControllerSet:" .. objectname)

-- end
return M
