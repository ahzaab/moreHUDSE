#ifndef MHUD_VERSION_INCLUDED
#define MHUD_VERSION_INCLUDED

#define MAKE_STR_HELPER(a_str) #a_str
#define MAKE_STR(a_str)        MAKE_STR_HELPER(a_str)

#define MHUD_VERSION_MAJOR 0
#define MHUD_VERSION_MINOR 4
#define MHUD_VERSION_PATCH 0
#define MHUD_VERSION_BETA  0
#define MHUD_VERSION_VERSTRING   \
    MAKE_STR(MHUD_VERSION_MAJOR) \
    "." MAKE_STR(MHUD_VERSION_MINOR) "." MAKE_STR(MHUD_VERSION_PATCH) "." MAKE_STR(MHUD_VERSION_BETA)

#endif

// This is terrible, but I have to do it this way for backwords compatibility
// There are mods that rely on this format MMmmPPBB
#define MHUD_VERSION_NUMBER (static_cast<uint32_t>((MHUD_VERSION_MAJOR * pow(10, 6)) + \
                                                   (MHUD_VERSION_MINOR * pow(10, 4)) + \
                                                   (MHUD_VERSION_PATCH * pow(10, 2)) + \
                                                   (MHUD_VERSION_BETA * pow(10, 0))))
