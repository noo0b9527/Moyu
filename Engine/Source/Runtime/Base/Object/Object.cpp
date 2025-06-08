#include "Runtime/Base/Object/Object.h"
#include "Runtime/Base/Memory/Memory.h"

namespace Moyu
{
    Moyu::Type Object::s_Type("Object", nullptr);

    Object::Object() : m_count(1)
    {}

    Object::~Object() = default;

    Object::Object(Object const& rhs)
    {
        *this = rhs;
    }
    Object& Object::operator=(Object const& rhs)
    {
        if (this != &rhs)
        {
            m_count.store(1);
        }
        return *this;
    }

    int Object::GetReferenceCount() const
    {
        return m_count;
    }

    void Object::Retain()
    {
        m_count.fetch_add(1, std::memory_order_relaxed);
    }

    void Object::Release()
    {
        if (m_count.fetch_sub(1, std::memory_order_release) == 1)
        {
            std::atomic_thread_fence(std::memory_order_acquire);
            Object* self = this;
            Moyu::Delete(self);
        }
    }
} // namespace Moyu
