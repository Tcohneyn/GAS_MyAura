--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_TextValueRow_C
local M = UnLua.Class()

function M:SetLabelText(LabelText)
    self.TextBlock_Label:SetText(LabelText)
end

function M:SetNumericalValueInt(NewValue)
    self.WBP_FramedValue.TextBlock_Value:SetText(UE.UKismetTextLibrary.Conv_IntToText(NewValue))
end
return M
