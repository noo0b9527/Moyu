#pragma once
#include "Runtime/Base/Object/Object.h"
#include "Runtime/Base/_Module/API.h"

namespace Moyu
{
    enum class Acquire
    {
        RETAIN,
        NORETAIN,
    };

    template<typename T>
    class MOYU_BASE_API StrongRef
    {
    public:
        StrongRef() : m_object(nullptr)
        {}

        explicit StrongRef(T* obj, Acquire acquire = Acquire::RETAIN) :
            m_object(obj)
        {
            if (m_object && acquire == Acquire::RETAIN)
            {
                m_object->Retain();
            }
        }

        StrongRef(StrongRef const& rhs) : m_object(rhs.Get())
        {
            if (m_object)
            {
                m_object->Retain();
            }
        }

        StrongRef(StrongRef&& rhs) : m_object(rhs.m_object)
        {
            rhs.m_object = nullptr;
        }

        StrongRef& operator=(StrongRef const& rhs)
        {
            if (this == &rhs)
            {
                return *this;
            }
            Set(rhs.Get());
            return *this;
        }

        T* operator->() const
        {
            return m_object;
        }

        explicit operator T*() const
        {
            return m_object;
        }

        explicit operator bool() const
        {
            return m_object != nullptr;
        }

        ~StrongRef()
        {
            if (m_object)
            {
                m_object->Release();
            }
        }

        inline void Set(T* obj, Acquire acquire = Acquire::RETAIN)
        {
            if (obj && acquire == Acquire::RETAIN)
            {
                obj->Retain();
            }

            if (m_object)
            {
                m_object->Release();
            }

            m_object = obj;
        }

        inline T* Get()
        {
            return m_object;
        }

    private:
        T* m_object;
    };

} // namespace Moyu
