#include "QtDelayStateChecker.h"


QtDelayStateChecker::QtDelayStateChecker() :
    m_lastUpdateTime(std::chrono::steady_clock::now()) {}

void QtDelayStateChecker::SetCheckFunc(
    std::function<int(int)> funcCheck,
    std::function<void(int)> funcStateChanged)
{
    m_funcStateCheck = funcCheck;
    m_funcStateChanged = funcStateChanged;
    m_lastUpdateTime = std::chrono::steady_clock::now();
}

void QtDelayStateChecker::UpdateData(int value)
{
    if(!m_funcStateCheck)
        return;
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
        (currentTime - m_lastUpdateTime).count();
    int curState = m_funcStateCheck(value);
    //检测状态是否一致
    if(curState != m_state) {
        if(duration > m_delayMS) {
            //超过时长更新状态
            m_state = curState;
            if(m_funcStateChanged)
                m_funcStateChanged(m_state);
            m_lastUpdateTime = std::chrono::steady_clock::now();
        }
    } else {
        //状态一致更新时间
        m_lastUpdateTime = std::chrono::steady_clock::now();
    }
}

void QtDelayStateChecker::SetDelayTime(int64_t ms)
{
    m_delayMS = ms;
    m_lastUpdateTime = std::chrono::steady_clock::now();
}

void QtDelayStateChecker::SetState(int state)
{
    m_state = state;
    m_lastUpdateTime = std::chrono::steady_clock::now();
}
