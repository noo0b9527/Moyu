#pragma once

#include <atomic>

#include "Runtime/Base/Type/Type.h"
#include "Runtime/Base/_Module/API.h"

namespace Moyu
{
    class MOYU_BASE_API Object
    {
    public:
        static Moyu::Type s_Type;

        Object();
        Object(Object const& rhs);
        Object& operator=(Object const& rhs);

        virtual ~Object() = 0;

        /**
         * Gets the reference count of this Object.
         * @returns The reference count.
         **/
        int GetReferenceCount() const;

        /**
         * Retains the Object, i.e. increases the
         * reference count by one.
         **/
        void Retain();

        /**
         * Releases one reference to the Object, i.e. decrements the
         * reference count by one, and potentially deletes the Object
         * if there are no more references.
         **/
        void Release();

    private:
        // The reference count
        std::atomic<int> m_count;
    };

} // namespace Moyu
