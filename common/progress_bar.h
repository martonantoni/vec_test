#pragma once

class cProgressBar final
{
    uint64_t mProgressRange;
    uint64_t mProgress = 0;
    bool mDone = false;
    static constexpr int progressSteps = 20;
public:
    cProgressBar(uint64_t fullRange)
    {
        mProgressRange = fullRange / progressSteps;
        std::cout << "\n["
            << std::string(progressSteps, '.')
            << "]\r[";
        std::cout.flush();
    }
    ~cProgressBar()
    {
        if (!mDone)
        {
            done();
        }
    }
    void step()
    {
        if (++mProgress == mProgressRange)
        {
            std::cout << "#";
            std::cout.flush();
            mProgress = 0;
        }
    }
    void done()
    {
        std::cout << "\r" << std::string(progressSteps + 2, ' ') << "\r";
        std::cout.flush();
    }
};