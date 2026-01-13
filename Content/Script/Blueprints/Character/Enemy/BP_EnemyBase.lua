--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_EnemyBase_C
local M = UnLua.Class()

function M:StartDissolveTimeline(DynamicMaterialInstance)
print("StartDissolveTimeline")
   self.DMI = DynamicMaterialInstance
   --self.BodyDissolveUpdate:Bind(self, M.OnDissolveUpdate)
   if self.DissolveCurve then
      self.BodyDissolveTimeline:AddInterpFloat(self.DissolveCurve, function()
      -- 1. 获取 Timeline 当前播放到了几秒
      local CurrentTime = self.BodyDissolveTimeline:GetPlaybackPosition()
      -- 2. 手动从曲线资源里，根据时间取值
      local val = self.DissolveCurve:GetFloatValue(CurrentTime)
      print("DissolveValue", val)
      if self.DMI then
         self.DMI:SetScalarParameterValue("Dissolve", val)
      end
   end)
end
self.BodyDissolveTimeline:Play()
self.BodyDissolveTimeline:SetLooping(false)
end

function M:StartWeaponDissolveTimeline(DynamicMaterialInstance)
   self.DMI1 = DynamicMaterialInstance
   --self.WeaponDissolveUpdate:Bind(self, M.OnDissolveUpdate1)
   if self.DissolveCurve and self.WeaponDissolveTimeline then
      self.WeaponDissolveTimeline:AddInterpFloat(self.DissolveCurve, function()
               -- 1. 获取 Timeline 当前播放到了几秒
         local CurrentTime = self.BodyDissolveTimeline:GetPlaybackPosition()
         -- 2. 手动从曲线资源里，根据时间取值
         local val = self.DissolveCurve:GetFloatValue(CurrentTime)
         if self.DMI1 then
            self.DMI1:SetScalarParameterValue("Dissolve", val)
         end
      end)
      self.WeaponDissolveTimeline:SetLooping(false)
      self.WeaponDissolveTimeline:Play()
   end
end

function M:UpdateFacingTarget(Target)
    --安全检查：如果组件不存在则退出
    if not self.MotionWarpingComponent then
        print("Error: MotionWarping component is nil!")
        return
    end
   self.MotionWarpingComponent:AddOrUpdateWarpTargetFromLocation("FaceTarget", Target)
end
-- function M:OnDissolveUpdate(DissolveValue)

-- end
-- function M:OnDissolveUpdate1(DissolveValue)

-- end
return M
