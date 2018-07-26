#if defined(PLATFORM_MAC)

#include "Standard.h"
#include "Resources.h"

int macDataLen(const char* oldStr) {
    char str[40];
    char name1[17];
    char name2[17];
    sprintf(str, "%s%s", "J", oldStr);
    for (int n = strlen(str); n < 32; n++) {
        str[n] = '_';
    } str[32] = '\0';
    strncpy(name1, str, 16); name1[16] = '\0';
    strncpy(name2, str + 16, 16); name2[16] = '\0';

    unsigned long size;
    getsectiondata(&_mh_execute_header, name1, name2, &size);
    return (int)size;
}

unsigned char* macDataPtr(const char* oldStr) {
    char str[40];
    char name1[17];
    char name2[17];
    sprintf(str, "%s%s", "J", oldStr);
    for (int n = strlen(str); n < 32; n++) {
        str[n] = '_';
    } str[32] = '\0';
    strncpy(name1, str, 16); name1[16] = '\0';
    strncpy(name2, str + 16, 16); name2[16] = '\0';
    unsigned long size;
    return getsectiondata(&_mh_execute_header, name1, name2, &size);
}

#endif
