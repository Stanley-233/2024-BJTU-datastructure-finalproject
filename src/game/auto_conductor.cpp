#include "auto_conductor.h"

auto_conductor::auto_conductor() {
    //创建该线程但不让其开始
    flag = false;
}

void auto_conductor::setFlag(bool flag) {
    QMutexLocker locker(&mutex);
    this->flag = flag;
}

bool auto_conductor::getFlag() {
    QMutexLocker locker(&mutex);
    return flag;
}

void auto_conductor::run() {
    while (true) {
        if (getFlag()) {
            //发出允许动作信号
            emit permission();
        }
        msleep(SPEED);
    }
}
