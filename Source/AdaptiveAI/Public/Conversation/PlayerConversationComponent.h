#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ConversationSystem.h"
#include "PlayerConversationComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCSpoke, const FString&, Line, EConversationStyle, Style);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerConversationEvent);

USTRUCT(BlueprintType)
struct FConversationInteraction
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    AActor* Partner = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    EConversationStyle Style = EConversationStyle::Friendly;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    FString SelectedPhrase;

    FConversationInteraction()
    {
    }
};

UCLASS(ClassGroup = (AdaptiveAI), meta = (BlueprintSpawnableComponent))
class ADAPTIVEAI_API UPlayerConversationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPlayerConversationComponent();

    UPROPERTY(EditAnywhere, Category = "Conversation")
    float InteractionRange = 400.0f;

    UPROPERTY(EditAnywhere, Category = "Conversation")
    TSubclassOf<class UConversationHUDWidget> HUDWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    TWeakObjectPtr<AActor> CurrentConversationPartner;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    EConversationState CurrentState = EConversationState::Idle;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    TArray<FConversationInteraction> AvailableInteractions;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    FString LastNPCLine;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    EConversationStyle LastNPCStyle = EConversationStyle::Neutral;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    FString PendingFriendlyLine;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    FString PendingNeutralLine;

    UPROPERTY(BlueprintReadOnly, Category = "Conversation")
    FString PendingDefensiveLine;

    UPROPERTY(BlueprintAssignable, Category = "Conversation|Events")
    FOnNPCSpoke OnNPCSpoke;

    UPROPERTY(BlueprintAssignable, Category = "Conversation|Events")
    FOnPlayerConversationEvent OnConversationStarted;

    UPROPERTY(BlueprintAssignable, Category = "Conversation|Events")
    FOnPlayerConversationEvent OnConversationEnded;

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    void StartConversation();

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    void EndConversation();

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    void ChooseInteraction(int32 Index);

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    void RespondFriendly();

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    void RespondNeutral();

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    void RespondDefensive();

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    void AskAboutTopic(const FString& TopicName);

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    float GetNPCAffinity() const;

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    int32 GetNPCLearningSteps() const;

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    float GetNPCEpsilon() const;

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    FString GetNPCReasoningMode() const;

    UFUNCTION(BlueprintCallable, Category = "Conversation")
    FString GetNPCPreferredStyle() const;

    UFUNCTION(BlueprintCallable, Category = "Debug")
    void ListAvailableConversations();

    UFUNCTION(BlueprintCallable, Category = "Debug")
    FString GetPartnerName() const;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    void CreateHUD();
    void FindNearbyConversationalists();
    void TryBindInput();
    void ToggleConversation();
    void SendPlayerResponse(const FString& Line, EConversationStyle Style);
    void RefreshResponseOptions();

    UFUNCTION()
    void HandleNPCTurnCompleted();

    UFUNCTION()
    void HandleNPCConversationEnded();

    UPROPERTY()
    TWeakObjectPtr<UConversationSystem> NPC_ConversationSystem;

    UPROPERTY()
    TWeakObjectPtr<APlayerController> PlayerController;

    float TimeSinceLastScan = 0.0f;
    float ScanInterval = 0.3f;
    bool bInputBound = false;

    UPROPERTY()
    TObjectPtr<UConversationSystem> ConversationSystem;

    UPROPERTY()
    TObjectPtr<class UConversationHUDWidget> HUDWidget;
};
