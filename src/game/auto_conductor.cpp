#include "auto_conductor.h"

auto_conductor::auto_conductor() {
    //创建该线程但不让其开始
    flag = false;
}

void auto_conductor::setFlag(bool flag) {
    this->flag = flag;
}

bool auto_conductor::getFlag() {
    return flag;
}

void auto_conductor::run() {
    while (true) {
        if (flag) {
            //发出允许动作信号
            emit permission();
        }
        msleep(SPEED);
    }
}
