#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>

namespace
{
    class Log
    {
    public:
        Log()
        {
#ifndef NDEBUG
            auto t = std::time(nullptr);
            tm tm = { 0 };
            localtime_r(&t, &tm);
            m_stream << "[" << std::setfill(' ') << std::setw(8) << std::this_thread::get_id() << "][" << std::put_time(&tm, "%F %T") << "]: ";
#endif
        }

        ~Log()
        {
#ifndef NDEBUG
            std::string buffer = m_stream.str();
            std::cerr << buffer << std::endl;
#endif
        }

#ifndef NDEBUG
        std::ostringstream& stream() { return m_stream; }
#else
        template<typename T>
        Log& operator << (T) { return *this; }

        Log& stream() { return *this; }
#endif

    private:
#ifndef NDEBUG
        std::ostringstream m_stream;
#endif
    };
}

#define LOG() Log().stream()
