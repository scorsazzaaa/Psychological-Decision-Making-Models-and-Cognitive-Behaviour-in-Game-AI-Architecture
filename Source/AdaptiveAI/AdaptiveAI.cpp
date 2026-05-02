#include "AdaptiveAI.h"
#include "Modules/ModuleManager.h"

class FAdaptiveAIModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("Adaptive AI Module Started"));
    }

    virtual void ShutdownModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("Adaptive AI Module Shutdown"));
    }
};

IMPLEMENT_MODULE(FAdaptiveAIModule, AdaptiveAI)
