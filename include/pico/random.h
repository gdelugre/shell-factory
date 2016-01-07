#ifndef PICOLIB_RANDOM_H_
#define PICOLIB_RANDOM_H_

#include <cstdint>
#include <algorithm>

namespace Pico {

    template <typename T1, typename T2, bool cmp>
    struct min_type_helper;

    template <typename T1, typename T2>
    struct min_type_helper<T1, T2, true>
    {
        using type = T1;
    };

    template <typename T1, typename T2>
    struct min_type_helper<T1, T2, false>
    {
        using type = T2;
    };

    //
    // Select the smallest type between T1 and T2.
    //
    template <typename T1, typename T2>
    struct min_type
    {
        using type = typename min_type_helper<T1, T2, sizeof(T1) < sizeof(T2)>::type;
    };

    //
    // Generic linear congruential random number generator.
    //
    template <typename StateType, long A, long C, unsigned OutputBits, unsigned OutputShift>
    class LCG
    {
        static_assert(OutputBits > 0, "Number of output bits must be positive.");
        static_assert(OutputShift < sizeof(StateType) * 8, "Output shift too big.");
        static_assert(OutputBits <= sizeof(StateType) * 8 - OutputShift, "RNG state is too small for output parameters.");

        using state_t = StateType;
        static constexpr auto a = A;
        static constexpr auto c = C;
        static constexpr auto output_shift = OutputShift;
        static constexpr state_t output_mask = (static_cast<state_t>(1) << OutputBits) - 1;

        public:
            LCG(state_t s = -1) {
                seed(s);
            }

            template <typename T = state_t>
            T next()
            {
                size_t remaining = sizeof(T);
                size_t output_size = 0;
                T value;

                do {
                    // Truncate type if possible to produce smaller code.
                    typename min_type<state_t, T>::type rnd = update();
                    size_t copy_sz = std::min(sizeof(rnd), remaining);

                    // Help the compiler by providing him with a fixed copy size.
                    if (copy_sz != sizeof(rnd))
                        copy_sz = sizeof(T) % sizeof(rnd);

                    Memory::copy(
                        reinterpret_cast<char *>(&value) + output_size,
                        &rnd,
                        copy_sz
                    );

                    remaining -= copy_sz;
                    output_size += copy_sz;
                } while ( remaining > 0 );

                return value; 
            }

            METHOD void seed(state_t s)
            {
                state = s;
            }

        private:
            state_t state;

            static constexpr state_t transform(state_t prev)
            {
                return (prev * a + c);
            } 

            METHOD state_t update() {
                state = transform(state);
                return (state >> output_shift) & output_mask;
            }
    };

    //
    // Standard linear congruential RNG for C99 / C11.
    //
    using Random = LCG<uint32_t, 1103515245, 12345, 15, 16>;

    //
    // 48-bits congruential generator (POSIX rand48).
    //
    using Random48 = LCG<uint64_t, 25214903917, 11, 48, 16>;
}

#endif
