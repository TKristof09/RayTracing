#pragma once
#include <utility>
#include <iostream>

class LinearAllocator
{
public:
    LinearAllocator(size_t size)
    {
        m_start = new char[size];
        m_ptr   = m_start;
        m_size  = size;
    }
    ~LinearAllocator()
    {
        delete[](char*)m_start;
    }

    template<typename T, typename... Args>
    T* Allocate(Args&&... args)
    {
        if((size_t)m_ptr + sizeof(T) > (size_t)m_start + m_size)
        {
            std::cout << "LinearAllocator out of memory" << std::endl;
            assert(false);
            return nullptr;
        }

        void* ptr = m_ptr;
        m_ptr     = (void*)((char*)m_ptr + sizeof(T));
        T* result = new(ptr) T(std::forward<Args>(args)...);
        return result;
    }

    void Reset()
    {
        m_ptr = m_start;
    }

private:
    void* m_start;
    void* m_ptr;
    size_t m_size;
};

extern LinearAllocator g_shapeAllocator;
extern LinearAllocator g_materialAllocator;
