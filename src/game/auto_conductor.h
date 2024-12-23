#ifndef AUTO_CONDUCTOR_H
#define AUTO_CONDUCTOR_H

#include <QThread>
#include <QMutex>

constexpr int SPEED = 800;

class auto_conductor : public QThread {
    Q_OBJECT

public:
    auto_conductor();

    void setFlag(bool flag);
    bool getFlag();

private:
    bool flag;
    QMutex mutex;

protected:
    void run() override;

signals:
    void permission();
};

#endif // AUTO_CONDUCTOR_H
