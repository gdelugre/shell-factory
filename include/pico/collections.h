#ifndef PICOLIB_COLLECTIONS_H_
#define PICOLIB_COLLECTIONS_H_

#include <initializer_list>
#include <utility>

namespace Pico {

    template <typename T, unsigned Size>
    class Array
    {
        static_assert(Size > 0, "Pico::Array must have at least one element.");

        public:
            template <typename... V>
            CONSTRUCTOR Array(V... values) : elements{ values... } {}

            METHOD T* pointer() const { return elements; }
            METHOD size_t length() const { return Size; }
            METHOD T& first() const { return elements[0]; }
            METHOD T& last() const { return elements[Size - 1]; }

            METHOD T& operator[](unsigned index) {
                assert(index < Size);
                return elements[index];
            }

            METHOD bool operator ==(Array const& o) const {
                if ( this->length() != o.length() )
                    return false;

                for ( size_t i = 0; i < length(); i++ )
                    if ( (*this)[i] != o[i] )
                        return false;

                return true;
            }

            METHOD bool operator !=(Array const& o) const {
                return !(*this == o);
            }

            METHOD T* begin() { return &elements[0]; }
            METHOD T* end() { return &elements[Size]; }

        private:
            T elements[Size];
    };

    template <typename T>
    class List
    {
        static constexpr size_t default_capacity = 10;

        public:
            CONSTRUCTOR List(Pico::Heap& heap = Pico::Heap::global(), size_t capacity = default_capacity) : heap(heap) {
                resize(capacity);
            }

            DESTRUCTOR ~List() {
                clear();
                resize(0);
            }

            METHOD T* begin() { return &storage[0]; }
            METHOD T* end() { return &storage[nr_elements]; }

            METHOD T& operator[](unsigned index) {
                assert(index < nr_elements);
                return storage[index];
            }

            METHOD void clear();
            METHOD size_t size() const { return nr_elements; }
            METHOD size_t capacity() const { return current_capacity; }
            METHOD bool empty() const { return nr_elements == 0; }
            METHOD List& insert(int pos, T const& obj);
            METHOD List& insert(int pos, T&& obj);

        private:
            size_t nr_elements = 0;
            size_t current_capacity = default_capacity;
            T *storage = nullptr;
            Pico::Heap& heap;

            METHOD void resize(size_t new_capacity);
            METHOD unsigned pos_to_offset(int pos) {
                if ( pos < 0 )
                    pos = nr_elements + pos;

                assert(pos >= 0 && pos <= nr_elements);
                return pos;
            }
    };

    template <typename T>
    void List<T>::clear()
    {
        for ( T& elem : *this ) {
            elem.~T();
        }

        nr_elements = 0;
    }

    template <typename T>
    List<T>& List<T>::insert(int pos, T const& obj)
    {
        unsigned offset = pos_to_offset(pos);

        // TODO 
         
        return *this;
    }

    template <typename T>
    void List<T>::resize(size_t new_capacity)
    {
        if ( new_capacity == current_capacity )
            return;

        if ( storage == nullptr ) {
            storage = static_cast<T *>(heap.allocate(new_capacity * sizeof(T)));
            current_capacity = new_capacity;
            return;
        }

        assert(nr_elements <= new_capacity);

        if ( new_capacity == 0 ) {
            heap.free(storage, current_capacity * sizeof(T));
            storage = nullptr;
        }
        else {
            T *new_storage = static_cast<T *>(heap.allocate(new_capacity * sizeof(T)));

            for ( size_t i = 0; i < nr_elements; i++ ) {
                new (&new_storage[i]) T( std::move(storage[i]) );

                storage[i].~T();
            }

            heap.free(storage);
            storage = new_storage;
        }

        current_capacity = new_capacity;
    }
}

#endif
