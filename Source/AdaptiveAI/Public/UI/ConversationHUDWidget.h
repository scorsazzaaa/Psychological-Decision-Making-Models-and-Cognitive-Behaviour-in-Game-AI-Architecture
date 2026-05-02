#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Conversation/ConversationSystem.h"
#include "ConversationHUDWidget.generated.h"

class UPlayerConversationComponent;
class UTextBlock;
class UProgressBar;
class UButton;
class UVerticalBox;
class UHorizontalBox;
class UCanvasPanel;

UCLASS(BlueprintType, Blueprintable)
class ADAPTIVEAI_API UConversationHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetConversationComponent(UPlayerConversationComponent* Comp);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    void UpdateStats();

    UFUNCTION() void HandleNPCSpoke(const FString& Line, EConversationStyle Style);
    UFUNCTION() void HandleConversationStarted();
    UFUNCTION() void HandleConversationEnded();

    UFUNCTION() void OnFriendlyClicked();
    UFUNCTION() void OnNeutralClicked();
    UFUNCTION() void OnDefensiveClicked();

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* PromptText = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UVerticalBox* ConversationPanel = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    class UBorder* BackgroundBorder = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* NPCNameText = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* DialogueText = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* StyleText = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UProgressBar* AffinityBar = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* AffinityLabel = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* StatsText = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UButton* FriendlyBtn = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UButton* NeutralBtn = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UButton* DefensiveBtn = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* FriendlyBtnText = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* NeutralBtnText = nullptr;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* DefensiveBtnText = nullptr;

    TWeakObjectPtr<UPlayerConversationComponent> ConvComp;
};
