#include <iostream>
#include <vector>

template <typename T>
class Heap {
private:
    std::vector<T> items;
    bool sorted = false;
    std::size_t heap_size = 0;

public:
    Heap() = default;
    explicit Heap(const std::vector<T>& items);
    Heap(const Heap&) = default;
    Heap& operator=(const Heap&) = default;
    Heap(Heap&&) noexcept = default;
    Heap& operator=(Heap&&) noexcept = default;
    ~Heap() = default;

    T& operator[](std::size_t i) { return items[i]; }
    const T& operator[](std::size_t i) const { return items[i]; }

    void max_heapify(Heap&, std::size_t);
    void build_max_heap(Heap&);
    void insert(const T&);

    T extract_max();
    T max();
    void sort();

    std::size_t size() const { return heap_size; }

    void display();
};

std::size_t parent(int index)
{
    return index > 0 ? unsigned(index - 1) / 2 : 0;
}

std::size_t left(std::size_t index) { return (2 * index) + 1; }
std::size_t right(std::size_t index) { return (2 * index) + 2; }

template <typename T>
Heap<T>::Heap(const std::vector<T>& items)
    : items(items)
    , heap_size(items.size())
{
    build_max_heap(*this);
}

template <typename T>
void Heap<T>::max_heapify(Heap& heap, std::size_t index)
{
    auto L = left(index), R = right(index), largest = index;
    if (L < heap_size && heap[L] > heap[index])
        largest = L;
    else
        largest = index;

    if (R < heap_size && heap[R] > heap[largest])
        largest = R;

    if (largest != index) {
        std::swap(heap[index], heap[largest]);
        max_heapify(heap, largest);
    }
}

template <typename T>
void Heap<T>::build_max_heap(Heap& heap)
{
    for (auto i = static_cast<int>(heap.heap_size / 2); i >= 0; --i) {
        max_heapify(heap, i);
    }
}

template <typename T>
void heap_up(Heap<T>& heap, std::size_t index)
{
    while (index >= 0 && heap[index] > heap[parent(index)]) {
        std::swap(heap[index], heap[parent(index)]);
        index = parent(index);
    }
}

template <typename T>
void Heap<T>::insert(const T& elem)
{
    if (sorted) {
        build_max_heap(*this);
        sorted = false;
    }

    items.emplace_back(elem);
    heap_size = items.size();
    heap_up(*this, heap_size - 1);
}

template <typename T>
T Heap<T>::extract_max()
{
    std::swap(items.front(), items.back());
    auto elem = items.back();
    items.pop_back();
    heap_size = items.size();
    build_max_heap(*this);
    return elem;
}

template <typename T>
void Heap<T>::sort()
{
    auto original_size = heap_size;
    build_max_heap(*this);
    for (auto i = static_cast<int>(heap_size - 1); i > 0; --i) {
        std::swap(items[0], items[i]);
        heap_size--;
        max_heapify(*this, 0);
    }

    sorted = true;
    heap_size = original_size;
}

template <typename T>
T Heap<T>::max()
{
    return items.front();
}

template <typename T>
void Heap<T>::display()
{
    for (auto& elem : items) {
        std::cout << elem << " ";
    }
    std::cout << '\n';
}

int main()
{
    Heap<int> heap({ 1, 2, 3, 4, 5 });
    heap.display();
    heap.insert(6);
    heap.display();
    heap.sort();
    heap.display();
    /// re-heaped-up on subsequent
    /// insert.
    heap.insert(9);
    heap.display();
    return 0;
}
