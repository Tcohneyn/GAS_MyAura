// Tcohneyn All Rights Reserved


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTagsController.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
// 定义一个结构体来保存我们需要捕获（Capture）的属性定义
// 这是一个标准的 GAS 模式，用于避免在计算函数中重复定义属性 
struct AuraDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
    DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
    
    DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
	
    TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
    AuraDamageStatics()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);

        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

        const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);

        TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, LightningResistanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
    }
};
// 创建一个静态单例访问器，确保属性定义只被创建一次
static const AuraDamageStatics& DamageStatics()
{
    static AuraDamageStatics DStatics;
    return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
    // 告诉 GAS 系统，在这个类运行时，我们需要捕获 Armor 属性
    // 如果不加这一行，Execute_Implementation 中将无法获取到 Armor 的数值
    RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
    RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);

    RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}
// 核心执行函数：所有的自定义计算逻辑都在这里发生
void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    // 1. 获取 Ability System Component (ASC)
    // Source: 施放技能的人
    // Target: 承受技能的人
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    // 2. 获取 Avatar Actor (拥有 ASC 的实际 Actor，例如 Character)
    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
    ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
    ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

    // 3. 获取 GameplayEffectSpec (包含这次效果的所有信息，如等级、Context 等)
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    // 4. 收集 Gameplay Tags (用于处理基于 Tag 的逻辑，比如"如果目标有 'Debuff.Vulnerable' 标签，则伤害翻倍")
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    
    // 创建评估参数，将 Tag 传递进去
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    //Get Damage Set by Caller Magnitude
    // 获取由调用者设置的伤害值
    float Damage = 0.f;
    for (const TTuple<FGameplayTag, FGameplayTag>& Pair  : FAuraGameplayTags::Get().DamageTypesToResistances)
    {
        //const float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key);
        const FGameplayTag DamageTypeTag = Pair.Key;
        const FGameplayTag ResistanceTag = Pair.Value;
		
        checkf(AuraDamageStatics().TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());
        const FGameplayEffectAttributeCaptureDefinition CaptureDef = AuraDamageStatics().TagsToCaptureDefs[ResistanceTag];

        float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);
		
        float Resistance = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
        Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

        DamageTypeValue *= ( 100.f - Resistance ) / 100.f;
        Damage += DamageTypeValue;
    }
    
    // Capture BlockChance on Target, and determine if there was a successful Block
    // 捕获目标的格挡几率，并判定是否成功格挡
    // If Block, halve the damage.
    // 若成功格挡，则伤害减半

    float TargetBlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
    // 将目标格挡几率限制为不小于0
    TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

    const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
    
    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	
    UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
    // If Block, halve the damage.	
    Damage = bBlocked ? Damage / 2.f : Damage;

    float TargetArmor = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
    TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

    float SourceArmorPenetration = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
    SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

    const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
    // 查找曲线 (Find Curve)
    // 访问数据资产中存储的 DamageCalculationCoefficients (这是一个 UCurveTable 指针)。
    // 使用 FindCurve 函数，通过行名 FName("ArmorPenetration") 查找对应的曲线数据。
    // FString() 是 ContextString，这里传空字符串即可。
    const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
    // 评估曲线获取系数 (Evaluate)
    // SourceCombatInterface->GetPlayerLevel()：通过接口获取攻击者（Source）当前的等级。
    // ArmorPenetrationCurve->Eval(...)：将等级作为 X轴 (Time) 输入，计算出曲线上的 Y轴 (Value) 值。
    // 最终得到 ArmorPenetrationCoefficient（护甲穿透系数），例如 1-10级返回 0.25，10-20级返回 0.15。
    const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());
    
    // ArmorPenetration ignores a percentage of the Target's Armor.
    // 护甲穿透能无视目标一定百分比的护甲。
    const float EffectiveArmor = TargetArmor * ( 100 - SourceArmorPenetration * ArmorPenetrationCoefficient ) / 100.f;

    const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
    const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
    // Armor ignores a percentage of incoming Damage.
    // 护甲能减免一定百分比的所受伤害。
    Damage *= ( 100 - EffectiveArmor * EffectiveArmorCoefficient ) / 100.f;


    float SourceCriticalHitChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
    SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);
	
    float TargetCriticalHitResistance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
    TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

    float SourceCriticalHitDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
    SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

    const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
    const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel());

    // Critical Hit Resistance reduces Critical Hit Chance by a certain percentage
    // 暴击抗性会按一定百分比降低暴击几率
    const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
    const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

    UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
    
    // Double damage plus a bonus if critical hit
    // 如果发生暴击，则造成双倍伤害加上额外加成
    Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;
    
    const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
    
    // 将修改应用回目标
    OutExecutionOutput.AddOutputModifier(EvaluatedData);
}