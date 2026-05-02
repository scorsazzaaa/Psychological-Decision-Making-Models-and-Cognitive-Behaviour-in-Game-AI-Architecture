#include "Conversation/PlayerConversationComponent.h"
#include "UI/ConversationHUDWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerConversation, Log, All);

static const FString GreetingFriendly[3] = {
    TEXT("Always happy to stop and chat."),
    TEXT("Good timing — I could use a conversation."),
    TEXT("Happy to. What's going on?")
};
static const FString GreetingNeutral[3] = {
    TEXT("Sure, I've got a few minutes."),
    TEXT("Alright. What's on your mind?"),
    TEXT("I'll see how it goes.")
};
static const FString GreetingDefensive[3] = {
    TEXT("Keep it brief, though."),
    TEXT("Depends what you want to talk about."),
    TEXT("I'm listening. For now.")
};

static const FString TopicFriendly[3] = {
    TEXT("Oh? Tell me more."),
    TEXT("Now I'm curious — go on."),
    TEXT("Interesting. What happened?")
};
static const FString TopicNeutral[3] = {
    TEXT("Alright, what's the story?"),
    TEXT("I'm listening."),
    TEXT("Fair enough. What is it?")
};
static const FString TopicDefensive[3] = {
    TEXT("Should I be worried?"),
    TEXT("Sounds like it might be nothing."),
    TEXT("Alright, go on then.")
};

static const TArray<FString> DevelopmentFriendly = {
    TEXT("Yeah, I see what you mean."),
    TEXT("That makes sense, actually."),
    TEXT("Right, I hadn't thought of it like that."),
    TEXT("Honestly, same."),
    TEXT("That's actually interesting."),
    TEXT("I'm glad you said that.")
};
static const TArray<FString> DevelopmentNeutral = {
    TEXT("Hard to say."),
    TEXT("Maybe. Who knows."),
    TEXT("I suppose."),
    TEXT("Could be."),
    TEXT("Possibly."),
    TEXT("Not sure what to make of that.")
};
static const TArray<FString> DevelopmentDefensive = {
    TEXT("I don't know about that."),
    TEXT("That's a bit of a stretch."),
    TEXT("I'm not so sure."),
    TEXT("I'd take that with a pinch of salt."),
    TEXT("Really? I'm not convinced."),
    TEXT("I see it differently.")
};

UPlayerConversationComponent::UPlayerConversationComponent()
    : Super()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UPlayerConversationComponent::BeginPlay()
{
    Super::BeginPlay();
    PlayerController = GEngine ? GEngine->GetFirstLocalPlayerController(GetWorld()) : nullptr;

    if (PlayerController.IsValid())
    {
        CreateHUD();
    }
}

void UPlayerConversationComponent::CreateHUD()
{
    if (HUDWidget) return;

    APlayerController* PC = PlayerController.Get();
    if (!PC)
    {
        UE_LOG(LogPlayerConversation, Warning, TEXT("CreateHUD: no player controller"));
        return;
    }

    TSubclassOf<UConversationHUDWidget> WidgetClass = HUDWidgetClass;
    if (!WidgetClass)
    {
        WidgetClass = UConversationHUDWidget::StaticClass();
    }
    UE_LOG(LogPlayerConversation, Log, TEXT("CreateHUD: using class %s"), *WidgetClass->GetName());
    HUDWidget = CreateWidget<UConversationHUDWidget>(PC, WidgetClass);
    if (HUDWidget)
    {
        HUDWidget->SetConversationComponent(this);
        HUDWidget->AddToViewport(100);
        UE_LOG(LogPlayerConversation, Log, TEXT("Conversation HUD added to viewport | Class=%s | Visibility=%d"),
            *WidgetClass->GetName(), (int32)HUDWidget->GetVisibility());
    }
    else
    {
        UE_LOG(LogPlayerConversation, Error, TEXT("Conversation HUD widget creation failed!"));
    }
}

void UPlayerConversationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!HUDWidget && PlayerController.IsValid())
    {
        CreateHUD();
    }

    if (!bInputBound)
    {
        TryBindInput();
    }

    if (CurrentState == EConversationState::Idle)
    {
        TimeSinceLastScan += DeltaTime;
        if (TimeSinceLastScan >= ScanInterval)
        {
            FindNearbyConversationalists();
            TimeSinceLastScan = 0.0f;
        }
    }
}

void UPlayerConversationComponent::TryBindInput()
{
    APawn* Pawn = Cast<APawn>(GetOwner());
    if (!Pawn)
    {
        UE_LOG(LogPlayerConversation, Warning, TEXT("TryBindInput: owner is not a pawn"));
        return;
    }

    APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    if (!PC)
    {
        UE_LOG(LogPlayerConversation, Warning, TEXT("TryBindInput: no player controller"));
        return;
    }

    if (!PC->InputComponent)
    {
        UE_LOG(LogPlayerConversation, Warning, TEXT("TryBindInput: InputComponent is null (will retry)"));
        return;
    }

    PC->InputComponent->BindKey(EKeys::E,     IE_Pressed, this, &UPlayerConversationComponent::ToggleConversation);
    PC->InputComponent->BindKey(EKeys::One,   IE_Pressed, this, &UPlayerConversationComponent::RespondFriendly);
    PC->InputComponent->BindKey(EKeys::Two,   IE_Pressed, this, &UPlayerConversationComponent::RespondNeutral);
    PC->InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &UPlayerConversationComponent::RespondDefensive);

    bInputBound = true;
    UE_LOG(LogPlayerConversation, Log, TEXT("Conversation input bound: E=Talk, 1=Friendly, 2=Neutral, 3=Defensive"));
}

void UPlayerConversationComponent::FindNearbyConversationalists()
{
    AvailableInteractions.Empty();

    if (!PlayerController.IsValid() || !PlayerController->GetPawn())
    {
        UE_LOG(LogPlayerConversation, Warning, TEXT("FindNearbyConversationalists: no player controller or pawn"));
        return;
    }

    FVector PlayerLocation = PlayerController->GetPawn()->GetActorLocation();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        if (Actor == GetOwner()) continue;

        UConversationSystem* ActorConv = Actor->FindComponentByClass<UConversationSystem>();
        if (!ActorConv) continue;

        float Distance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());
        if (Distance <= InteractionRange)
        {
            FConversationInteraction Interaction;
            Interaction.Partner = Actor;
            Interaction.Style   = EConversationStyle::Friendly;
            AvailableInteractions.Add(Interaction);
        }
    }

    UE_LOG(LogPlayerConversation, Log, TEXT("Scan complete: found %d conversationalists within %.0f units"),
        AvailableInteractions.Num(), InteractionRange);
}

void UPlayerConversationComponent::ToggleConversation()
{
    if (CurrentState == EConversationState::Idle)
        StartConversation();
    else
        EndConversation();
}

void UPlayerConversationComponent::StartConversation()
{
    if (CurrentState != EConversationState::Idle)
    {
        UE_LOG(LogPlayerConversation, Warning, TEXT("Already in conversation"));
        return;
    }

    if (AvailableInteractions.Num() == 0)
    {
        UE_LOG(LogPlayerConversation, Log, TEXT("No nearby NPCs to talk to (range: %.0f)"), InteractionRange);
        return;
    }

    AActor* Partner = AvailableInteractions[0].Partner;
    if (!Partner) return;

    UConversationSystem* NPC_Conv = Partner->FindComponentByClass<UConversationSystem>();
    if (!NPC_Conv)
    {
        UE_LOG(LogPlayerConversation, Warning, TEXT("Partner %s has no ConversationSystem"), *Partner->GetName());
        return;
    }

    if (NPC_Conv->CurrentState != EConversationState::Idle)
    {
        UE_LOG(LogPlayerConversation, Warning, TEXT("%s is already in a conversation"), *Partner->GetName());
        return;
    }

    NPC_Conv->OnTurnCompleted.AddDynamic(this, &UPlayerConversationComponent::HandleNPCTurnCompleted);
    NPC_Conv->OnConversationEnded.AddDynamic(this, &UPlayerConversationComponent::HandleNPCConversationEnded);

    NPC_Conv->StartConversationWithPlayer();

    NPC_ConversationSystem     = NPC_Conv;
    CurrentConversationPartner = Partner;
    CurrentState               = EConversationState::Listening;
    LastNPCLine                = TEXT("...");
    RefreshResponseOptions();

    if (PlayerController.IsValid())
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        PlayerController->SetInputMode(InputMode);
        PlayerController->SetShowMouseCursor(true);
    }

    UE_LOG(LogPlayerConversation, Log, TEXT("Conversation started with %s"), *Partner->GetName());
    OnConversationStarted.Broadcast();
}

void UPlayerConversationComponent::EndConversation()
{
    if (NPC_ConversationSystem.IsValid())
    {
        NPC_ConversationSystem->OnTurnCompleted.RemoveDynamic(this, &UPlayerConversationComponent::HandleNPCTurnCompleted);
        NPC_ConversationSystem->OnConversationEnded.RemoveDynamic(this, &UPlayerConversationComponent::HandleNPCConversationEnded);
        NPC_ConversationSystem->EndConversation();
    }

    NPC_ConversationSystem     = nullptr;
    CurrentConversationPartner = nullptr;
    CurrentState               = EConversationState::Idle;

    if (PlayerController.IsValid())
    {
        PlayerController->SetInputMode(FInputModeGameOnly());
        PlayerController->SetShowMouseCursor(false);
    }

    UE_LOG(LogPlayerConversation, Log, TEXT("Conversation ended by player"));
    OnConversationEnded.Broadcast();
}

void UPlayerConversationComponent::ChooseInteraction(int32 Index)
{
    if (Index < 0 || Index >= AvailableInteractions.Num()) return;

    AActor* Partner = AvailableInteractions[Index].Partner;
    if (Partner)
    {
        CurrentConversationPartner = Partner;
        StartConversation();
    }
}

void UPlayerConversationComponent::SendPlayerResponse(const FString& Line, EConversationStyle Style)
{
    if (!NPC_ConversationSystem.IsValid() || CurrentState == EConversationState::Idle)
    {
        UE_LOG(LogPlayerConversation, Warning, TEXT("No active conversation to respond to"));
        return;
    }

    if (NPC_ConversationSystem->CurrentState != EConversationState::Listening)
        return;

    UE_LOG(LogPlayerConversation, Log, TEXT("Player responds [%s]: '%s'"), *UEnum::GetValueAsString(Style), *Line);

    NPC_ConversationSystem->OnPartnerSpoke(Line, Style);
    CurrentState = EConversationState::Listening;
}

void UPlayerConversationComponent::RefreshResponseOptions()
{
    int32 TurnCount = NPC_ConversationSystem.IsValid() ? NPC_ConversationSystem->NPCTurnCount : 3;

    if (TurnCount <= 1)
    {
        PendingFriendlyLine  = GreetingFriendly [FMath::RandRange(0, 2)];
        PendingNeutralLine   = GreetingNeutral  [FMath::RandRange(0, 2)];
        PendingDefensiveLine = GreetingDefensive[FMath::RandRange(0, 2)];
    }
    else if (TurnCount == 2)
    {
        PendingFriendlyLine  = TopicFriendly [FMath::RandRange(0, 2)];
        PendingNeutralLine   = TopicNeutral  [FMath::RandRange(0, 2)];
        PendingDefensiveLine = TopicDefensive[FMath::RandRange(0, 2)];
    }
    else
    {
        PendingFriendlyLine  = DevelopmentFriendly [FMath::RandRange(0, DevelopmentFriendly.Num()  - 1)];
        PendingNeutralLine   = DevelopmentNeutral  [FMath::RandRange(0, DevelopmentNeutral.Num()   - 1)];
        PendingDefensiveLine = DevelopmentDefensive[FMath::RandRange(0, DevelopmentDefensive.Num() - 1)];
    }
}

void UPlayerConversationComponent::RespondFriendly()
{
    SendPlayerResponse(PendingFriendlyLine, EConversationStyle::Friendly);
}

void UPlayerConversationComponent::RespondNeutral()
{
    SendPlayerResponse(PendingNeutralLine, EConversationStyle::Neutral);
}

void UPlayerConversationComponent::RespondDefensive()
{
    SendPlayerResponse(PendingDefensiveLine, EConversationStyle::Defensive);
}

void UPlayerConversationComponent::AskAboutTopic(const FString& TopicName)
{
    if (!NPC_ConversationSystem.IsValid())
    {
        UE_LOG(LogPlayerConversation, Warning, TEXT("No active conversation"));
        return;
    }

    FString Phrase = NPC_ConversationSystem->GetRandomPhrase(TopicName);
    UE_LOG(LogPlayerConversation, Log, TEXT("NPC says about %s: '%s'"), *TopicName, *Phrase);
}

void UPlayerConversationComponent::HandleNPCTurnCompleted()
{
    if (!NPC_ConversationSystem.IsValid()) return;

    const TArray<FConversationTurn>& History = NPC_ConversationSystem->ConversationHistory;
    if (History.Num() == 0) return;

    for (int32 i = History.Num() - 1; i >= 0; --i)
    {
        if (History[i].SpeakerState == EConversationState::Speaking)
        {
            LastNPCLine  = History[i].SpokenLine;
            LastNPCStyle = History[i].SpeakingStyle;
            break;
        }
    }

    UE_LOG(LogPlayerConversation, Log, TEXT("NPC says [%s]: '%s' | Affinity: %.1f | RL Steps: %d"),
        *UEnum::GetValueAsString(LastNPCStyle), *LastNPCLine,
        NPC_ConversationSystem->CurrentAffinity, GetNPCLearningSteps());

    CurrentState = EConversationState::Listening;
    RefreshResponseOptions();
    OnNPCSpoke.Broadcast(LastNPCLine, LastNPCStyle);
}

void UPlayerConversationComponent::HandleNPCConversationEnded()
{
    NPC_ConversationSystem     = nullptr;
    CurrentConversationPartner = nullptr;
    CurrentState               = EConversationState::Idle;

    if (PlayerController.IsValid())
    {
        PlayerController->SetInputMode(FInputModeGameOnly());
        PlayerController->SetShowMouseCursor(false);
    }

    UE_LOG(LogPlayerConversation, Log, TEXT("NPC ended the conversation"));
    OnConversationEnded.Broadcast();
}

float UPlayerConversationComponent::GetNPCAffinity() const
{
    return NPC_ConversationSystem.IsValid() ? NPC_ConversationSystem->CurrentAffinity : 0.0f;
}

int32 UPlayerConversationComponent::GetNPCLearningSteps() const
{
    return NPC_ConversationSystem.IsValid() ? NPC_ConversationSystem->TotalLearningSteps : 0;
}

float UPlayerConversationComponent::GetNPCEpsilon() const
{
    return NPC_ConversationSystem.IsValid() ? NPC_ConversationSystem->Epsilon : 0.0f;
}

FString UPlayerConversationComponent::GetNPCReasoningMode() const
{
    if (!NPC_ConversationSystem.IsValid()) return TEXT("N/A");
    return NPC_ConversationSystem->bSystem1FastMode
        ? TEXT("System 1 (Fast)")
        : TEXT("System 2 (Deliberative)");
}

void UPlayerConversationComponent::ListAvailableConversations()
{
    UE_LOG(LogPlayerConversation, Log, TEXT("=== Available Conversations ==="));
    for (int32 i = 0; i < AvailableInteractions.Num(); i++)
    {
        FConversationInteraction& Interaction = AvailableInteractions[i];
        if (!Interaction.Partner || !PlayerController.IsValid()) continue;
        float Dist = FVector::Dist(PlayerController->GetPawn()->GetActorLocation(), Interaction.Partner->GetActorLocation());
        UE_LOG(LogPlayerConversation, Log, TEXT("  [%d] %s (%.0f units)"), i, *Interaction.Partner->GetName(), Dist);
    }
}

FString UPlayerConversationComponent::GetPartnerName() const
{
    return CurrentConversationPartner.IsValid() ? CurrentConversationPartner->GetName() : TEXT("None");
}

FString UPlayerConversationComponent::GetNPCPreferredStyle() const
{
    return NPC_ConversationSystem.IsValid() ? NPC_ConversationSystem->GetPreferredStyleName() : TEXT("N/A");
}
