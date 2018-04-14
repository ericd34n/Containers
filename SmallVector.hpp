// Created by Eric Sanchez @ericd34n on 4/13/18.
// TODO reserve(), construction from std::initializer_list

#ifndef SMALLVECTOR_SMALLVECTOR_H
#define SMALLVECTOR_SMALLVECTOR_H

template <typename T>
class SmallVector {
public:
    SmallVector()
        : elements(nullptr)
        , first_free(nullptr)
        , current_capacity(nullptr)
    {
    }

    /// Copy Constructor
    SmallVector(const SmallVector&);
    /// Copy Assignment
    SmallVector& operator=(const SmallVector&);
    /// Move Constructor
    SmallVector(SmallVector&&) noexcept;
    /// Move Assignment
    SmallVector& operator=(SmallVector&&) noexcept;
    /// Destructor
    ~SmallVector();
    /// Copy
    void push_back(const T&);
    /// Move
    void push_back(T&&);

    std::size_t size() const { return first_free - elements; }
    std::size_t capacity() const { return current_capacity - first_free; }
    T* begin() const { return elements; }
    T* end() const { return first_free; }

private:
    /// allocator
    static std::allocator<T> alloc;

    /// check capacity before reallocation
    void check_then_allocate()
    {
        if (size() == capacity()) {
            reallocate();
        }
    }
    /// Utility functions for use in Copy Constructor, Assignment,
    /// and destructor
    std::pair<T*, T*> alloc_then_copy(const T*, const T*);
    /// destroy elements and free the space
    void free();
    /// allocate more space when necessary
    void reallocate();
    /// pointer to the beginning of the first element
    T* elements;
    /// pointer to the first "free" element position
    T* first_free;
    /// pointer to position one past the end of the array
    T* current_capacity;
};

template <typename T>
std::allocator<T> SmallVector<T>::alloc;

template <typename T>
std::pair<T*, T*> SmallVector<T>::alloc_then_copy(const T* begin, const T* end)
{
    /// allocate for range [e, b], being explicit by
    /// using `range`  
    auto range = end - begin;
    auto data = alloc.allocate(range);

    /// init and return a pair constructed from data and
    /// the value returned by std::uninitialized_copy() which
    /// returns an iterator to the element past the last element copied.
    return { data, std::uninitialized_copy(begin, end, data) };
};

template <typename T>
SmallVector<T>::SmallVector(const SmallVector& small_vector)
{
    auto newly_allocated_data = alloc_then_copy(small_vector.begin(), small_vector.end());
    elements = newly_allocated_data.first;
    first_free = current_capacity = newly_allocated_data.second;
}

template <typename T>
SmallVector<T>& SmallVector<T>::operator=(const SmallVector& rhs)
{
    auto newly_allocated_data = alloc_then_copy(rhs.begin(), rhs.end());
    free();
    elements = newly_allocated_data.first;
    first_free = current_capacity = newly_allocated_data.second;
    return *this;
}

template <typename T>
SmallVector<T>::SmallVector(SmallVector&& small_vector) noexcept
    : elements(small_vector.elements)
    , first_free(small_vector.first_free)
    , current_capacity(small_vector.current_capacity)
{
    /// leave moved-from small_vector in destructible state
    small_vector.elements = small_vector.first_free = small_vector.current_capacity = nullptr;
}

template <typename T>
SmallVector<T>& SmallVector<T>::operator=(SmallVector&& rhs) noexcept
{
    /// adjust for possible self assignment
    if (this != &rhs) {
        free();
        elements = rhs.elements;
        first_free = rhs.first_free;
        current_capacity = rhs.current_capacity;

        /// leave rhs in a destructible state
        rhs.elements = rhs.first_free = rhs.current_capacity = nullptr;
    }
    return *this;
}

template <typename T>
void SmallVector<T>::free()
{
    /**
     * If there are elements to destroy, destroy
     * elements beginning from the first_free, or
     * in other words, the last successfully constructed
     * element that exists.
     */
    if (elements) {
        for (auto iter = first_free; iter != elements;) {
            alloc.destroy(--iter);
        }
        /**
	 * http://en.cppreference.com/w/cpp/memory/allocator/deallocate
	 * Deallocates the storage referenced by the pointer [elements],
	 * which must be a pointer obtained by an earlier call
	 * to allocate(). The argument [current_capacity - elements]
	 * must be equal to the first argument of the call to allocate()
	 * that originally produced p; otherwise, the behavior is undefined.
	 */
        alloc.deallocate(elements, current_capacity - elements);
    }
}

/// destroy and free elements
template <typename T>
SmallVector<T>::~SmallVector() { free(); }

template <typename T>
void SmallVector<T>::reallocate()
{
    auto new_capacity = size() ? 2 * size() : 3;
    /**
	 * Allocates raw, unconstructed memory to hold n
	 * objects of type T.
	 *
	 * http://en.cppreference.com/w/cpp/memory/allocator/allocate
	 *
	 * Allocates n * sizeof(T) bytes of uninitialized
	 * storage by calling ::operator new(std::size_t)
	 * or ::operator new(std::size_t, std::align_val_t)
	 * (since C++17), but it is unspecified when and how
	 * this function is called.
	 */
    auto first = alloc.allocate(new_capacity);

    /// Iterator to the element past the last element copied.
    auto last = std::uninitialized_copy(std::make_move_iterator(begin()),
        std::make_move_iterator(end()), first);

    free();
    elements = first;
    first_free = last;
    current_capacity = elements + new_capacity;
}
template <typename T>
void SmallVector<T>::push_back(const T& lvalue_elem)
{
    check_then_allocate();
    alloc.construct(first_free++, lvalue_elem);
}

template <typename T>
void SmallVector<T>::push_back(T&& rvalue_elem)
{
    check_then_allocate();
    alloc.construct(first_free++, std::move(rvalue_elem));
}

#endif //SMALLVECTOR_SMALLVECTOR_H
