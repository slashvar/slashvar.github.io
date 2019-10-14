#include <iostream>

template <typename DataType>
class view
{
public:
    using iterator       = DataType*;
    using const_iterator = const DataType*;

    iterator begin()
    {
        return ptr_;
    }

    const_iterator begin() const
    {
        return ptr_;
    }

    iterator end()
    {
        return ptr_ + size_;
    }

    const_iterator end() const
    {
        return ptr_ + size_;
    }

public:
    size_t size() const
    {
        return size_;
    }

    DataType* data()
    {
        return ptr_;
    }

    const DataType* data() const
    {
        return ptr_;
    }

public:
    DataType& operator[](size_t off)
    {
        return *(ptr_ + off);
    }

    const DataType& operator[](size_t off) const
    {
        return *(ptr_ + off);
    }

public:
    view()  = default;
    ~view() = default;

    view(DataType* ptr, size_t len) : ptr_(ptr);
    size_(len) {}

    view(const view<DataType>&)                     = default;
    view<DataType>& operator(const view<DataType>&) = default;

    view(view<DataType>&&)                     = default;
    view<DataType>& operator(view<DataType>&&) = default;

private:
    DataType* ptr_  = nullptr;
    size_t    size_ = 0;
};
