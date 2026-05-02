#include "UI/ConversationHUDWidget.h"
#include "Conversation/PlayerConversationComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogConversationHUD, Log, All);

void UConversationHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogConversationHUD, Log, TEXT("ConversationHUDWidget NativeConstruct | Panel=%s Name=%s Dialogue=%s"),
        (ConversationPanel ? TEXT("OK") : TEXT("NULL")),
        (NPCNameText ? TEXT("OK") : TEXT("NULL")),
        (DialogueText ? TEXT("OK") : TEXT("NULL")));

    if (FriendlyBtn)
        FriendlyBtn->OnClicked.AddDynamic(this, &UConversationHUDWidget::OnFriendlyClicked);
    if (NeutralBtn)
        NeutralBtn->OnClicked.AddDynamic(this, &UConversationHUDWidget::OnNeutralClicked);
    if (DefensiveBtn)
        DefensiveBtn->OnClicked.AddDynamic(this, &UConversationHUDWidget::OnDefensiveClicked);

    if (ConversationPanel)
        ConversationPanel->SetVisibility(ESlateVisibility::Hidden);
    if (BackgroundBorder)
        BackgroundBorder->SetVisibility(ESlateVisibility::Hidden);
}

void UConversationHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    UpdateStats();
}

void UConversationHUDWidget::SetConversationComponent(UPlayerConversationComponent* Comp)
{
    ConvComp = Comp;
    if (!Comp) return;

    Comp->OnNPCSpoke.AddDynamic(this, &UConversationHUDWidget::HandleNPCSpoke);
    Comp->OnConversationStarted.AddDynamic(this, &UConversationHUDWidget::HandleConversationStarted);
    Comp->OnConversationEnded.AddDynamic(this, &UConversationHUDWidget::HandleConversationEnded);
}

void UConversationHUDWidget::HandleNPCSpoke(const FString& Line, EConversationStyle Style)
{
    if (DialogueText)
        DialogueText->SetText(FText::FromString(Line));

    if (ConvComp.IsValid())
    {
        if (FriendlyBtnText)  FriendlyBtnText ->SetText(FText::FromString(ConvComp->PendingFriendlyLine));
        if (NeutralBtnText)   NeutralBtnText  ->SetText(FText::FromString(ConvComp->PendingNeutralLine));
        if (DefensiveBtnText) DefensiveBtnText->SetText(FText::FromString(ConvComp->PendingDefensiveLine));
    }

    FLinearColor StyleColour;
    FString StyleLabel;
    switch (Style)
    {
    case EConversationStyle::Friendly:
        StyleColour = FLinearColor(0.2f, 0.9f, 0.2f, 1.f);
        StyleLabel  = TEXT("Friendly");
        break;
    case EConversationStyle::Defensive:
        StyleColour = FLinearColor(0.9f, 0.3f, 0.2f, 1.f);
        StyleLabel  = TEXT("Defensive");
        break;
    case EConversationStyle::Aggressive:
        StyleColour = FLinearColor(1.0f, 0.1f, 0.1f, 1.f);
        StyleLabel  = TEXT("Aggressive");
        break;
    default:
        StyleColour = FLinearColor(0.8f, 0.8f, 0.8f, 1.f);
        StyleLabel  = TEXT("Neutral");
        break;
    }

    if (StyleText)
    {
        StyleText->SetText(FText::FromString(StyleLabel));
        StyleText->SetColorAndOpacity(FSlateColor(StyleColour));
    }
}

void UConversationHUDWidget::HandleConversationStarted()
{
    UE_LOG(LogConversationHUD, Log, TEXT("HandleConversationStarted"));
    if (ConversationPanel)
        ConversationPanel->SetVisibility(ESlateVisibility::Visible);
    if (BackgroundBorder)
        BackgroundBorder->SetVisibility(ESlateVisibility::Visible);
    if (PromptText)
        PromptText->SetVisibility(ESlateVisibility::Hidden);
    if (ConvComp.IsValid() && NPCNameText)
        NPCNameText->SetText(FText::FromString(ConvComp->GetPartnerName()));

    if (ConvComp.IsValid())
    {
        if (FriendlyBtnText)  FriendlyBtnText ->SetText(FText::FromString(ConvComp->PendingFriendlyLine));
        if (NeutralBtnText)   NeutralBtnText  ->SetText(FText::FromString(ConvComp->PendingNeutralLine));
        if (DefensiveBtnText) DefensiveBtnText->SetText(FText::FromString(ConvComp->PendingDefensiveLine));
    }
}

void UConversationHUDWidget::HandleConversationEnded()
{
    UE_LOG(LogConversationHUD, Log, TEXT("HandleConversationEnded"));
    if (ConversationPanel)
        ConversationPanel->SetVisibility(ESlateVisibility::Hidden);
    if (BackgroundBorder)
        BackgroundBorder->SetVisibility(ESlateVisibility::Hidden);
    if (DialogueText)
        DialogueText->SetText(FText::FromString(TEXT("...")));
}

void UConversationHUDWidget::UpdateStats()
{
    if (!ConvComp.IsValid()) return;

    bool bIdle    = (ConvComp->CurrentState == EConversationState::Idle);
    bool bNearNPC = (ConvComp->AvailableInteractions.Num() > 0);
    if (PromptText)
        PromptText->SetVisibility((bIdle && bNearNPC) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

    if (!ConversationPanel || ConversationPanel->GetVisibility() == ESlateVisibility::Hidden)
        return;

    float Affinity = ConvComp->GetNPCAffinity();
    if (AffinityBar)
    {
        AffinityBar->SetPercent(Affinity / 100.f);
        FLinearColor BarColour = (Affinity > 60.f)
            ? FLinearColor(0.2f, 0.85f, 0.2f, 1.f)
            : (Affinity > 35.f)
                ? FLinearColor(0.9f, 0.75f, 0.1f, 1.f)
                : FLinearColor(0.9f, 0.2f, 0.2f, 1.f);
        AffinityBar->SetFillColorAndOpacity(BarColour);
    }
    if (AffinityLabel)
        AffinityLabel->SetText(FText::FromString(FString::Printf(TEXT("Affinity %.0f  "), Affinity)));

    if (StatsText)
    {
        int32   steps     = ConvComp->GetNPCLearningSteps();
        float   certainty = (1.0f - ConvComp->GetNPCEpsilon()) * 100.0f;
        FString preferred = ConvComp->GetNPCPreferredStyle();
        FString mode      = ConvComp->GetNPCReasoningMode();

        StatsText->SetText(FText::FromString(FString::Printf(
            TEXT("Q-Learning: %d exchanges  |  %.0f%% confident  |  Prefers: %s\n%s"),
            steps, certainty, *preferred, *mode)));
    }
}

void UConversationHUDWidget::OnFriendlyClicked()  { if (ConvComp.IsValid()) ConvComp->RespondFriendly();  }
void UConversationHUDWidget::OnNeutralClicked()   { if (ConvComp.IsValid()) ConvComp->RespondNeutral();   }
void UConversationHUDWidget::OnDefensiveClicked() { if (ConvComp.IsValid()) ConvComp->RespondDefensive(); }
