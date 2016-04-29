#ifndef PICOLIB_COLLECTIONS_H_
#define PICOLIB_COLLECTIONS_H_

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
                return (this->length() == o.length()) && Memory::compare(elements, o.pointer(), sizeof(elements));
            }

            METHOD bool operator !=(Array const& o) const {
                return !(*this == o);
            }

            METHOD T* begin() { return &elements[0]; }
            METHOD T* end() { return &elements[Size]; }

        private:
            T elements[Size];
    };
}

#endif
