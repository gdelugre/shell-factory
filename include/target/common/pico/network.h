#ifndef COMMON_PICO_NETWORK_H_
#define COMMON_PICO_NETWORK_H_

namespace Pico {

    namespace Network {

        template <AddressType>
        struct Address;

        template <>
        struct Address<AddressType::IPV4>
        {
            union {
                uint8_t bytes[4];
                uint32_t value;
            };
        };

        template <>
        struct Address<AddressType::IPV6>
        {
            uint8_t bytes[16];
        };

        // IP addresses structure types.
        using IpAddress     = Address<AddressType::IPV4>;
        using IpAddress6    = Address<AddressType::IPV6>;

        template <size_t N>
        struct ip_address_type;

        template <>
        struct ip_address_type<4>
        {
            using type = IpAddress;
        };

        template <>
        struct ip_address_type<16>
        {
            using type = IpAddress6;
        };

        // Generates an IP address structure out of a list of static arguments.
        template <typename... V> 
        FUNCTION
        constexpr auto ip_address_from_bytes(V... bytes)
        {
            static_assert(sizeof...(bytes) == 4 || sizeof...(bytes) == 16, "Invalid number of bytes for IP address.");

            return typename ip_address_type<sizeof...(bytes)>::type { { static_cast<uint8_t>(bytes)... } };
        }
    }
}

#endif
