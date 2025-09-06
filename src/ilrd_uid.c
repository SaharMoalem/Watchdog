 #include <ifaddrs.h>      /* getifaddrs */
#include <string.h>       /* memcpy, memcmp */
#include <unistd.h>       /* getpid */
#include <stdatomic.h>      /* atomic */

#include "ilrd_uid.h"

const ilrd_uid_t bad_uid = {0};

ilrd_uid_t UIDCreate(void)
{
    ilrd_uid_t uid;
    struct ifaddrs *ifaddr;
    static atomic_uint counter = 0;
    
    if(getifaddrs(&ifaddr) == -1)
    {
        return bad_uid;
    }

    memcpy((char*)uid.ip, ifaddr->ifa_addr->sa_data, 14);
    uid.time = time(NULL);
    uid.pid = getpid();
    uid.counter = atomic_fetch_add(&counter, 1);
    freeifaddrs(ifaddr);
    
    return uid;
}

int UIDIsSame(ilrd_uid_t uid1, ilrd_uid_t uid2)
{
    return (uid1.time == uid2.time && uid1.counter == uid2.counter &&
                uid1.pid == uid2.pid &&
                memcmp((char*)uid1.ip, (char*)uid2.ip, 14) == 0);
}
