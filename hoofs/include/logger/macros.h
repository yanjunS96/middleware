#ifndef __H_MVP_MACROS__H__
#define __H_MVP_MACROS__H__
#include <mutex>
// There must be many copy-paste versions of these macros which are same
// things, undefine them to avoid conflict.
#undef UNUSED
#undef DISALLOW_COPY_AND_ASSIGN
#define UNUSED(param) (void)param
#define DISALLOW_COPY_AND_ASSIGN(classname)                                    \
    classname(const classname &) = delete;                                     \
    classname &operator=(const classname &) = delete;
#define DECLARE_SINGLETON(classname)                                           \
  public:                                                                      \
    static classname *Instance(bool create_if_needed = true)                   \
    {                                                                          \
        static classname *instance = nullptr;                                  \
        if (!instance && create_if_needed)                                     \
        {                                                                      \
            static std::once_flag flag;                                        \
            std::call_once(                                                    \
                flag, [&] { instance = new (std::nothrow) classname(); });     \
        }                                                                      \
        return instance;                                                       \
    }                                                                          \
                                                                               \
  private:                                                                     \
    classname();                                                               \
    DISALLOW_COPY_AND_ASSIGN(classname)
#endif //__H_MVP_MACROS__H__
