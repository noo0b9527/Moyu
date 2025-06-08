#include "Runtime/Base/Module/Module.h"
#include "Runtime/Base/Memory/Memory.h"
#include "Runtime/Base/MoyuMacro.h"
#include "Runtime/Base/Type/Type.h"

#include <map>
#include <string>
#include <utility>

using ModuleRegistry = std::map<std::string, Moyu::Module*>;

// The registry must be dynamically managed, because some modules
// (the math module) are static globals that are not guaranteed to
// be destroyed before other static globals.
static ModuleRegistry* s_Registry = nullptr;

static ModuleRegistry& RegistryInstance()
{
    if (!s_Registry)
        s_Registry = Moyu::New<ModuleRegistry>();

    return *s_Registry;
}

static void FreeEmptyRegistry()
{
    if (s_Registry && s_Registry->empty())
    {
        Moyu::Delete(s_Registry);
        s_Registry = nullptr;
    }
}

namespace Moyu
{
    Moyu::Type Module::s_Type("Module", &Object::s_Type);
    Module*    Module::m_instances[] = {};

    Module::Module(ModuleType moduleType, char const* name) :
        m_moduleType(moduleType), m_name(name)
    {
        RegisterInstance(this);
    }

    Module::~Module()
    {
        ModuleRegistry& registry = RegistryInstance();

        // We can't use the overridden Module::GetName() in this destructor.
        for (auto it = registry.begin(); it != registry.end(); ++it)
        {
            if (it->second == this)
            {
                registry.erase(it);
                break;
            }
        }

        // Same deal with Module::GetModuleType().
        for (Module* instance : m_instances)
        {
            if (instance == this)
            {
                instance = nullptr;
                break;
            }
        }

        FreeEmptyRegistry();
    }

    Module* Module::GetInstance(std::string const& name)
    {
        ModuleRegistry& registry = RegistryInstance();

        auto it = registry.find(name);
        if (registry.end() == it)
        {
            return nullptr;
        }

        return it->second;
    }

    void Module::RegisterInstance(Module* instance)
    {
        MOYU_ASSERT(instance != nullptr);

        std::string name(instance->GetName());

        ModuleRegistry& registry = RegistryInstance();

        auto it = registry.find(name);

        if (it != registry.end())
        {
            if (it->second == instance)
            {
                return;
            }

            MOYU_ASSERT(false);
        }
        registry.insert(std::make_pair(name, instance));

        ModuleType type = instance->GetModuleType();
        MOYU_ASSERT(type != ModuleType::Unkown);

        if (!m_instances[type])
        {
            //Todo: Warning
        }

        m_instances[type] = instance;
    }


} // namespace Moyu
