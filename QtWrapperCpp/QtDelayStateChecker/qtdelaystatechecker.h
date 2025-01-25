#ifndef QTDELAYSTATECHECKER_H
#define QTDELAYSTATECHECKER_H

#include <QObject>
#include <QElapsedTimer>

//延时转换状态检测器
class QtDelayStateChecker : public QObject
{
    Q_OBJECT
public:
    QtDelayStateChecker(QObject* parent = nullptr);
    //设置检测方式
    void SetCheckFunc(
        std::function<int(int)> funcPreprocessCheck);
    //更新数据状态值等待延时时长结束则改变当前状态
    void UpdateData(int value);
    //设置检测延时时长
    void SetDelayTime(int64_t ms);
    //强制设置当前状态
    void SetState(int state);
    //获得当前状态
    int GetState();
signals:
    //状态改变通知
    void sigStateChanged(int state);
private:
    //更新实时数值
    QAtomicInt m_value{0};
    //状态转换
    QAtomicInt m_state{0};
    //延时时长记录
    QElapsedTimer m_timer;
    //更新测算时长
    std::atomic<int64_t> m_delayMS{0};
    //根据数据改变状态规则
    std::function<int(int)> m_funcPreprocessCheck{};
};

#endif // QTDELAYSTATECHECKER_H
