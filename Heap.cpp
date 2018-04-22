/// Eric Sanchez @ericdeansanchez 

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

template <typename T>
class Heap {
public:
    /// default
    Heap() = default;
    /// Heap does not acquire resources
    ~Heap() = default;
    /// copy constructor
    Heap(const Heap&);
    /// copy assignment
    Heap& operator=(const Heap&);
    ///  move constructor
    Heap(Heap&&) noexcept = default;
    /// move assignment: use vector move assignment
    Heap& operator=(Heap&&) noexcept = default;

    /// get a reference to the root
    T& top();
    /// get reference to parent node
    T& parent(int index) { return items[get_parent_index(index)]; }
    /// get reference to left_child node
    T& left_child(int index) { return items[get_left_child(index)]; }
    /// get reference to right_child node
    T& right_child(int index) { return items[get_right_child(index)]; }
    /// extract root of min_heap
    T extract_min();
    /// extract root of max_heap
    T extract_max();

    /// add node to the heap
    void add(const T&);
    void heapify_up();
    void heapify_down();

    void max_heapify(int);

private:
    /// container for 'nodes' of the heap allow vector
    /// to handle allocation if the heap needs to grow
    std::vector<T> items;
    /**
     * sometimes it is helpful if indexes are signed integer values
     * I currently am unsure if I am going to write an algorithm where
     * a left index and right index may cross and either left or right
     * become negative, sometimes it can be a helpful loop invariant
     */
    int size = 0;
    /// size of the underlying vector
    int heap_capacity() { return static_cast<int>(items.capacity()); }
    /// get integer index of left child
    int get_left_child(int parent_index) { return (2 * parent_index) + 1; };
    /// get integer index of right child
    int get_right_child(int parent_index) { return (2 * parent_index) + 2; }
    /// get parent index of right child
    int get_parent_index(int child_index) { return (child_index - 1) / 2; };

    /// is left child index in range [0, size)
    bool has_left_child(int index) { return get_left_child(index) < size; }
    /// is right child index in range [0, size)
    bool has_right_child(int index) { return get_right_child(index) < size; }
    /// is parent index in range [0, size), if at index zero it is the root
    bool has_parent(int index) { return get_parent_index(index) >= 0; }

    /* utlity functions */
    /// if heap.empty() returns true, alert operation on index not_in_range
    void check_in_range(const std::string&, const char*, const char*);
    void not_in_range(const std::string&, const char*, const char*);
};

template <typename T>
Heap<T>::Heap(const Heap& rhs)
    : items(rhs.items)
    , size(rhs.size)
{
}

template <typename T>
Heap<T>& Heap<T>::operator=(const Heap<T>& rhs)
{
    items = rhs.items;
    size = rhs.size;
    return *this;
}

std::string error_msg(const std::string& msg, const char* func, const char* sig)
{
    auto function = std::string(func);
    auto signature = std::string(sig);
    return msg + ": cannot " + func + "()\n" + signature;
}

template <typename T>
void Heap<T>::not_in_range(const std::string& msg, const char* func, const char* sig)
{
    throw std::length_error(error_msg(msg, func, sig));
}

template <typename T>
void Heap<T>::check_in_range(const std::string& msg, const char* func, const char* sig)
{
    if (items.empty()) {
        not_in_range(msg, func, sig);
    }
}

template <typename T>
T& Heap<T>::top()
{
    check_in_range("empty heap", __func__, __PRETTY_FUNCTION__);
    return items[0];
}

template <typename T>
void Heap<T>::heapify_up()
{
    auto index = size - 1;
    while (has_parent(index) && parent(index) > items[index]) {
        std::swap(parent(index), items[index]);
        index = get_parent_index(index);
    }
}

template <typename T>
void Heap<T>::heapify_down()
{
    int index = 0;
    while (has_left_child(index)) {
        int smaller_child_index = get_left_child(index);

        if (has_right_child(index) && right_child(index) < left_child(index)) {
            smaller_child_index = get_right_child(index);
        }

        if (items[index] < items[smaller_child_index]) {
            break;
        } else {
            std::swap(items[smaller_child_index], items[index]);
        }

        index = smaller_child_index;
    }
}

template <typename T>
T Heap<T>::extract_min()
{
    check_in_range("empty heap", __func__, __PRETTY_FUNCTION__);
    auto item = items[0];
    items[0] = items[--size];
    items.pop_back();
    heapify_down();
    return item;
}

template <typename T>
void Heap<T>::add(const T& elem)
{
    items.push_back(elem);
    ++size;
    heapify_up();
}

int main()
{
    Heap<int> heap;

    heap.add(6);
    std::cout << heap.extract_min() << std::endl;
    heap.add(7);
    heap.add(0);
    heap.add(19);
    heap.add(-1);
    heap.add(-89);
    return 0;
}
