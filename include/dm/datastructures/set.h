/*
 * Copyright 2014-2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_SET_H_HEADER_GUARD
#define DM_SET_H_HEADER_GUARD

#include <stdint.h> // uint32_t
#include <new>      // placement-new

#include "common.h" // Heap alloc utils.

#include "../common/common.h" // DM_INLINE
#include "../check.h"         // DM_CHECK

#include "../../../3rdparty/bx/allocator.h" // bx::ReallocatorI

namespace dm
{
    // Based on: http://research.swtch.com/sparse

    template <uint16_t MaxValueT>
    struct SetT
    {
        SetT()
        {
            m_num = 0;
        }

        #include "set_inline_impl.h"

        uint16_t count() const
        {
            return m_num;
        }

        uint16_t max() const
        {
            return MaxValueT;
        }

    private:
        uint16_t m_num;
        uint16_t m_values[MaxValueT];
        uint16_t m_indices[MaxValueT];
    };

    struct Set
    {
        // Uninitialized state, init() needs to be called !
        Set()
        {
            m_values = NULL;
            m_indices = NULL;
        }

        Set(uint16_t _max, bx::ReallocatorI* _reallocator)
        {
            init(_max, _reallocator);
        }

        Set(uint16_t _max, void* _mem, bx::AllocatorI* _allocator)
        {
            init(_max, _mem, _allocator);
        }

        ~Set()
        {
            destroy();
        }

        // Allocates memory internally.
        void init(uint16_t _max, bx::ReallocatorI* _reallocator)
        {
            m_num = 0;
            m_max = _max;
            m_values = (uint16_t*)BX_ALLOC(_reallocator, sizeFor(_max));
            m_indices = m_values + _max;
            m_reallocator = _reallocator;
            m_cleanup = true;
        }

        enum
        {
            SizePerElement = 2*sizeof(uint16_t),
        };

        static inline uint32_t sizeFor(uint16_t _max)
        {
            return _max*SizePerElement;
        }

        // Uses externally allocated memory.
        void* init(uint16_t _max, void* _mem, bx::AllocatorI* _allocator = NULL)
        {
            m_num = 0;
            m_max = _max;
            m_values = (uint16_t*)_mem;
            m_indices = m_values + _max;
            m_allocator = _allocator;
            m_cleanup = false;

            void* end = (void*)((uint8_t*)_mem + sizeFor(_max));
            return end;
        }

        void destroy()
        {
            if (m_cleanup && NULL != m_values)
            {
                BX_FREE(m_reallocator, m_values);
                m_values = NULL;
                m_indices = NULL;
            }
        }

        bool isInitialized() const
        {
            return (NULL != m_values);
        }

        #include "set_inline_impl.h"

        uint16_t count() const
        {
            return m_num;
        }

        uint16_t max() const
        {
            return m_max;
        }

        bx::AllocatorI* allocator()
        {
            return m_allocator;
        }

    private:
        uint16_t m_max;
        uint16_t m_num;
        uint16_t* m_values;
        uint16_t* m_indices;
        union
        {
            bx::AllocatorI*   m_allocator;
            bx::ReallocatorI* m_reallocator;
        };
        bool m_cleanup;
    };

} // namespace dm

#endif // DM_SET_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
