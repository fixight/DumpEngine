// Created by egors on 26.02.2026.
//

#ifndef DUMPENGINE_DSTACKALLOCATOR_H
#define DUMPENGINE_DSTACKALLOCATOR_H

#include <cstddef>
#include <cstdlib>
#include <cstdint>  // Для std::uintptr_t
#include <new>      // Для std::bad_alloc
#include <cassert>  // Для assert

using Marker = size_t;

class DStackAllocator {
private:
    void* m_start_ptr;
    size_t m_total_size;
    size_t m_offset;

public:
    DStackAllocator(size_t size) : m_total_size(size), m_offset(0) {
        m_start_ptr = std::malloc(m_total_size);
        if (m_start_ptr == nullptr) {
            throw std::bad_alloc();
        }
    }

    ~DStackAllocator() {
        std::free(m_start_ptr);
    }

    DStackAllocator(const DStackAllocator&) = delete;
    DStackAllocator& operator=(const DStackAllocator&) = delete;

    DStackAllocator(DStackAllocator&& other) noexcept
        : m_start_ptr(other.m_start_ptr), m_total_size(other.m_total_size), m_offset(other.m_offset) {
        other.m_start_ptr = nullptr;
        other.m_total_size = 0;
        other.m_offset = 0;
    }

    DStackAllocator& operator=(DStackAllocator&& other) noexcept {
        if (this != &other) {
            std::free(m_start_ptr);
            m_start_ptr = other.m_start_ptr;
            m_total_size = other.m_total_size;
            m_offset = other.m_offset;

            other.m_start_ptr = nullptr;
            other.m_total_size = 0;
            other.m_offset = 0;
        }
        return *this;
    }

    void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) {
        std::uintptr_t current_address = reinterpret_cast<std::uintptr_t>(m_start_ptr) + m_offset;

        size_t padding = 0;
        if (current_address % alignment != 0) {
            padding = alignment - (current_address % alignment);
        }

        if (m_offset + padding + size > m_total_size) {
            return nullptr;
        }

        m_offset += padding;
        std::uintptr_t next_address =

        next_address = current_address + padding;

        m_offset += size;

        return reinterpret_cast<void*>(next_address);
    }

    Marker GetMarker() const {
        return m_offset;
    }

    void FreeToMarker(Marker marker) {
        assert(marker <= m_offset && "Marker is out of bounds!");
        m_offset = marker;
    }

    void Clear() {
        m_offset = 0;
    }

    template<typename T, typename... Args>
    T* New(Args&&... args) {
        void* raw_memory = Allocate(sizeof(T), alignof(T));
        if (!raw_memory) {
            return nullptr;
        }

        return new (raw_memory) T(std::forward<Args>(args)...);
    }

    template<typename T>
    void Delete(T* object) {
        if (object) {

            object->~T();
        }
    }
};

#endif //DUMPENGINE_DSTACKALLOCATOR_H