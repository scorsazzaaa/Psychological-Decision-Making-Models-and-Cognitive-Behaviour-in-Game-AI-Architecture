#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TextRenderComponent.h"
#include "Conversation/ConversationSystem.h"
#include "SocialNPC.generated.h"

UCLASS()
class ADAPTIVEAI_API ASocialNPC : public ACharacter
{
    GENERATED_BODY()

public:
    ASocialNPC(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Social")
    bool bEnableConversation = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Social")
    float ConversationRange = 500.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UConversationSystem> ConversationSystem;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UTextRenderComponent> ArchetypeLabel;

    UFUNCTION(BlueprintCallable, Category = "Debug")
    void StartConversationWith(AActor* Partner);

    UFUNCTION(BlueprintCallable, Category = "Debug")
    void EndCurrentConversation();

    UFUNCTION(BlueprintCallable, Category = "Debug")
    float GetAffinity() const;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    void UpdateConversation();

    UPROPERTY()
    TWeakObjectPtr<AActor> CurrentConversationPartner;

    UFUNCTION()
    void OnConversationStarted();

    UFUNCTION()
    void OnConversationEnded();

    UFUNCTION()
    void OnTurnCompleted();
};
