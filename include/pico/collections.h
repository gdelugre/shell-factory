#ifndef PICOLIB_COLLECTIONS_H_
#define PICOLIB_COLLECTIONS_H_

#include <initializer_list>
#include <utility>

namespace Pico {

    template <typename T>
    class Collection
    {
        public:
            METHOD size_t   length() const { return nr_elements; }
            METHOD T&       empty() const { return nr_elements == 0; }
            METHOD T&       first() const { return &storage[0]; }
            METHOD T&       last() const { return &storage[nr_elements - 1]; }
            METHOD T&       operator [](unsigned index) {
                assert(index < nr_elements);
                return storage[index];
            }

            METHOD bool     operator ==(Collection const& o) const {
                if ( this->length() != o.length() )
                    return false;

                for ( size_t i = 0; i < this->length(); i++ )
                    if ( (*this)[i] != o[i] )
                        return false;

                return true;
            }

            METHOD bool     operator !=(Collection const& o) const {
                return !(*this == o);
            }

            METHOD T*       begin() { return &storage[0]; }
            METHOD T*       end() { return &storage[nr_elements]; }

        protected:
            Collection() = default;
            Collection(T *elements, size_t nr_elements) : storage(elements), nr_elements(nr_elements) {}

            T *storage = nullptr;
            size_t nr_elements = 0;
    };

    template <typename T, unsigned Size>
    class Array : public Collection<T>
    {
        static_assert(Size > 0, "Pico::Array must have at least one element.");

        public:
            template <typename... V>
            CONSTRUCTOR Array(V... values) : Collection<T>(elements, Size), elements{ values... } {}

            METHOD T* pointer() const { return elements; }

        private:
            T elements[Size];
    };

    template <typename T>
    class List : public Collection<T>
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

            METHOD void clear();
            METHOD size_t capacity() const { return current_capacity; }
            METHOD List& insert(int pos, T const& obj);
            METHOD List& insert(int pos, T&& obj);

        private:
            size_t current_capacity = default_capacity;
            Pico::Heap& heap;

            METHOD void resize(size_t new_capacity);
            METHOD unsigned pos_to_offset(int pos) {
                if ( pos < 0 )
                    pos = this->nr_elements + pos;

                assert(pos >= 0 && pos <= this->nr_elements);
                return pos;
            }
    };

    template <typename T>
    void List<T>::clear()
    {
        for ( T& elem : *this ) {
            elem.~T();
        }

        this->nr_elements = 0;
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

        if ( this->storage == nullptr ) {
            this->storage = static_cast<T *>(heap.allocate(new_capacity * sizeof(T)));
            current_capacity = new_capacity;
            return;
        }

        assert(this->nr_elements <= new_capacity);

        if ( new_capacity == 0 ) {
            heap.free(this->storage, current_capacity * sizeof(T));
            this->storage = nullptr;
        }
        else {
            T *new_storage = static_cast<T *>(heap.allocate(new_capacity * sizeof(T)));

            for ( size_t i = 0; i < this->nr_elements; i++ ) {
                new (&new_storage[i]) T( std::move(this->storage[i]) );

                this->storage[i].~T();
            }

            heap.free(this->storage);
            this->storage = new_storage;
        }

        current_capacity = new_capacity;
    }
}

#endif
