#pragma once

class MonitorRetry {
public:
    bool take()
    {
        if (remaining_ == 0) {
            return false;
        }
        --remaining_;
        return true;
    }

    void reset() { remaining_ = maxAttempts; }

private:
    static constexpr unsigned maxAttempts = 15;
    unsigned remaining_ = maxAttempts;
};
