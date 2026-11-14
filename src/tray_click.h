#pragma once

class TrayClick {
public:
    void defer() { pending_ = true; }
    void cancel() { pending_ = false; }

    bool take()
    {
        const bool pending = pending_;
        cancel();
        return pending;
    }

private:
    bool pending_ = false;
};
