//
// Created by egors on 26.02.2026.
//

#ifndef DUMPENGINE_DLINEARALLOCATOR_H
#define DUMPENGINE_DLINEARALLOCATOR_H
#include <cstddef>
#include <cstdlib>
#include <new>
#include <utility>


class DLinearAllocator {

private:
    void* m_start_ptr;
    size_t m_total_size;
    size_t m_offset;

public:
    DLinearAllocator(size_t size) : m_total_size(size), m_offset(0) {
        m_start_ptr = std::malloc(size);
    }

    ~DLinearAllocator() {
        free(m_start_ptr);
    }

    DLinearAllocator(const DLinearAllocator&) = delete;
    DLinearAllocator& operator=(const DLinearAllocator&) = delete;

    void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) {
        size_t current_address = reinterpret_cast<size_t>(m_start_ptr) + m_offset;

        size_t padding = 0;
        if (current_address % alignment != 0) {
            padding = alignment - (current_address % alignment);
        }

        if (m_offset + padding + size > m_total_size) {
            return nullptr;
        }

        m_offset += padding;
        size_t next_address = current_address + padding;
        m_offset += size;

        return reinterpret_cast<void*>(next_address);
    }

    void Reset() {
        m_offset = 0;
    }

    template <class T, typename... Args>
    T* AllocateNew( Args&&... args) {
        void* memory = Allocate(sizeof(T), alignof(T));


        return new (memory) T(std::forward<Args>(args)...);
    }

    template <class T>
    void DeallocateDelete(T* object) {
        if (object) {
            object->~T();
        }
    }
};


#endif //DUMPENGINE_DLINEARALLOCATOR_H