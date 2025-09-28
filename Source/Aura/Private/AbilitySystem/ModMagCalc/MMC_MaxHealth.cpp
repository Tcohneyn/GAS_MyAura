// Tcohneyn All Rights Reserved


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
    // 定义需要捕获的属性为 "Vigor"（体力/精力）。
    VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();

    // 设置捕获来源为目标（Target），也就是这个效果施加在谁身上，就从谁那里取 Vigor。
    VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

    // 设置为 false，表示不是快照（Snapshot）。
    // 意味着每次计算时都会动态获取 Vigor 的当前值，而不是应用时固定住。
    VigorDef.bSnapshot = false;

    // 把这个捕获定义添加进 RelevantAttributesToCapture，
    // 确保之后计算时会捕获并提供该属性值。
    RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    // 从源目标和目标收集标签
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    // 构建属性评估参数，传入 Source 和 Target 的标签。
    // 后续在计算捕获属性时，可能会参考这些标签。
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    // 初始化 Vigor 值。
    float Vigor = 0.f;

    // 获取被捕获的 Vigor 值，并存入 Vigor 变量。
    // 计算时会考虑前面传入的 EvaluationParameters。
    GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);

    // 确保 Vigor 不小于 0，避免出现负数导致数值计算异常。
    Vigor = FMath::Max<float>(Vigor, 0.f);

    // 从 GameplayEffectSpec 的上下文中获取 SourceObject，并尝试转换为 ICombatInterface。
    // 这样就能访问施法者/玩家的战斗接口，获取其等级等信息。
    ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());

    // 通过接口获取玩家等级，用于计算最大生命值。
    const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

    // 最终返回计算后的最大生命值：
    // 基础值 80 + (2.5 * Vigor) + (10 * 玩家等级)。
    return 80.f + 2.5f * Vigor + 10.f * PlayerLevel;
}