#pragma once

#include <cstdint>

#include <string>

#include "Runtime/Base/Object/Object.h"
#include "Runtime/Base/Type/Type.h"
#include "Runtime/Base/_Module/API.h"

namespace Moyu
{
    class MOYU_BASE_API Module
    {
    public:
        enum ModuleType
        {
            Unkown = 0,
            Audio,
            Data,
            Event,
            FileSystem,
            Font,
            Graphics,
            Image,
            // Joystick,
            // Math,
            Keyboard,
            Mouse,
            // Physics,
            // Sensor,
            Sound,
            Thread,
            Timer,
            // Touch,
            // Video,
            Window,
            Max,
        };

        static Moyu::Type s_Type;

        Module(ModuleType moduleType, char const* name);
        virtual ~Module();

        /**
         * Gets the base type of the module.
         **/
        inline ModuleType GetModuleType() const
        {
            return m_moduleType;
        }

        /**
         * Gets the name of the module. This is used in case of errors
         * and other messages.
         *
         * @return The full name of the module, eg. Moyu.xxx.xxx.
         **/
        inline const char* GetName() const
        {
            return m_name.c_str();
        }

        /**
         * Retrieve module instance from internal registry. May return NULL
         * if module not registered.
         * @param name The full name of the module.
         * @return Module instance or NULL if the module is not registered.
         */
        static Module* GetInstance(std::string const& name);

        /**
         * Retrieve module instance from the internal registry using the base
         * module type. May return null if the module is not registered.
         * @param type The base type of the module.
         **/
        template<typename T>
        static T* GetInstance(ModuleType type)
        {
            return type != ModuleType::Unkown ?
                       static_cast<T*>(m_instances[type]) :
                       nullptr;
        }

    private:
        static void RegisterInstance(Module* instance);

        ModuleType     m_moduleType;
        std::string    m_name;
        static Module* m_instances[ModuleType::Max];
    };

} // namespace Moyu
