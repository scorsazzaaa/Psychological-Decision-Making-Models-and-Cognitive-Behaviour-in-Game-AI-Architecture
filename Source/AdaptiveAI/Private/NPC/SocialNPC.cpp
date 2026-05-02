#include "NPC/SocialNPC.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogAdaptiveAISocial, Log, All);

ASocialNPC::ASocialNPC(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ConversationSystem = CreateDefaultSubobject<UConversationSystem>(TEXT("ConversationSystem"));
    ConversationSystem->PrimaryComponentTick.bCanEverTick = true;

    bEnableConversation = true;

    ArchetypeLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ArchetypeLabel"));
    ArchetypeLabel->SetupAttachment(RootComponent);
    ArchetypeLabel->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
    ArchetypeLabel->SetHorizontalAlignment(EHTA_Center);
    ArchetypeLabel->SetVerticalAlignment(EVRTA_TextCenter);
    ArchetypeLabel->SetTextRenderColor(FColor::White);
    ArchetypeLabel->SetWorldSize(12.f);
    ArchetypeLabel->SetText(FText::GetEmpty());
}

void ASocialNPC::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogAdaptiveAISocial, Log, TEXT("SocialNPC '%s' spawned with Conversation System"), *GetName());

    if (ConversationSystem && bEnableConversation)
    {
        ConversationSystem->OnConversationStarted.AddDynamic(this, &ASocialNPC::OnConversationStarted);
        ConversationSystem->OnConversationEnded.AddDynamic(this, &ASocialNPC::OnConversationEnded);
        ConversationSystem->OnTurnCompleted.AddDynamic(this, &ASocialNPC::OnTurnCompleted);

        if (ArchetypeLabel)
        {
            FString Name;
            switch (ConversationSystem->Archetype)
            {
            case ENPCArchetype::Scholar:  Name = TEXT("Scholar");  break;
            case ENPCArchetype::Guard:    Name = TEXT("Guard");    break;
            case ENPCArchetype::Merchant: Name = TEXT("Merchant"); break;
            default:                      Name = TEXT("NPC");      break;
            }
            ArchetypeLabel->SetText(FText::FromString(Name));
        }
    }
}

void ASocialNPC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bEnableConversation && ConversationSystem)
    {
        UpdateConversation();
    }

    if (ArchetypeLabel)
    {
        APlayerCameraManager* Cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
        if (Cam)
        {
            FVector ToCamera = Cam->GetCameraLocation() - ArchetypeLabel->GetComponentLocation();
            FRotator LookAt  = ToCamera.ToOrientationRotator();
            ArchetypeLabel->SetWorldRotation(FRotator(0.f, LookAt.Yaw, 0.f));
        }
    }
}

void ASocialNPC::UpdateConversation()
{
    if (!ConversationSystem || ConversationSystem->CurrentState != EConversationState::Idle)
    {
        return;
    }

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

    AActor* ClosestPartner = nullptr;
    float ClosestDistSq = ConversationRange * ConversationRange;

    for (AActor* Actor : FoundActors)
    {
        if (Actor == this || !Actor->IsA<ASocialNPC>())
        {
            continue;
        }

        float DistSq = FVector::DistSquared(GetActorLocation(), Actor->GetActorLocation());
        if (DistSq < ClosestDistSq)
        {
            ClosestDistSq = DistSq;
            ClosestPartner = Actor;
        }
    }

    if (ClosestPartner)
    {
        UE_LOG(LogAdaptiveAISocial, Log, TEXT("SocialNPC '%s' found partner '%s' at distance %.1f"),
            *GetName(), *ClosestPartner->GetName(), FMath::Sqrt(ClosestDistSq));
        ConversationSystem->StartConversation(ClosestPartner);
    }
}

void ASocialNPC::StartConversationWith(AActor* Partner)
{
    if (ConversationSystem)
    {
        ConversationSystem->StartConversation(Partner);
        UE_LOG(LogAdaptiveAISocial, Log, TEXT("SocialNPC '%s' started conversation with '%s'"), *GetName(), *Partner->GetName());
    }
}

void ASocialNPC::EndCurrentConversation()
{
    if (ConversationSystem)
    {
        ConversationSystem->EndConversation();
        UE_LOG(LogAdaptiveAISocial, Log, TEXT("SocialNPC '%s' ended conversation"), *GetName());
    }
}

float ASocialNPC::GetAffinity() const
{
    return ConversationSystem ? ConversationSystem->CurrentAffinity : 0.0f;
}

void ASocialNPC::OnConversationStarted()
{
    UE_LOG(LogAdaptiveAISocial, Log, TEXT("SocialNPC '%s' conversation started!"), *GetName());
}

void ASocialNPC::OnConversationEnded()
{
    UE_LOG(LogAdaptiveAISocial, Log, TEXT("SocialNPC '%s' conversation ended!"), *GetName());
}

void ASocialNPC::OnTurnCompleted()
{
    UE_LOG(LogAdaptiveAISocial, Log, TEXT("SocialNPC '%s' completed a conversation turn"), *GetName());
}
