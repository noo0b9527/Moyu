#include "Runtime/Base/Type/Type.h"

#include <unordered_map>

namespace Moyu
{
    static std::unordered_map<std::string, Type*> s_Types {};
    static std::uint32_t                          s_NextId = 1;

    Type::Type(char const* name, Type* parent) :
        m_name(name), m_parent(parent), m_id(0), m_inited(false)
    {}

    Type* Type::Get(char const* name)
    {
        auto pos = s_Types.find(name);
        if (pos == s_Types.end())
        {
            return nullptr;
        }

        return pos->second;
    }

    void Type::Init()
    {
        if (m_inited)
        {
            return;
        }

        s_Types[m_name] = this;
        m_id            = s_NextId++;
        m_bits[m_id]    = true;
        m_inited        = true;

        if (!m_parent)
        {
            return;
        }

        if (!m_parent->m_inited)
        {
            m_parent->Init();
        }

        m_bits |= m_parent->m_bits;
    }

    uint32_t Type::GetId()
    {
        if (!m_inited)
        {
            Init();
        }

        return m_id;
    }

    char const* Type::GetName() const { return m_name; }
} // namespace Moyu
