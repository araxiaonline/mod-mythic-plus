#ifndef MYTHICPLUS_H
#define MYTHICPLUS_H

#include "MapMgr.h"
#include "Log.h"

class MythicPlus {
private:
    MythicPlus();
        ~MythicPlus();
public:

    bool Enabled = true;

    bool IsMapEligible(Map* map);

    static MythicPlus * getInstance() {
        static MythicPlus instance;
        return &instance;
    }

};

#endif
