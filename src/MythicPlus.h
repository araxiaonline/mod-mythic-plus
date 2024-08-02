#ifndef MYTHICPLUS_H
#define MYTHICPLUS_H

class MythicPlus {
private:
    MythicPlus();
    ~MythicPlus();
public:

    static MythicPlus * getInstance() {
        static MythicPlus instance;
        return &instance;
    }

};

#define sMythicPlus MythicPlus::getInstance()

#endif
