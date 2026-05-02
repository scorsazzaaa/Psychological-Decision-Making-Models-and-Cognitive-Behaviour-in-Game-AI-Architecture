# Manual Blueprint Setup (No Python Required)

The folders are empty because Blueprint assets must be created inside UE5. Here's how:

## Step 1: Create NPC Blueprints

### BP_AggressiveNPC
1. In Content Browser, navigate to `AdaptiveAI/Blueprints`
2. Right-click empty space → Blueprint Class
3. In "All Classes" search box, type: **Aggressive**
4. Select **Aggressive NPC** → Click Select
5. Name: `BP_AggressiveNPC`
6. Press Enter

### Configure the Blueprint
Double-click `BP_AggressiveNPC` to open it:

**In the Class Defaults panel (left side):**
```
AI Controller Class: AdaptiveAIController
Auto Possess AI: PlacedInWorldOrSpawned
```

**Compile and Save** (Top toolbar: Compile → Save)

### Repeat for other archetypes:
- `BP_FearfulNPC` (parent: Fearful NPC)
- `BP_BalancedNPC` (parent: Balanced NPC)

---

## Step 2: Create Blackboard

1. Right-click in `AdaptiveAI/BehaviorTrees` → Artificial Intelligence → Blackboard
2. Name: `BB_AdaptiveAI`
3. Double-click to open

**Add these keys (Click "New Key" for each):**

| Key Name | Key Type | Details |
|----------|----------|---------|
| SelfActor | Object | Base Class: Actor |
| EnemyActor | Object | Base Class: Actor |
| Health | Float | |
| Fear | Float | |
| Confidence | Float | |
| SelectedAction | Enum | Enum Name: ERLAction |
| bIsInCombat | Bool | |

**Save** the Blackboard

---

## Step 3: Create Behavior Tree

1. Right-click in `AdaptiveAI/BehaviorTrees` → Artificial Intelligence → Behavior Tree
2. Name: `BT_AdaptiveAI`
3. Double-click to open

**Important:** When the Behavior Tree Editor opens:
1. Look at the Toolbar → "Blackboard" dropdown
2. Select `BB_AdaptiveAI` from the list

**Build the tree:**

1. Drag from Root node → Add Node → Selector
2. Drag from Selector → Add Node → Sequence
3. On the Sequence:
   - Right-click → Add Service → Search "Adaptive AI Service"
   - Set Service Mode: Update State
   - Set Update Interval: 0.5
4. Drag from Sequence → Add Node → Task
   - Search "Adaptive Select Action"
   - Set Selection Mode: RLBased
5. Drag from Sequence → Add Node → Task
   - Search "Wait"
   - Set Wait Time: 0.5

**Your tree should look like:**
```
Root
└── Selector
    └── Sequence [Service: Adaptive AI Service]
        ├── Task: Adaptive Select Action
        └── Task: Wait
```

**Compile and Save**

---

## Step 4: Assign BT to NPCs

Go back to each NPC Blueprint (BP_AggressiveNPC, etc.):

In Class Defaults:
```
Behavior Tree: BT_AdaptiveAI
Blackboard Data: BB_AdaptiveAI
```

Compile → Save

---

## Step 5: Test It

1. Open a level (Content/ThirdPerson/Maps/ThirdPersonMap)
2. Drag `BP_AggressiveNPC` into the world
3. Press Play
4. Walk up to the NPC
5. It should react!

---

## Optional: Debug Widget

### Create Widget
1. Right-click in `AdaptiveAI/UI` → User Interface → Widget Blueprint
2. Name: `WBP_AIStateDebug`

### Add UI Elements
In the Designer tab:

1. **Text** widget:
   - Content: "Archetype:"
   - Name: ArchetypeText

2. **Progress Bar**:
   - Name: FearBar
   - Fill Color: Red

3. **Progress Bar**:
   - Name: ConfidenceBar
   - Fill Color: Green

4. **Text** widgets for:
   - ModeText
   - ActionText
   - MemoryText
   - LearningText

### Assign to NPC
In BP_AggressiveNPC:
1. Switch to Event Graph
2. Add Event BeginPlay
3. Drag off → Search "Toggle Debug Visualization"
4. Check "Visible"
5. Compile → Save

---

## Troubleshooting

### "Can't find Aggressive NPC class"
**Solution:** The C++ hasn't been compiled yet. Build the project first (Development Editor).

### "Blackboard dropdown is empty"
**Solution:** Create the Blackboard first, then reopen the Behavior Tree.

### "Service/Task not found"
**Solution:** These are C++ classes. Make sure you've built the project.

### Tree doesn't do anything
**Solution:** Check that:
1. AI Controller is set
2. Auto Possess is enabled
3. Behavior Tree is assigned
4. You've compiled and saved everything

---

## Quick Checklist

- [ ] Built project (Development Editor)
- [ ] Created BP_AggressiveNPC
- [ ] Created BB_AdaptiveAI (with 7 keys)
- [ ] Created BT_AdaptiveAI (linked to BB)
- [ ] Assigned BT and BB to NPC
- [ ] Placed NPC in level
- [ ] Pressed Play

Once built, you'll see the AdaptiveAI classes in the Blueprint creation dialog.
