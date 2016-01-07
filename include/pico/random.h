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
    template <typename StateType, long A, long C, unsigned OutputBits>
    class LCG
    {
        using state_t = StateType;

        static constexpr auto a = A;
        static constexpr auto c = C;
        static constexpr state_t output_mask = (static_cast<state_t>(1) << OutputBits) - 1;

        public:
            LCG(state_t s = 1) {
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
                state = s & output_mask;
            }

        private:
            state_t state;

            static constexpr state_t transform(state_t prev)
            {
                return (prev * a + c) & output_mask;
            } 

            METHOD state_t update() {
                state = transform(state);
                return state;
            }
    };

    //
    // Linear congruential RNG based on old glibc parameters.
    //
    using Random = LCG<uint32_t, 1103515245, 12345, 31>;
}

#endif
