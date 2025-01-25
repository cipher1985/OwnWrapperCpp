#ifndef QTDELAYSTATECHECKER_H
#define QTDELAYSTATECHECKER_H

#include <QObject>
#include <QElapsedTimer>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
//延时转换状态检测器
class QtDelayStateChecker : public QObject
{
    Q_OBJECT
public:
    QtDelayStateChecker();
    //设置检测方法
    void SetCheckFunc(std::function<int(int)> funcCheck,
        std::function<void(int)> funcStateChanged);
    //更新数据
    void UpdateData(int value);
    //设置检测延时时长
    void SetDelayTime(int64_t ms);
    //强制设置当前状态
    void SetState(int state);
    //获得当前状态
    int GetState();
signals:
    void sigCheck(int );
    void sigStateChanged(int state);
private:
    //更新实时数值
    QAtomicInt m_value{0};
    //状态转换
    QAtomicInt m_state{0};
    //延时时长记录
    QElapsedTimer m_timer;
    //上次更新时间
    std::chrono::steady_clock::time_point m_lastUpdateTime;
    //更新测算时长
    std::atomic<int64_t> m_delayMS{0};
    //根据数据改变状态规则
    std::function<int(int)> m_funcStateCheck{};
    //状态改变回调方法
    std::function<void(int)> m_funcStateChanged{};
};

#endif // QTDELAYSTATECHECKER_H
