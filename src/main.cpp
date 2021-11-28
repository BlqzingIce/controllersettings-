#include "main.hpp"
#include "config.hpp"

#include "GlobalNamespace/OculusVRHelper.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/XR/XRNode.hpp"

#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "config-utils/shared/config-utils.hpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

MAKE_HOOK_MATCH(
    AdjustControllerTransform, 
    &GlobalNamespace::OculusVRHelper::AdjustControllerTransform, 
    void, 
    GlobalNamespace::OculusVRHelper* self,
    UnityEngine::XR::XRNode node, 
    UnityEngine::Transform* transform, 
    UnityEngine::Vector3 position, 
    UnityEngine::Vector3 rotation
    ) 
{
    if(getconfig().rotfix.GetValue())
    { 
        if(node == UnityEngine::XR::XRNode::LeftHand) 
        {
            rotation.z = -rotation.z;
        }
    }

    if(getconfig().disableBaseAdjustment.GetValue())
    { 
        rotation.x += 40;
        position.z -= 0.055;
    }

    AdjustControllerTransform(self, node, transform, position, rotation);

    if(getconfig().adjustmentOrder.GetValue())
    { 
        transform->Rotate(UnityEngine::Vector3(-rotation.x, -rotation.y, -rotation.z));
        transform->Translate(UnityEngine::Vector3(-position.x, -position.y, -position.z));

        transform->Rotate(UnityEngine::Vector3(0, 0, rotation.z));
        transform->Translate(position);
        transform->Rotate(UnityEngine::Vector3(rotation.x, rotation.y, 0));
    }
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load();
    getconfig().Init(info);
    getConfig().Reload();
    getConfig().Write();

    getLogger().info("Completed setup!");
}

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    if(firstActivation) 
    {
        UnityEngine::GameObject* container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());

        UnityEngine::UI::Toggle* rotfix = AddConfigValueToggle(container->get_transform(), getconfig().rotfix);
        QuestUI::BeatSaberUI::AddHoverHint(rotfix->get_gameObject(), "Correctly mirrors the z rotation to the left controller");

        UnityEngine::UI::Toggle* disableBaseAdjustment = AddConfigValueToggle(container->get_transform(), getconfig().disableBaseAdjustment);
        QuestUI::BeatSaberUI::AddHoverHint(disableBaseAdjustment->get_gameObject(), "Disables hidden controller adjustments (-40 degrees to x rotation and +5.5 cm to z position)");

        UnityEngine::UI::Toggle* adjustmentOrder = AddConfigValueToggle(container->get_transform(), getconfig().adjustmentOrder);
        QuestUI::BeatSaberUI::AddHoverHint(adjustmentOrder->get_gameObject(), "Instead of applying rotation and then position, applies Z rotation, position, and then XY rotations.");
    }
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    QuestUI::Init();
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), AdjustControllerTransform);
    getLogger().info("Installed all hooks!");
}