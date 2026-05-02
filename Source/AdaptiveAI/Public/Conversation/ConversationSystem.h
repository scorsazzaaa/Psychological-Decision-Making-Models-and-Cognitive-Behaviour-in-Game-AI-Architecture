#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ConversationSystem.generated.h"

UENUM(BlueprintType)
enum class EConversationState : uint8
{
    Idle       UMETA(DisplayName = "Idle"),
    Initiating UMETA(DisplayName = "Initiating"),
    Speaking   UMETA(DisplayName = "Speaking"),
    Listening  UMETA(DisplayName = "Listening"),
    Responding UMETA(DisplayName = "Responding"),
    Ending     UMETA(DisplayName = "Ending")
};

UENUM(BlueprintType)
enum class EConversationStyle : uint8
{
    Friendly   UMETA(DisplayName = "Friendly"),
    Neutral    UMETA(DisplayName = "Neutral"),
    Defensive  UMETA(DisplayName = "Defensive"),
    Aggressive UMETA(DisplayName = "Aggressive")
};

UENUM(BlueprintType)
enum class ENPCArchetype : uint8
{
    Scholar  UMETA(DisplayName = "The Scholar  (System-2 / Deliberative)"),
    Guard    UMETA(DisplayName = "The Guard    (System-1 / Reactive)"),
    Merchant UMETA(DisplayName = "The Merchant (Decision Fusion / Adaptive)")
};

USTRUCT(BlueprintType)
struct FConversationTurn
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    EConversationState SpeakerState = EConversationState::Idle;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    EConversationStyle SpeakingStyle = EConversationStyle::Neutral;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    FString SpokenLine;
};

USTRUCT(BlueprintType)
struct FConversationTopic
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    FString TopicName;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    TArray<FString> Phrases;

    FConversationTopic() {}
    FConversationTopic(const FString& InName, const TArray<FString>& InPhrases)
        : TopicName(InName), Phrases(InPhrases) {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConversationEvent);

UCLASS(ClassGroup = (AdaptiveAI), meta = (BlueprintSpawnableComponent))
class ADAPTIVEAI_API UConversationSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UConversationSystem();

    UPROPERTY(EditAnywhere, Category = "NPC | Archetype")
    ENPCArchetype Archetype = ENPCArchetype::Merchant;

    UPROPERTY(EditAnywhere, Category = "NPC | OCEAN Personality")
    float Openness = 0.5f;

    UPROPERTY(EditAnywhere, Category = "NPC | OCEAN Personality")
    float Conscientiousness = 0.5f;

    UPROPERTY(EditAnywhere, Category = "NPC | OCEAN Personality")
    float Extraversion = 0.5f;

    UPROPERTY(EditAnywhere, Category = "NPC | OCEAN Personality")
    float Agreeableness = 0.5f;

    UPROPERTY(EditAnywhere, Category = "NPC | OCEAN Personality")
    float Neuroticism = 0.5f;

    UPROPERTY(EditAnywhere, Category = "NPC | Reinforcement Learning")
    float LearningRate = 0.1f;

    UPROPERTY(EditAnywhere, Category = "NPC | Reinforcement Learning")
    float DiscountFactor = 0.9f;

    UPROPERTY(EditAnywhere, Category = "NPC | Reinforcement Learning")
    float EpsilonDecayRate = 0.01f;

    UPROPERTY(EditAnywhere, Category = "NPC | Conversation")
    float DecisionInterval = 1.5f;

    UPROPERTY(EditAnywhere, Category = "NPC | Conversation")
    float MaxConversationDuration = 30.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation | State")
    EConversationState CurrentState = EConversationState::Idle;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation | State")
    float CurrentAffinity = 50.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation | RL")
    float Epsilon = 0.5f;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation | RL")
    int32 TotalLearningSteps = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation | Dual-Process")
    bool bSystem1FastMode = false;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation | State")
    TWeakObjectPtr<AActor> ConversationPartner;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation | State")
    TArray<FConversationTurn> ConversationHistory;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation | State")
    int32 NPCTurnCount = 0;

    UPROPERTY(BlueprintAssignable, Category = "Conversation | Events")
    FOnConversationEvent OnConversationStarted;

    UPROPERTY(BlueprintAssignable, Category = "Conversation | Events")
    FOnConversationEvent OnConversationEnded;

    UPROPERTY(BlueprintAssignable, Category = "Conversation | Events")
    FOnConversationEvent OnTurnCompleted;

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    void StartConversation(AActor* Partner);

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    void StartConversationWithPlayer();

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    void EndConversation();

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    void UpdateConversationState();

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    void OnPartnerSpoke(const FString& Line, EConversationStyle PartnerStyle);

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    FString GetRandomPhrase(const FString& TopicName) const;

    UFUNCTION(BlueprintCallable, Category = "Conversation | RL")
    FString GetPreferredStyleName() const;

    UPROPERTY(EditAnywhere, Category = "Conversation")
    TArray<FConversationTopic> AvailableTopics;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    float QTable[9][3];

    EConversationStyle LastPlayerStyle = EConversationStyle::Neutral;

    int32 PendingLearningState  = -1;
    int32 PendingLearningAction = -1;

    float TimeSinceLastDecision  = 0.0f;
    float ConversationDuration   = 0.0f;
    float MinEpsilon             = 0.05f;

    void  InitArchetype();
    void  InitQTable();
    int32 GetStateIndex() const;
    EConversationStyle SelectAction();
    EConversationStyle SelectActionSystem1();
    void  ApplyBellmanUpdate(int32 State, int32 Action, float Reward, int32 NextState);
    FString GetDialogueLine(EConversationStyle Style) const;
    FString GetAcknowledgmentPrefix() const;
    void  LogQTable(const FString& Label) const;

    int32 ActiveTopic  = 0;
    mutable TArray<FString> RecentLines;
};
