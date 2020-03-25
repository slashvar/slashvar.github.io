#pragma once

template <typename T>
class my_pointer
{
public:
    my_pointer() = default;
    // Construct by copy of the original value
    my_pointer(const T& val) : data_(new T(val)) {}
    ~my_pointer()
    {
        if (data_) {
            delete data_;
        }
    }

    my_pointer(const my_pointer&) = delete;
    my_pointer& operator=(const my_pointer&) = delete;

    my_pointer(my_pointer&& other) : data_(other.data_)
    {
        other.data_ = nullptr;
    }

    my_pointer& operator=(my_pointer&& other)
    {
        if (data_) {
            delete data_;
        }
        data_       = other.data_;
        other.data_ = nullptr;
        return *this;
    }

    T& operator*()
    {
        return *data_;
    }

    const T& operator*() const
    {
        return *data_;
    }

    T* operator->()
    {
        return data_;
    }

    const T* operator->() const
    {
        return data_;
    }

    void reset()
    {
        if (data_) {
            delete data_;
        }
        data_ = nullptr;
    }

private:
    T* data_ = nullptr;
};
