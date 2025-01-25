#include "QtDelayStateChecker.h"


QtDelayStateChecker::QtDelayStateChecker(QObject* parent) :
    QObject(parent)
{
    m_timer.restart();
    m_funcPreprocessCheck = [](int value){ return value; };
}

void QtDelayStateChecker::SetCheckFunc(
    std::function<int(int)> funcPreprocessCheck)
{
    if(funcPreprocessCheck == nullptr)
        return;
    m_funcPreprocessCheck = funcPreprocessCheck;
    m_timer.restart();
}

void QtDelayStateChecker::UpdateData(int value)
{
    int curState = m_funcPreprocessCheck(value);
    //检测状态是否一致
    if(curState != m_state) {
        if(m_timer.elapsed() > m_delayMS) {
            //超过时长更新状态
            m_state = curState;
            emit sigStateChanged(m_state);
            m_timer.restart();
        }
    } else {
        //状态一致更新时间
        m_timer.restart();
    }
}

void QtDelayStateChecker::SetDelayTime(int64_t ms)
{
    m_delayMS = ms;
    m_timer.restart();
}

void QtDelayStateChecker::SetState(int state)
{
    m_state = state;
    m_timer.restart();
}

int QtDelayStateChecker::GetState()
{
    return m_state;
}
