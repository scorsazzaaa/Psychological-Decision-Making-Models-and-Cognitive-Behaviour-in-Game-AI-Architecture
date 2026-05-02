# Run this in UE5 Python Script Editor to create all Blueprint assets
# Editor -> Scripting -> Run Python Script

import unreal

def create_blueprint(asset_path, parent_class):
    """Create a Blueprint asset at the given path"""
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("ParentClass", parent_class)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    new_asset = asset_tools.create_asset(
        asset_path.split("/")[-1],
        "/".join(asset_path.split("/")[:-1]),
        None,
        factory
    )
    return new_asset

def create_behavior_tree(asset_path):
    """Create a Behavior Tree asset"""
    factory = unreal.BehaviorTreeFactory()
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    new_asset = asset_tools.create_asset(
        asset_path.split("/")[-1],
        "/".join(asset_path.split("/")[:-1]),
        unreal.BehaviorTree,
        factory
    )
    return new_asset

def create_blackboard(asset_path):
    """Create a Blackboard asset"""
    factory = unreal.BlackboardDataFactory()
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    new_asset = asset_tools.create_asset(
        asset_path.split("/")[-1],
        "/".join(asset_path.split("/")[:-1]),
        unreal.BlackboardData,
        factory
    )
    return new_asset

def create_widget_blueprint(asset_path):
    """Create a Widget Blueprint asset"""
    factory = unreal.WidgetBlueprintFactory()
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    new_asset = asset_tools.create_asset(
        asset_path.split("/")[-1],
        "/".join(asset_path.split("/")[:-1]),
        unreal.WidgetBlueprint,
        factory
    )
    return new_asset

print("Starting AdaptiveAI Blueprint creation...")

# Find parent classes
adaptive_ai_controller = unreal.load_class(None, "/Script/AdaptiveAI.AdaptiveAIController")
aggressive_npc = unreal.load_class(None, "/Script/AdaptiveAI.AggressiveNPC")
fearful_npc = unreal.load_class(None, "/Script/AdaptiveAI.FearfulNPC")
balanced_npc = unreal.load_class(None, "/Script/AdaptiveAI.BalancedNPC")
ai_state_widget = unreal.load_class(None, "/Script/AdaptiveAI.AIStateWidget")

# Create NPC Blueprints
print("Creating NPC Blueprints...")
if aggressive_npc:
    bp_aggressive = create_blueprint("/Game/AdaptiveAI/Blueprints/BP_AggressiveNPC", aggressive_npc)
    print(f"Created: {bp_aggressive.get_path_name()}")

if fearful_npc:
    bp_fearful = create_blueprint("/Game/AdaptiveAI/Blueprints/BP_FearfulNPC", fearful_npc)
    print(f"Created: {bp_fearful.get_path_name()}")

if balanced_npc:
    bp_balanced = create_blueprint("/Game/AdaptiveAI/Blueprints/BP_BalancedNPC", balanced_npc)
    print(f"Created: {bp_balanced.get_path_name()}")

# Create Behavior Tree and Blackboard
print("Creating Behavior Tree and Blackboard...")
try:
    bb = create_blackboard("/Game/AdaptiveAI/BehaviorTrees/BB_AdaptiveAI")
    print(f"Created: {bb.get_path_name()}")
except Exception as e:
    print(f"Blackboard creation skipped: {e}")

try:
    bt = create_behavior_tree("/Game/AdaptiveAI/BehaviorTrees/BT_AdaptiveAI")
    print(f"Created: {bt.get_path_name()}")
except Exception as e:
    print(f"Behavior Tree creation skipped: {e}")

# Create Debug Widget
print("Creating Debug Widget...")
try:
    widget = create_widget_blueprint("/Game/AdaptiveAI/UI/WBP_AIStateDebug")
    print(f"Created: {widget.get_path_name()}")
except Exception as e:
    print(f"Widget creation skipped: {e}")

print("\nSetup complete!")
print("\nNext steps:")
print("1. Open BB_AdaptiveAI and add these keys:")
print("   - SelfActor (Object)")
print("   - EnemyActor (Object)")
print("   - Health (Float)")
print("   - Fear (Float)")
print("   - Confidence (Float)")
print("   - SelectedAction (Enum - ERLAction)")
print("   - bIsInCombat (Bool)")
print("\n2. Open BT_AdaptiveAI and build your tree")
print("3. Assign BB and BT to your NPC Blueprints")
print("4. Place NPCs in level and test!")
