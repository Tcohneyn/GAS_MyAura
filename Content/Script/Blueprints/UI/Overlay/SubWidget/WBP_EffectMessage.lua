--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_EffectMessage_C
local M = UnLua.Class()

function M:SetImageandText(Image, Text)
    self.TextBlock_Message:SetText(Text)
    self.Image_Icon:SetBrushFromTexture(Image, false)
    self:PlayAnimation(self.Text_Message, 0, 1, UE.EUMGSequencePlayMode.Forward, 1.0)
    self:DestroyDelay()
end

function M:DestroyDelay()
    coroutine.resume(coroutine.create(
        function()
            UE.UKismetSystemLibrary.Delay(self, 0.85)
            self:RemoveFromParent()
        end
    ))
end

return M
