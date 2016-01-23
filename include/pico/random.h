#ifndef PICOLIB_RANDOM_H_
#define PICOLIB_RANDOM_H_

#include <cstdint>

namespace Pico {

    //
    // Target-specific implementation.
    //
    class SecureRandom
    {
        public:
            CONSTRUCTOR SecureRandom();
            DESTRUCTOR ~SecureRandom();

            template <typename T>
            size_t read(T& value);

        private:
            Target::random_pool pool;
    };

    //
    // Generic linear congruential random number generator.
    //
    template <typename StateType, uint64_t A, uint64_t C, unsigned OutputBits, unsigned OutputShift>
    class LCG
    {
        static_assert(OutputBits > 0, "Number of output bits must be positive.");
        static_assert(OutputShift < sizeof(StateType) * 8, "Output shift too big.");
        static_assert(OutputBits <= sizeof(StateType) * 8 - OutputShift, "RNG state is too small for output parameters.");

        using state_t = StateType;
        static constexpr auto a = A;
        static constexpr auto c = C;
        static constexpr size_t output_size = OutputBits;
        static constexpr auto output_shift = OutputShift;
        static constexpr state_t output_mask = (static_cast<state_t>(1) << output_size) - 1;

        public:
            CONSTRUCTOR LCG(state_t s = -1) {
                seed(s);
            }

            METHOD state_t next()
            {
                state = transform(state);
                return (state >> output_shift) & output_mask;
            }

            METHOD void seed(state_t s)
            {
                state = s;
            }

            template <typename T>
            void fill(T& value)
            {
                static_assert(output_size / 8 > 0, "Output size too small for fill.");

                for (size_t i = 0; i < sizeof(value); i++) {
                    uint8_t c = next();

                    auto ptr = reinterpret_cast<uint8_t *>(&value) + i;

                    *ptr = c;
                }
            }

        private:
            state_t state;

            static constexpr state_t transform(state_t prev)
            {
                return (prev * a + c);
            } 
    };

    //
    // Standard linear congruential RNG for C99 / C11.
    //
    using Random = LCG<uint32_t, 1103515245, 12345, 15, 16>;

    //
    // 48-bits congruential generator (POSIX rand48).
    //
    using Random48 = LCG<uint64_t, 25214903917, 11, 32, 16>;
}

#endif
