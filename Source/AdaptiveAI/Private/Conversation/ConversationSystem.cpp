#include "Conversation/ConversationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogConversationSystem, Log, All);

static const FString TopicIntros[3][3] =
{
    {
        TEXT("I've been going through some old records today. Found something that's been sitting with me — have you ever wondered what this place used to be like?"),
        TEXT("Funniest thing happened this morning. I noticed something that doesn't quite add up, and I've been turning it over ever since."),
        TEXT("I've been sitting here with the same thought going round in my head all morning. Do you ever just wonder how things ended up the way they are?")
    },
    {
        TEXT("I'll tell you something — there's been some odd movement around here lately. People I don't recognise, coming and going at strange hours."),
        TEXT("See that one over there? I've had my eye on them for a couple of days now. Something's off."),
        TEXT("It's been a strange few days around here, if I'm honest. Not sure if you've picked up on that.")
    },
    {
        TEXT("I actually just got something in that I think you'd find interesting. Didn't come easy to get hold of, either."),
        TEXT("You want to hear something? I had the most unusual customer in here earlier."),
        TEXT("I pick up a lot of things in this line of work — word gets around to me. Want to know what I've been hearing?")
    }
};

static const FString TopicLines[3][3][3][4] =
{
    {
        {
            {
                TEXT("Right? Most people don't even know this. Apparently there's a whole section of the archive nobody's opened in years."),
                TEXT("Exactly — and the more you dig in, the stranger it gets. Found a reference to something that really shouldn't be there."),
                TEXT("I thought the same thing. There's a pattern once you start looking, and it's hard to unsee."),
                TEXT("Makes you wonder what else we've just walked past without noticing.")
            },
            {
                TEXT("Fair enough. Not everyone finds old records fascinating. I get it."),
                TEXT("I suppose it's a bit niche. It just stuck with me, that's all."),
                TEXT("You'd be surprised — some of it reads more like a story than dry documentation."),
                TEXT("I'm not asking you to care about it. Just been on my mind.")
            },
            {
                TEXT("I know, I know. But hear me out — this isn't the usual dry stuff."),
                TEXT("I thought that too, at first. Then I actually read it."),
                TEXT("Can't make you find it interesting. But I genuinely think there's something there."),
                TEXT("Fair. I might be reading too much into it.")
            }
        },
        {
            {
                TEXT("That's what I mean — most people would've walked straight past it. I nearly did."),
                TEXT("Exactly. And it doesn't fit with anything else I know about this place."),
                TEXT("I knew you'd get it. It's been bothering me all day."),
                TEXT("Like there's something I should understand and I'm just not seeing it yet.")
            },
            {
                TEXT("Maybe I'm reading too much into it. Happens sometimes."),
                TEXT("You might be right. Could be nothing."),
                TEXT("I'll probably figure it out eventually. It's just one of those things."),
                TEXT("I tend to notice things other people don't, which is either useful or just annoying.")
            },
            {
                TEXT("I thought you'd say that. Maybe I am overthinking it."),
                TEXT("Could be. But something about it feels off to me."),
                TEXT("You're probably right. I can't quite shake it though."),
                TEXT("Fair enough. It's not exactly urgent.")
            }
        },
        {
            {
                TEXT("That's actually reassuring. I sometimes feel like I'm the only one who thinks about this sort of thing."),
                TEXT("Right. And the more I sit with it, the harder it is to just accept the simple answer."),
                TEXT("Exactly what I keep coming back to. I don't have a good answer either."),
                TEXT("Good to know it's not just me.")
            },
            {
                TEXT("Fair enough. Most people have more pressing things to deal with."),
                TEXT("I get it. Not exactly a practical question."),
                TEXT("I suppose I'm just built this way. Can't help it."),
                TEXT("You don't have to have an answer. Neither do I, honestly.")
            },
            {
                TEXT("Maybe. These things tend to matter eventually though."),
                TEXT("Probably overthinking again. Wouldn't be the first time."),
                TEXT("You might be right. I've been in my own head too much today."),
                TEXT("That's one way to look at it.")
            }
        }
    },
    {
        {
            {
                TEXT("Glad someone else has noticed. I was starting to think I was imagining it."),
                TEXT("Right. And it's not just once — it's a pattern. Same times, same routes."),
                TEXT("Exactly. Doesn't have to mean anything serious. But I'm keeping an eye on it."),
                TEXT("That's all I'm saying. Not jumping to conclusions. Just watching.")
            },
            {
                TEXT("Probably nothing. People pass through."),
                TEXT("Hope so. I've been wrong before by not paying attention."),
                TEXT("Maybe I'm reading too much into it. Hard to say."),
                TEXT("Just keep it in mind, that's all.")
            },
            {
                TEXT("I've been wrong before. Not saying it's definitely something."),
                TEXT("Probably right. Just being cautious."),
                TEXT("Fine. I'll keep it to myself next time."),
                TEXT("Not asking you to worry. Just stay aware.")
            }
        },
        {
            {
                TEXT("Right? Can't put my finger on it. But something's off."),
                TEXT("Exactly. Been around at odd times too. Doesn't add up."),
                TEXT("I thought the same. Keeping an eye on it."),
                TEXT("Good. Just wanted to know it wasn't only me.")
            },
            {
                TEXT("Maybe. I've been wrong before. But I trust my gut on this one."),
                TEXT("Could be nothing. I just notice these things."),
                TEXT("Not making any accusations. Just watching."),
                TEXT("Keep it in mind, that's all I'm saying.")
            },
            {
                TEXT("You might be right. I don't have anything concrete."),
                TEXT("I know how it sounds. I'm just watching."),
                TEXT("Fair. I'll drop it."),
                TEXT("Fine. Forget I mentioned it.")
            }
        },
        {
            {
                TEXT("Glad it's not just me. People have been on edge and nobody wants to say it."),
                TEXT("Right. And when it's been this quiet this long, something usually changes."),
                TEXT("Exactly. I'd rather be on guard and wrong than not ready."),
                TEXT("Better to be prepared.")
            },
            {
                TEXT("Could just be one of those weeks."),
                TEXT("Maybe I'm overthinking it. Wouldn't be the first time."),
                TEXT("Hard to know. Just feels off."),
                TEXT("We'll see. Could be nothing.")
            },
            {
                TEXT("Hope so. I'd rather be wrong about this."),
                TEXT("Might be right. I do tend to see trouble where there isn't any."),
                TEXT("Fair. Probably is nothing."),
                TEXT("Maybe. I'll ease up.")
            }
        }
    },
    {
        {
            {
                TEXT("Right? Figured you'd be interested. It's not the sort of thing that comes through here often."),
                TEXT("Exactly. That's why I held it back instead of just putting it straight out."),
                TEXT("Once you know what it actually is, it's hard to not want it."),
                TEXT("Good ear. Most people don't even think to ask.")
            },
            {
                TEXT("Fair enough. Not everyone's after the same thing."),
                TEXT("No pressure. It'll sell eventually. Just thought of you first."),
                TEXT("That's alright. Have a look around — something might catch your eye."),
                TEXT("Take your time. Always my advice.")
            },
            {
                TEXT("That's fair. I wouldn't expect you to take my word for it."),
                TEXT("I know — everything I say sounds like a sales pitch. I get it."),
                TEXT("Still, I don't push things I don't actually believe in."),
                TEXT("No hard feelings. Not everyone needs what I've got.")
            }
        },
        {
            {
                TEXT("Right, so — came in, wanted something very specific, wouldn't say what, just kept pointing."),
                TEXT("Exactly my reaction. I deal with unusual people but that one stood out."),
                TEXT("That's kind of what I thought too. Entertaining, if nothing else."),
                TEXT("Glad you agree. I couldn't stop laughing after they left.")
            },
            {
                TEXT("Maybe it was nothing. People are strange sometimes."),
                TEXT("I might be overthinking it. Just stuck in my head."),
                TEXT("Probably just one of those things. You see enough people, you start noticing."),
                TEXT("Might not mean anything. Just made me think.")
            },
            {
                TEXT("Fair enough. I tell a lot of stories — not all of them hold up."),
                TEXT("You might be right. I do tend to read into people too much."),
                TEXT("Maybe I'm making it more interesting than it was."),
                TEXT("Could've just been a weird day.")
            }
        },
        {
            {
                TEXT("Right — heard it from three different people now. That's usually a sign."),
                TEXT("Exactly. In my experience the boring-sounding rumours are the ones that turn out to be true."),
                TEXT("I thought so too. Interesting times."),
                TEXT("Whether it's true is another question, but it's spreading.")
            },
            {
                TEXT("Could be nothing. People talk."),
                TEXT("I pass along what I hear. What you do with it is up to you."),
                TEXT("I've heard stranger things that turned out to be nothing."),
                TEXT("Take it for what it is. Probably just chatter.")
            },
            {
                TEXT("Fair enough. I wouldn't put money on it either."),
                TEXT("You're probably right. Doesn't stop people talking though."),
                TEXT("Rumours tend to get bigger the further they travel."),
                TEXT("Sensible view. I just find it interesting.")
            }
        }
    }
};

static const FString Greetings[3][4] =
{
    {
        TEXT("Oh, a visitor. Come in, come in — don't get many of those."),
        TEXT("Ah, good timing. I was just about to take a break from all this."),
        TEXT("Hello there. Always good to see a new face around here."),
        TEXT("Oh! Caught me mid-thought. Not a problem, stay as long as you like.")
    },
    {
        TEXT("You there. I've been watching you."),
        TEXT("Strangers aren't welcome here. Remember that."),
        TEXT("I don't know you. That's a problem."),
        TEXT("Keep your distance. I mean it.")
    },
    {
        TEXT("Ah, a visitor! Or perhaps just a curious wanderer? Either way, welcome."),
        TEXT("Perfect timing — I was just thinking about a proposition."),
        TEXT("Come in, come in! Every visitor is a potential opportunity."),
        TEXT("Well hello there. What can I offer you today?")
    }
};

UConversationSystem::UConversationSystem()
    : Super()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    FMemory::Memzero(QTable, sizeof(QTable));
}

void UConversationSystem::BeginPlay()
{
    Super::BeginPlay();
    InitArchetype();
    InitQTable();

    UE_LOG(LogConversationSystem, Log,
        TEXT("[%s] ConversationSystem ready | Archetype=%s | O=%.2f C=%.2f E=%.2f A=%.2f N=%.2f | ε=%.2f"),
        *GetOwner()->GetName(),
        *UEnum::GetValueAsString(Archetype),
        Openness, Conscientiousness, Extraversion, Agreeableness, Neuroticism,
        Epsilon);
}

void UConversationSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (CurrentState == EConversationState::Idle || CurrentState == EConversationState::Ending)
        return;

    TimeSinceLastDecision += DeltaTime;
    ConversationDuration  += DeltaTime;

    if (ConversationDuration >= MaxConversationDuration)
    {
        UE_LOG(LogConversationSystem, Log, TEXT("[%s] Conversation timed out"), *GetOwner()->GetName());
        EndConversation();
        return;
    }

    float emotionalIntensity = (1.f - CurrentAffinity / 100.f) * Neuroticism;
    bSystem1FastMode = (emotionalIntensity > 0.35f);
    float effectiveInterval = bSystem1FastMode ? DecisionInterval * 0.65f : DecisionInterval;

    if (TimeSinceLastDecision >= effectiveInterval)
    {
        UpdateConversationState();
    }
}

void UConversationSystem::InitArchetype()
{
    switch (Archetype)
    {
    case ENPCArchetype::Scholar:
        Openness          = 0.90f;
        Conscientiousness = 0.80f;
        Extraversion      = 0.40f;
        Agreeableness     = 0.70f;
        Neuroticism       = 0.20f;
        Epsilon           = 0.40f;
        EpsilonDecayRate  = 0.008f;
        LearningRate      = 0.08f;
        break;

    case ENPCArchetype::Guard:
        Openness          = 0.20f;
        Conscientiousness = 0.60f;
        Extraversion      = 0.30f;
        Agreeableness     = 0.20f;
        Neuroticism       = 0.80f;
        Epsilon           = 0.60f;
        EpsilonDecayRate  = 0.015f;
        LearningRate      = 0.15f;
        break;

    case ENPCArchetype::Merchant:
        Openness          = 0.60f;
        Conscientiousness = 0.50f;
        Extraversion      = 0.90f;
        Agreeableness     = 0.80f;
        Neuroticism       = 0.30f;
        Epsilon           = 0.50f;
        EpsilonDecayRate  = 0.012f;
        LearningRate      = 0.12f;
        break;
    }
}

void UConversationSystem::InitQTable()
{
    switch (Archetype)
    {
    case ENPCArchetype::Scholar:
    {
        float init[9][3] = {
            {0.30f, 0.50f, 0.20f},
            {0.20f, 0.60f, 0.20f},
            {0.10f, 0.50f, 0.40f},
            {0.50f, 0.40f, 0.10f},
            {0.40f, 0.50f, 0.10f},
            {0.20f, 0.50f, 0.30f},
            {0.70f, 0.20f, 0.10f},
            {0.60f, 0.30f, 0.10f},
            {0.40f, 0.40f, 0.20f},
        };
        FMemory::Memcpy(QTable, init, sizeof(QTable));
        break;
    }
    case ENPCArchetype::Guard:
    {
        float init[9][3] = {
            {0.10f, 0.30f, 0.60f},
            {0.10f, 0.30f, 0.60f},
            {0.00f, 0.20f, 0.80f},
            {0.20f, 0.40f, 0.40f},
            {0.20f, 0.50f, 0.30f},
            {0.10f, 0.30f, 0.60f},
            {0.40f, 0.40f, 0.20f},
            {0.30f, 0.50f, 0.20f},
            {0.20f, 0.30f, 0.50f},
        };
        FMemory::Memcpy(QTable, init, sizeof(QTable));
        break;
    }
    case ENPCArchetype::Merchant:
    {
        float init[9][3] = {
            {0.40f, 0.40f, 0.20f},
            {0.30f, 0.50f, 0.20f},
            {0.20f, 0.40f, 0.40f},
            {0.60f, 0.30f, 0.10f},
            {0.40f, 0.50f, 0.10f},
            {0.30f, 0.40f, 0.30f},
            {0.70f, 0.20f, 0.10f},
            {0.50f, 0.40f, 0.10f},
            {0.50f, 0.30f, 0.20f},
        };
        FMemory::Memcpy(QTable, init, sizeof(QTable));
        break;
    }
    }
}

void UConversationSystem::StartConversation(AActor* Partner)
{
    if (!Partner || CurrentState != EConversationState::Idle)
        return;

    ConversationPartner       = Partner;
    CurrentState              = EConversationState::Speaking;
    ConversationDuration      = 0.0f;
    TimeSinceLastDecision     = DecisionInterval;
    PendingLearningState      = -1;
    PendingLearningAction     = -1;
    ConversationHistory.Empty();
    ActiveTopic   = FMath::RandRange(0, 2);
    NPCTurnCount  = 0;
    RecentLines.Empty();

    UE_LOG(LogConversationSystem, Log, TEXT("[%s] Conversation started with %s"),
        *GetOwner()->GetName(), *Partner->GetName());

    LogQTable(TEXT("START"));
    OnConversationStarted.Broadcast();
}

void UConversationSystem::StartConversationWithPlayer()
{
    APlayerController* PC = GEngine ? GEngine->GetFirstLocalPlayerController(GetWorld()) : nullptr;
    if (PC && PC->GetPawn())
        StartConversation(PC->GetPawn());
}

void UConversationSystem::EndConversation()
{
    if (CurrentState == EConversationState::Idle)
        return;

    UE_LOG(LogConversationSystem, Log,
        TEXT("[%s] Conversation ended | Affinity: %.1f | Q-Steps: %d | ε=%.3f"),
        *GetOwner()->GetName(), CurrentAffinity, TotalLearningSteps, Epsilon);

    LogQTable(TEXT("END"));

    ConversationPartner   = nullptr;
    CurrentState          = EConversationState::Idle;
    TimeSinceLastDecision = 0.0f;

    OnConversationEnded.Broadcast();
}

void UConversationSystem::UpdateConversationState()
{
    if (CurrentState == EConversationState::Idle || CurrentState == EConversationState::Ending)
        return;

    if (!ConversationPartner.IsValid())
    {
        EndConversation();
        return;
    }

    if (CurrentState == EConversationState::Listening)
        return;

    EConversationStyle ChosenStyle;
    if (bSystem1FastMode)
    {
        ChosenStyle           = SelectActionSystem1();
        PendingLearningState  = -1;
        PendingLearningAction = -1;
    }
    else
    {
        PendingLearningState  = GetStateIndex();
        ChosenStyle           = SelectAction();
        PendingLearningAction = (int32)ChosenStyle;
    }

    FString Line;
    if (NPCTurnCount == 0)
    {
        Line                  = Greetings[(int32)Archetype][FMath::RandRange(0, 3)];
        ChosenStyle           = EConversationStyle::Neutral;
        PendingLearningAction = (int32)EConversationStyle::Neutral;
    }
    else if (NPCTurnCount == 1)
    {
        Line                  = TopicIntros[(int32)Archetype][ActiveTopic];
        ChosenStyle           = EConversationStyle::Neutral;
        PendingLearningAction = (int32)EConversationStyle::Neutral;
    }
    else
    {
        Line = GetDialogueLine(ChosenStyle);
        if (FMath::FRand() < 0.4f)
            Line = GetAcknowledgmentPrefix() + Line;
    }
    NPCTurnCount++;

    FConversationTurn Turn;
    Turn.SpeakerState  = EConversationState::Speaking;
    Turn.SpeakingStyle = ChosenStyle;
    Turn.SpokenLine    = Line;
    ConversationHistory.Add(Turn);

    UE_LOG(LogConversationSystem, Log,
        TEXT("[%s] NPC [%s]: \"%s\" | Affinity=%.1f | ε=%.3f | Steps=%d | %s"),
        *GetOwner()->GetName(),
        *UEnum::GetValueAsString(ChosenStyle),
        *Line,
        CurrentAffinity, Epsilon, TotalLearningSteps,
        bSystem1FastMode ? TEXT("System1-Fast") : TEXT("System2-Deliberative"));

    CurrentState          = EConversationState::Listening;
    TimeSinceLastDecision = 0.0f;
    OnTurnCompleted.Broadcast();
}

void UConversationSystem::OnPartnerSpoke(const FString& Line, EConversationStyle PartnerStyle)
{
    float affinityDelta = 0.f;
    switch (PartnerStyle)
    {
    case EConversationStyle::Friendly:
        affinityDelta =  8.f + Agreeableness * 4.f;
        break;
    case EConversationStyle::Neutral:
        affinityDelta =  0.f;
        break;
    case EConversationStyle::Defensive:
        affinityDelta = -(8.f + Neuroticism * 4.f);
        break;
    default:
        affinityDelta = -(12.f + Neuroticism * 6.f);
        break;
    }

    float affinityBefore = CurrentAffinity;
    CurrentAffinity = FMath::Clamp(CurrentAffinity + affinityDelta, 0.f, 100.f);

    if (PendingLearningState >= 0)
    {
        LastPlayerStyle = PartnerStyle;
        int32 nextState = GetStateIndex();

        float reward = FMath::Clamp(affinityDelta / 20.f, -1.f, 1.f);

        ApplyBellmanUpdate(PendingLearningState, PendingLearningAction, reward, nextState);
        PendingLearningState  = -1;
        PendingLearningAction = -1;
    }
    else
    {
        LastPlayerStyle = PartnerStyle;
    }

    UE_LOG(LogConversationSystem, Log,
        TEXT("[%s] Player [%s] | Affinity: %.1f→%.1f | Q-Steps: %d | ε=%.3f"),
        *GetOwner()->GetName(),
        *UEnum::GetValueAsString(PartnerStyle),
        affinityBefore, CurrentAffinity,
        TotalLearningSteps, Epsilon);

    FConversationTurn PlayerTurn;
    PlayerTurn.SpeakerState  = EConversationState::Listening;
    PlayerTurn.SpeakingStyle = PartnerStyle;
    PlayerTurn.SpokenLine    = Line;
    ConversationHistory.Add(PlayerTurn);

    if (CurrentAffinity <= 2.f)
    {
        UE_LOG(LogConversationSystem, Log, TEXT("[%s] Affinity exhausted — ending conversation"), *GetOwner()->GetName());
        EndConversation();
        return;
    }

    if (CurrentState == EConversationState::Listening)
    {
        CurrentState          = EConversationState::Responding;
        TimeSinceLastDecision = 0.f;
    }
}

int32 UConversationSystem::GetStateIndex() const
{
    int32 affinityTier = (CurrentAffinity < 34.f) ? 0 : (CurrentAffinity < 67.f) ? 1 : 2;
    int32 styleTier    = (LastPlayerStyle == EConversationStyle::Friendly)  ? 0
                       : (LastPlayerStyle == EConversationStyle::Neutral)    ? 1 : 2;
    return affinityTier * 3 + styleTier;
}

EConversationStyle UConversationSystem::SelectAction()
{
    int32 state = GetStateIndex();

    if (FMath::FRand() < Epsilon)
    {
        return (EConversationStyle)FMath::RandRange(0, 2);
    }

    float biasedQ[3];
    biasedQ[0] = QTable[state][0] + Agreeableness * 0.10f;
    biasedQ[1] = QTable[state][1];
    biasedQ[2] = QTable[state][2] + Neuroticism  * 0.10f;

    int32 bestAction = 0;
    for (int32 i = 1; i < 3; ++i)
    {
        if (biasedQ[i] > biasedQ[bestAction])
            bestAction = i;
    }

    return (EConversationStyle)bestAction;
}

void UConversationSystem::ApplyBellmanUpdate(int32 State, int32 Action, float Reward, int32 NextState)
{
    float maxNextQ = FMath::Max3(QTable[NextState][0], QTable[NextState][1], QTable[NextState][2]);
    float tdError  = Reward + DiscountFactor * maxNextQ - QTable[State][Action];
    QTable[State][Action] += LearningRate * tdError;

    Epsilon = FMath::Max(MinEpsilon, Epsilon - EpsilonDecayRate);
    TotalLearningSteps++;

    UE_LOG(LogConversationSystem, Verbose,
        TEXT("[%s] Q[%d][%d] ← %.3f | TD=%.3f | ε=%.3f | Steps=%d"),
        *GetOwner()->GetName(), State, Action, QTable[State][Action],
        tdError, Epsilon, TotalLearningSteps);
}

FString UConversationSystem::GetDialogueLine(EConversationStyle Style) const
{
    int32 archetypeIdx = (int32)Archetype;
    int32 topicIdx     = ActiveTopic;
    int32 styleIdx     = (Style == EConversationStyle::Friendly) ? 0
                       : (Style == EConversationStyle::Neutral)  ? 1 : 2;

    TArray<FString> candidates;
    for (int32 i = 0; i < 4; ++i)
    {
        const FString& Line = TopicLines[archetypeIdx][topicIdx][styleIdx][i];
        if (!RecentLines.Contains(Line))
            candidates.Add(Line);
    }

    if (candidates.IsEmpty())
    {
        RecentLines.Empty();
        for (int32 i = 0; i < 4; ++i)
            candidates.Add(TopicLines[archetypeIdx][topicIdx][styleIdx][i]);
    }

    FString Chosen = candidates[FMath::RandRange(0, candidates.Num() - 1)];

    RecentLines.Add(Chosen);
    if (RecentLines.Num() > 3)
        RecentLines.RemoveAt(0);

    return Chosen;
}

EConversationStyle UConversationSystem::SelectActionSystem1()
{
    if (CurrentAffinity < 25.f)
        return EConversationStyle::Defensive;
    if (CurrentAffinity > 75.f)
        return EConversationStyle::Friendly;

    if (LastPlayerStyle == EConversationStyle::Friendly)
        return (Neuroticism > 0.5f) ? EConversationStyle::Neutral : EConversationStyle::Friendly;
    if (LastPlayerStyle == EConversationStyle::Defensive)
        return (Neuroticism > 0.5f) ? EConversationStyle::Defensive : EConversationStyle::Neutral;

    return EConversationStyle::Neutral;
}

void UConversationSystem::LogQTable(const FString& Label) const
{
    static const FString StateLabels[9] = {
        TEXT("Low  + Friendly"), TEXT("Low  + Neutral"), TEXT("Low  + Defensive"),
        TEXT("Med  + Friendly"), TEXT("Med  + Neutral"), TEXT("Med  + Defensive"),
        TEXT("High + Friendly"), TEXT("High + Neutral"), TEXT("High + Defensive")
    };

    UE_LOG(LogConversationSystem, Log,
        TEXT("[%s] ── Q-Table %s ── ε=%.3f  Steps=%d  Affinity=%.1f"),
        *GetOwner()->GetName(), *Label, Epsilon, TotalLearningSteps, CurrentAffinity);

    for (int32 s = 0; s < 9; ++s)
    {
        int32 best = 0;
        for (int32 a = 1; a < 3; ++a)
            if (QTable[s][a] > QTable[s][best]) best = a;

        UE_LOG(LogConversationSystem, Log,
            TEXT("  [%s]  F=%.3f%s  N=%.3f%s  D=%.3f%s"),
            *StateLabels[s],
            QTable[s][0], (best == 0 ? TEXT("*") : TEXT(" ")),
            QTable[s][1], (best == 1 ? TEXT("*") : TEXT(" ")),
            QTable[s][2], (best == 2 ? TEXT("*") : TEXT(" ")));
    }
}

FString UConversationSystem::GetAcknowledgmentPrefix() const
{
    static const FString Acks[3][3][3] =
    {
        {
            { TEXT("I appreciate that. "),    TEXT("That resonates with me. "),         TEXT("Agreed. ") },
            { TEXT("I see. "),                TEXT("Noted. "),                           TEXT("Interesting. ") },
            { TEXT("A fair challenge. "),     TEXT("I understand your reservations. "),  TEXT("Point taken. ") }
        },
        {
            { TEXT("...Right. "),             TEXT("Hmm. "),                             TEXT("Fine. ") },
            { TEXT("..."),                    TEXT("Keep going. "),                       TEXT("Mmm. ") },
            { TEXT("As I thought. "),         TEXT("Careful. "),                         TEXT("Watch it. ") }
        },
        {
            { TEXT("Excellent! "),            TEXT("Wonderful! "),                       TEXT("Now we're talking! ") },
            { TEXT("I see. "),                TEXT("Fair enough. "),                      TEXT("Understood. ") },
            { TEXT("I take your point. "),    TEXT("Noted. "),                            TEXT("Fair. ") }
        }
    };

    int32 archetypeIdx = (int32)Archetype;
    int32 styleIdx = (LastPlayerStyle == EConversationStyle::Friendly) ? 0
                   : (LastPlayerStyle == EConversationStyle::Neutral)  ? 1 : 2;

    return Acks[archetypeIdx][styleIdx][FMath::RandRange(0, 2)];
}

FString UConversationSystem::GetPreferredStyleName() const
{
    int32 state = GetStateIndex();
    float biasedQ[3];
    biasedQ[0] = QTable[state][0] + Agreeableness * 0.10f;
    biasedQ[1] = QTable[state][1];
    biasedQ[2] = QTable[state][2] + Neuroticism   * 0.10f;

    int32 best = 0;
    for (int32 i = 1; i < 3; ++i)
        if (biasedQ[i] > biasedQ[best]) best = i;

    switch ((EConversationStyle)best)
    {
    case EConversationStyle::Friendly:  return TEXT("Friendly");
    case EConversationStyle::Defensive: return TEXT("Defensive");
    default:                            return TEXT("Neutral");
    }
}

FString UConversationSystem::GetRandomPhrase(const FString& TopicName) const
{
    for (const FConversationTopic& Topic : AvailableTopics)
    {
        if (Topic.TopicName == TopicName && Topic.Phrases.Num() > 0)
            return Topic.Phrases[FMath::RandRange(0, Topic.Phrases.Num() - 1)];
    }
    return GetDialogueLine(EConversationStyle::Neutral);
}
