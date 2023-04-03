#ifndef ANNOUNCEMENT_TIMER_H
#define ANNOUNCEMENT_TIMER_H

#include <thread>
#include <functional>
#include <condition_variable>

namespace motovis
{
    namespace doip
    {
        /// @brief A timer to announce the vehicle for a specific timer with a specific delay in between
        class AnnouncementTimer
        {
        private:
            const std::chrono::microseconds cSpinWait{1};

            std::mutex mMutex;
            std::unique_lock<std::mutex> mLock;
            std::condition_variable mConditionVariable;
            std::chrono::seconds mDelay;
            std::chrono::seconds mInterval;
            uint8_t mCount;
            std::thread mThread;
            std::function<void()> mCallback;

            void announce()
            {
                mLock.lock();

                std::cv_status _status{mConditionVariable.wait_for(mLock, mDelay)};

                // Start the annoucement after the delay if it is NOT interrupted
                if (_status == std::cv_status::timeout)
                {
                    for (uint8_t i = 0; i < mCount; ++i)
                    {
                        _status = mConditionVariable.wait_for(mLock, mInterval);

                        // Invoke the callback if the timer is NOT interrupted, otherwise
                        // break the annoucement iteration loop and release the lock
                        if (_status == std::cv_status::timeout)
                        {
                            mCallback();
                        }
                        else
                        {
                            break;
                        }
                    }
                }

                mLock.unlock();
            }

            void dispose()
            {
                if (mThread.joinable())
                {
                    mConditionVariable.notify_one();
                    mThread.join();
                }
            }

        public:
            /// @brief Constructor
            /// @param delay Delay in seconds before starting the announcement
            /// @param interval Interval in seconds between each announcement
            /// @param count Number of annoucement broadcasting iterations
            AnnouncementTimer(
                    std::chrono::seconds delay,
                    std::chrono::seconds interval,
                    uint8_t count): mCount{count > 0 ? count : throw std::out_of_range("Announcement count should be greater than zero.")},
                                    mLock(mMutex, std::defer_lock),
                                    mDelay{delay},
                                    mInterval{interval}
            {
            }
            AnnouncementTimer(const AnnouncementTimer &) = delete;
            AnnouncementTimer &operator=(const AnnouncementTimer &) = delete;
            ~AnnouncementTimer()
            {
                dispose();
            }

            /// @brief (Re)start the timer to tick
            /// @param callback Delegate to be invoked on each timer tick
            /// @note The caller will be blocked until the timer has been started.
            void Start(std::function<void()> &&callback)
            {
                if (mLock.owns_lock())
                {
                    dispose();
                }

                mCallback = std::move(callback);
                mThread = std::thread(&AnnouncementTimer::announce, this);

                // Spinning till the timer become activated.
                while (!mLock.owns_lock())
                {
                    std::this_thread::sleep_for(cSpinWait);
                }
            }
        };
    }
}

#endif