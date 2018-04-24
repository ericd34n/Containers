#include <iostream>
#include <sstream>
#include <string>
#include <vector>

enum class Mode { Min, Max };

template <typename T, Mode M>
struct HeapBase {
    /// default
    HeapBase() = default;
    /// Heap does not acquire resources
    ~HeapBase() = default;
    /// copy constructor
    HeapBase(const HeapBase&);
    /// copy assignment
    HeapBase& operator=(const HeapBase&);
    ///  move constructor
    HeapBase(HeapBase&&) noexcept = default;
    /// move assignment: use vector move assignment
    HeapBase& operator=(HeapBase&&) noexcept = default;

    /// get a reference to the root
    T& top();
    /// get reference to parent node
    T& parent(int index) { return items[get_parent_index(index)]; }
    /// get reference to left_child node
    T& left_child(int index) { return items[get_left_child(index)]; }
    /// get reference to right_child node
    T& right_child(int index) { return items[get_right_child(index)]; }
    void max_heapify();
    void build_max_heap();

    /// container for 'nodes' of the heap allow vector
    /// to handle allocation if the heap needs to grow
    std::vector<T> items;
    /**
	 * sometimes it is helpful if indexes are signed integer values
	 * I currently am unsure if I am going to write an algorithm where
	 * a left index and right index may cross and either left or right
	 * become negative, sometimes it can be a helpful loop invariant
	 */
    /// size of the heap
    int size = 0;
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

    /// extract root of heap
    T extract();
    /// add node to the heap
    void add(const T&);
    /// heapify_down: reorder heap from the bottom up
    void heapify_up();
    /// heapify_down: reorder heap from the top down
    void heapify_down();
    
    void check_in_range(const std::string&, const char*, const char*);
    void not_in_range(const std::string&, const char*, const char*);
    virtual void print() = 0;
};

template <typename T, Mode M>
HeapBase<T, M>::HeapBase(const HeapBase& rhs)
    : items(rhs.items)
    , size(rhs.size)
{
}

template <typename T, Mode M>
HeapBase<T, M>& HeapBase<T, M>::operator=(const HeapBase<T, M>& rhs)
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

template <typename T, Mode M>
void HeapBase<T, M>::not_in_range(const std::string& msg, const char* func, const char* sig)
{
    throw std::length_error(error_msg(msg, func, sig));
}

template <typename T, Mode M>
void HeapBase<T, M>::check_in_range(const std::string& msg, const char* func, const char* sig)
{
    if (items.empty()) {
        not_in_range(msg, func, sig);
    }
}

template <typename T, Mode M>
T& HeapBase<T, M>::top()
{
    check_in_range("empty heap", __func__, __PRETTY_FUNCTION__);
    return items[0];
}

template <typename T, Mode M>
void HeapBase<T, M>::heapify_up()
{
    auto index = size - 1;
    while (has_parent(index) && parent(index) > items[index]) {
        std::swap(parent(index), items[index]);
        index = get_parent_index(index);
    }
}

template <typename T, Mode M>
void HeapBase<T, M>::heapify_down()
{
    int index = 0;
    while (has_left_child(index)) {
        int smaller_child_index = get_left_child(index);

        if (has_right_child(index) && right_child(index) > left_child(index)) {
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

template <typename T, Mode M>
void HeapBase<T, M>::add(const T& elem)
{
    items.push_back(elem);
    ++size;
    heapify_up();
}

template <typename T, Mode M>
T HeapBase<T, M>::extract()
{
    check_in_range("empty heap", __func__, __PRETTY_FUNCTION__);
    auto item = items[0];
    items[0] = items[--size];
    items.pop_back();
    heapify_down();
    return item;
}

int left(int index) { return (2 * index) + 1; }
int right(int index) { return (2 * index) + 2; }

template <typename T>
void _max_heapify(std::vector<T>& A, int index)
{
    int l = left(index);
    int r = right(index);
    int largest = 0;
    if (l < A.size() && A[l] > A[index]) {
        largest = l;
    } else {
        largest = index;
    }

    if (r < A.size() && A[r] > A[largest]) {
        largest = r;
    }

    if (largest != index) {
        std::swap(A[index], A[largest]);
        _max_heapify(A, largest);
    }
}

template <typename T>
void _build_max_heap(std::vector<T>& A)
{
    for (auto index = static_cast<int>(A.size() / 2); index >= 0; --index) {
        _max_heapify(A, index);
    }
}

template <typename T, Mode M>
void HeapBase<T, M>::max_heapify()
{
    _max_heapify(items, 0);
}

template <typename T, Mode M>
void HeapBase<T, M>::build_max_heap()
{
    _build_max_heap(items);
}

template <typename T, Mode M>
struct Heap : HeapBase<T, M> {
};

template <typename T>
struct Heap<T, Mode::Min> : HeapBase<T, Mode::Min> {
    void print() override;
};

template <typename T>
void Heap<T, Mode::Min>::print()
{
    std::cout << "Min" << std::endl;
}

template <typename T>
struct Heap<T, Mode::Max> : HeapBase<T, Mode::Min> {
    void print() override;
};

template <typename T>
void Heap<T, Mode::Max>::print()
{
    std::cout << "Max" << std::endl;
}

int main()
{
    Heap<int, Mode::Min> min_heap;
    min_heap.print();
    min_heap.add(7);
    min_heap.add(0);
    min_heap.add(2);

    Heap<int, Mode::Max> max_heap;
    max_heap.add(7);
    max_heap.add(1010);
    max_heap.add(3);
    max_heap.print();
    //min_heap.print();
    //Heap<int, Mode::Max> max_heap;
    //max_heap.print();
    
    return 0;
}
