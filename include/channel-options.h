#ifndef CHANNEL_OPTIONS_H_
#define CHANNEL_OPTIONS_H_

#include <pico.h>

namespace Options {

    #if !defined(CHANNEL)
    #define OPT_CHANNEL USE_STDOUT
    #else
    #define OPT_CHANNEL CHANNEL
    #endif

    /*
     * HOST parameter.
     * Used for socket channels.
     */
    #if !defined(HOST)
    #define OPT_HOST    0,0,0,0
    #else
    #define OPT_HOST    HOST
    #endif

    #if !defined(LHOST)
    #define OPT_LHOST HOST
    #else
    #define OPT_LHOST LHOST
    #endif
   
    #if !defined(RHOST)
    #define OPT_RHOST HOST
    #else
    #define OPT_RHOST RHOST
    #endif

    /*
     * PORT parameter.
     * Used for socket channels.
     */
    #if !defined(PORT)
    #define OPT_PORT    0
    #else
    #define OPT_PORT    PORT
    #endif

    #if !defined(LPORT)
    #define OPT_LPORT PORT
    #else
    #define OPT_LPORT LPORT
    #endif
   
    #if !defined(RPORT)
    #define OPT_RPORT PORT
    #else
    #define OPT_RPORT RPORT
    #endif

    #if !defined(HANDLE)
    #define OPT_HANDLE  -1
    #else
    #define OPT_HANDLE  HANDLE
    #endif

    constexpr uint16_t local_port = OPT_LPORT;
    constexpr uint16_t remote_port = OPT_RPORT;
    constexpr auto local_address = Pico::Network::ip_address_from_bytes(OPT_LHOST);
    constexpr auto remote_address = Pico::Network::ip_address_from_bytes(OPT_RHOST);
    constexpr Target::handle handle = OPT_HANDLE;
}

#endif
