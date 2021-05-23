#pragma once

#include <string.h>

template<typename T>
class RingBufferIterator;

template<typename T>
struct RingBuffer
{
    friend class RingBufferIterator<T>;
 public:
    explicit RingBuffer(size_t size) :
        capacity_(size), empty_(true), head_idx_(capacity_ - 1)
    {
        buffer_ = new T[size];
        std::memset(buffer_, 0, sizeof(T) * size);
    }

    inline const bool IsEmpty() { return empty_; }
    inline const size_t Capacity() { return capacity_; }

    // Most recent item added to the buffer
    inline const T Head() { return buffer_[head_idx_]; }

    void Add(const T& item);

    inline const T operator[](int idx)
    {
        return buffer_[(head_idx_ + idx + 1) % capacity_];
    }

 private:
    T* buffer_;
    bool empty_;
    size_t capacity_;
    size_t head_idx_;
};

template<typename T>
void RingBuffer<T>::Add(const T& item)
{
    ++head_idx_;
    if (head_idx_ == capacity_)
    {
        head_idx_ -= capacity_;
    }
    buffer_[head_idx_] = item;
    empty_ = false;
}