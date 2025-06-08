#pragma once

#include <bitset>
#include <cstdint>

#include "Runtime/Base/_Module/API.h"

namespace Moyu
{

    /// Simulating reflections
    /// Not thread safe
    /// Todo: thread safe or use clang generate type infomation
    class MOYU_BASE_API Type
    {
    public:
        static constexpr uint32_t kMaxTypes = 128;

        Type() = default;
        Type(char const* name, Type* parent);

        // Not copyable
        Type(Type const&)            = delete;
        Type& operator=(Type const&) = delete;

        // Not movable
        Type(Type&&)            = delete;
        Type& operator=(Type&&) = delete;

        ~Type() = default;

        static Type* Get(char const* name);

        void        Init();
        uint32_t    GetId();
        char const* GetName() const;

        inline bool Isa(uint32_t const& other)
        {
            if (!m_inited)
            {
                Init();
            }

            return m_bits[other];
        }

        inline bool Isa(Type const& other)
        {
            if (!m_inited)
            {
                Init();
            }

            return m_bits[other.m_id];
        }

    private:
        char const* const      m_name;
        Type* const            m_parent;
        uint32_t               m_id;
        bool                   m_inited;
        std::bitset<kMaxTypes> m_bits;
    };
} // namespace Moyu
