#include "cxx_stl/string.hpp"

using namespace middleware::hoofs::cxx_stl;

template<typename T1, typename T2>
typename std::enable_if<(internal::IsCharArray<T1>::value || internal::IsCxxString<T1>::value)
                        && (internal::IsCharArray<T2>::value || internal::IsCxxString<T2>::value),
        string<internal::GetCapa<T1>::capa + internal::GetCapa<T2>::capa>>::type
middleware::hoofs::cxx_stl::concatenate(const T1& t1, const T2& t2) noexcept
{
    uint64_t size1 = internal::GetSize<T1>::call(t1);
    uint64_t size2 = internal::GetSize<T2>::call(t2);
    string<internal::GetCapa<T1>::capa + internal::GetCapa<T2>::capa> newString;
    std::memcpy(&(newString.m_rawstring[0]), internal::GetData<T1>::call(t1), size1);
    std::memcpy(&(newString.m_rawstring[0]) + size1, internal::GetData<T2>::call(t2), size2);
    newString.m_rawstring[size1 + size2] = '\0';
    newString.m_rawstringSize = size1 + size2;

    return newString;
}
template<typename T1, typename T2, typename... Args>
typename std::enable_if<(internal::IsCharArray<T1>::value || internal::IsCxxString<T1>::value)
                        && (internal::IsCharArray<T2>::value || internal::IsCxxString<T2>::value),
        string<internal::SumCapa<T1,T2,Args...>::value>>::type
middleware::hoofs::cxx_stl::concatenate(const T1& t1, const T2& t2, const Args&... args) noexcept
{
    return concatenate(concatenate(t1,t2),args...);
}

template<uint64_t Capacity>
inline string<Capacity>::string(const string &other) noexcept
{
    copy(other);
}

template<uint64_t Capacity>
inline string<Capacity>::string(string &&other) noexcept
{
    move(std::move(other));
}

template<uint64_t Capacity>
inline string<Capacity>& string<Capacity>::operator=(const string &rhs) noexcept
{
    if (this == &rhs){return *this;}
    return copy(rhs);
}

template<uint64_t Capacity>
inline string<Capacity>& string<Capacity>::operator=(string &&rhs) noexcept
{
    if (this == &rhs){return *this;}
    return move(std::move(rhs));
}

template<uint64_t Capacity>
template<uint64_t N>
inline string<Capacity>::string(const string<N> &other) noexcept
{
    static_assert(N<Capacity,"Construction failed. The capacity of the given fixed string is larger than the capacity of this.");
    copy(other);
}

template<uint64_t Capacity>
template<uint64_t N>
inline string<Capacity>::string(string<N> &&other) noexcept
{
    static_assert(N<Capacity,"Construction failed. The capacity of the given fixed string is larger than the capacity of this.");
    move(std::move(other));
}

template<uint64_t Capacity>
template<uint64_t N>
inline string<Capacity>& string<Capacity>::operator=(const string<N> &rhs) noexcept
{
    static_assert(N<Capacity,"Construction failed. The capacity of the given fixed string is larger than the capacity of this.");
    return copy(rhs);
}

template<uint64_t Capacity>
template<uint64_t N>
inline string<Capacity>& string<Capacity>::operator=( string<N> &&rhs) noexcept
{
    static_assert(N<Capacity,"Construction failed. The capacity of the given fixed string is larger than the capacity of this.");
    return move(std::move(rhs));
}

template<uint64_t Capacity>
template<uint64_t N>
inline string<Capacity>::string(const char (&other)[N]) noexcept
{
    *this = other;
}

template<uint64_t Capacity>
inline string<Capacity>::string(const char * const other)noexcept
:string(other,[&]()->uint64_t {
    return other ? strnlen(other, Capacity) : 0U;
}())
{}
template<uint64_t Capacity>
inline string<Capacity>::string(const std::string &other) noexcept
:string(other.data(), other.size())
{}
template<uint64_t Capacity>
inline string<Capacity>::string(const char *const other, const uint64_t count) noexcept
{
    if(nullptr == other)
    {
        m_rawstring[0U] = '\0';
        m_rawstringSize = 0U;
    }
    else if(Capacity < count)
    {
        std::memcpy(&(m_rawstring[0]),other,Capacity);
        m_rawstring[Capacity] = '\0';
        m_rawstringSize = Capacity;
        MLOG_ERROR("Constructor truncates the last {} characters of {}, "
                  "because the char array length is larger than the capacity"
                  , count - Capacity, other);
    }
    else
    {
        std::memcpy(&(m_rawstring[0]),other,count);
        m_rawstring[count] = '\0';
        m_rawstringSize = count;
    }
}

template<uint64_t Capacity>
template<uint64_t N>
inline string<Capacity>& string<Capacity>::operator=(const char (&rhs)[N]) noexcept
{
    static_assert(N<=Capacity+1U,
            "Assignment failed. The given char array is larger than the capacity of the fixed string.");
    if(rhs == this->c_str()){return *this;}
    std::memcpy(&(m_rawstring[0]), rhs, N);
    m_rawstringSize = std::min(Capacity, static_cast<uint64_t>(strnlen(rhs, N)));
    m_rawstring[m_rawstringSize] = '\0';
    if('\0' != rhs[m_rawstringSize])
    {
        MLOG_ERROR("Assignment of array which is not zero-terminated! Last value of array overwritten with 0!");
    }
    return *this;
}

template<uint64_t Capacity>
template<uint64_t N>
inline string<Capacity>& string<Capacity>::assign(const string<N>& str) noexcept
{
    static_assert(N<=Capacity,"Assignment failed. The capacity of the given fixed string is larger than the capacity of this");
    *this = str;
    return *this;
}

template<uint64_t Capacity>
template<uint64_t N>
inline string<Capacity>& string<Capacity>::assign(const char (&rhs)[N]) noexcept {
    *this = rhs;
    return *this;
}

template<uint64_t Capacity>
inline bool string<Capacity>::unsafe_assign(const char* const str) noexcept
{
    if ((str == this->c_str()) || (nullptr == str)){return false;}
    const uint64_t strSize = strnlen(str, Capacity + 1U);
    if (Capacity < strSize)
    {
        MLOG_ERROR("Assignment failed. The given cstring is larger ({}) than "
                   "the capacity ({}) of the fixed string.", strSize, Capacity);
        return false;
    }
    std::memcpy(&(m_rawstring[0]), str, strSize);
    m_rawstring[strSize] = '\0';
    m_rawstringSize = strSize;
    return true;
}
template<uint64_t Capacity>
inline bool string<Capacity>::unsafe_assign(const std::string &str) noexcept
{
    uint64_t strSize = str.size();
    if (Capacity < strSize)
    {
        MLOG_ERROR("Assignment failed. The given std::string is larger ({}) than "
                   "the capacity ({}) of the fixed string.", strSize, Capacity);
        return false;
    }
    std::memcpy(&(m_rawstring[0]), str.c_str(), strSize);
    m_rawstring[strSize] = '\0';
    m_rawstringSize = strSize;
    return true;
}

template<uint64_t Capacity>
template<uint64_t N>
inline int64_t string<Capacity>::compare(const string<N> &other) const noexcept
{
    uint64_t otherSize = other.size();
    if(m_rawstringSize < otherSize){return -1;}
    else if(m_rawstringSize > otherSize){return 1;}
    return memcmp(this->c_str(), other.c_str(), m_rawstringSize);
}

template<uint64_t Capacity>
template<uint64_t N>
inline bool string<Capacity>::operator==(const string<N> &rhs) const noexcept
{
    return (compare(rhs) == 0);
}
template<uint64_t Capacity>
template<uint64_t N>
inline bool string<Capacity>::operator!=(const string<N> &rhs) const noexcept
{
    return (compare(rhs) != 0);
}
template<uint64_t Capacity>
template<uint64_t N>
inline bool string<Capacity>::operator<(const string<N> &rhs) const noexcept
{
    return (compare(rhs) < 0);
}
template<uint64_t Capacity>
template<uint64_t N>
inline bool string<Capacity>::operator<=(const string<N> &rhs) const noexcept
{
    return !(compare(rhs) > 0);
}
template<uint64_t Capacity>
template<uint64_t N>
inline bool string<Capacity>::operator>(const string<N> &rhs) const noexcept
{
    return (compare(rhs) > 0);
}
template<uint64_t Capacity>
template<uint64_t N>
inline bool string<Capacity>::operator>=(const string<N> &rhs) const noexcept
{
    return !(compare(rhs) < 0);
}

template<uint64_t Capacity>
inline bool string<Capacity>::operator==(const char *const rhs) const noexcept
{
    uint64_t strSize = strlen(rhs);
    if(m_rawstringSize != strSize){return false;}
    return memcmp(this->c_str(), rhs, m_rawstringSize);
}
template<uint64_t Capacity>
inline bool string<Capacity>::operator!=(const char *const rhs) const noexcept
{
    return !(this->operator==(rhs));
}

template<uint64_t Capacipy>
inline const char* string<Capacipy>::c_str() const noexcept
{
    return m_rawstring;
}
template<uint64_t Capacipy>
inline constexpr uint64_t string<Capacipy>::size() const noexcept
{
    return m_rawstringSize;
}

template<uint64_t Capacipy>
inline constexpr uint64_t string<Capacipy>::capacity() noexcept {
    return Capacipy;
}

template<uint64_t Capacipy>
inline constexpr bool string<Capacipy>::empty() const noexcept {
    return (m_rawstringSize == 0U);
}

template<uint64_t Capacipy>
inline string<Capacipy>::operator std::string() const noexcept
{
    return std::string(this->c_str());
}

/*
template<uint64_t Capacipy>
template<typename T>
inline string<Capacipy>& string<Capacipy>::operator+=(const T & value) noexcept
{
    uint64_t valueSize = sizeof(value);
    if (Capacipy - size() > valueSize)
    {
        memcpy(&(m_rawstring[m_rawstringSize]), static_cast<const char *>(&value), valueSize);
        m_rawstringSize += valueSize;
        m_rawstring[m_rawstringSize] = '\0';
        return *this;
    }
    else
    {
        string<valueSize + Capacipy> str;
        memcpy(str.c_str(), this->c_str(), this->size());
        memcpy(str.c_str() + this->size(), static_cast<const char *>(&value), valueSize);
        str.m_rawstringSize = this->size() + valueSize;
        str.m_rawstring[str.m_rawstringSize] = '\0';
        return str;
    }
}
*/
template<uint64_t Capacity>
template<typename T>
inline typename std::enable_if<internal::IsCharArray<T>::value || internal::IsCxxString<T>::value, middleware::hoofs::cxx_stl::string<Capacity> &>::type
string<Capacity>::append(const T &t) noexcept
{
    uint64_t tSize = internal::GetSize<T>::call(t);
    const char * tData = internal::GetData<T>::call(t);
    uint64_t clampendTsize = std::min(Capacity - m_rawstringSize, tSize);
    std::memcpy(&(m_rawstring[m_rawstringSize]), tData, clampendTsize);
    if(tSize> clampendTsize)
    {
        MLOG_ERROR("The last {} characters of {} are truncated, "
                   "because the length is larger than the capacity."
                   , tSize- Capacity+m_rawstringSize, tData);
    }
    m_rawstringSize += clampendTsize;
    m_rawstring[m_rawstringSize] = '\0';
    return *this;
}
template<uint64_t Capacipy>
template<typename T>
inline typename std::enable_if<internal::IsCharArray<T>::value || internal::IsCxxString<T>::value, bool>::type
string<Capacipy>::unsafe_append(const T &t) noexcept
{
    uint64_t tSize = internal::GetSize<T>::call(t);
    const char * tData = internal::GetData<T>::call(t);
    uint64_t clampendTsize = std::min(Capacipy - m_rawstringSize, tSize);
    if(tSize> clampendTsize)
    {
        MLOG_ERROR("Appending failed because the sum of sizes exceeds this' capacity.");
        return false;
    }
    std::memcpy(&(m_rawstring[m_rawstringSize]), tData, clampendTsize);
    m_rawstringSize += clampendTsize;
    m_rawstring[m_rawstringSize] = '\0';
    return *this;
}

template<uint64_t Capacity>
string<Capacity> string<Capacity>::substr(const uint64_t pos, const uint64_t count) const noexcept
{
    static_assert(pos<=m_rawstringSize,"substr failed. because pos is too bigger!");
    uint64_t length = std::min(count, m_rawstringSize - pos);
    string subString;
    std::memcpy(&(subString.m_rawstring[0]), &m_rawstring[pos], length);
    subString.m_rawstring[length] = '\0';
    subString.m_rawstringSize = length;
    return subString;
}
template<uint64_t Capacity>
string<Capacity> string<Capacity>::substr(const uint64_t pos) const noexcept
{
    return substr(pos,m_rawstringSize);
}

template<uint64_t Capacity>
template<typename T>
typename std::enable_if<std::is_same<T, std::string>::value || internal::IsCharArray<T>::value || internal::IsCxxString<T>::value, uint64_t>
        string<Capacity>::find(const T &t, const uint64_t pos) const noexcept
{
    static_assert(pos<=m_rawstringSize,"substr failed. because pos is too bigger!");
    const char * found = std::strstr(c_str()+pos, internal::GetData<T>::call(t));
    if(found == nullptr){return size() + 1;}
    return (found - c_str());
}
template<uint64_t Capacity>
template<typename T>
typename std::enable_if<std::is_same<T, std::string>::value || internal::IsCharArray<T>::value || internal::IsCxxString<T>::value, uint64_t>
string<Capacity>::find_first_of(const T &t, const uint64_t pos) const noexcept
{
    static_assert(pos<=m_rawstringSize,"substr failed. because pos is too bigger!");
    const char * found = nullptr;
    const char* data = internal::GetData<T>::call(t);
    for(auto p = pos; p < m_rawstringSize; ++p){
        found = std::strchr(data, m_rawstring[p]);
        if(nullptr != found){return p;}
    }
    return size() + 1;
}
template<uint64_t Capacity>
template<typename T>
typename std::enable_if<std::is_same<T, std::string>::value || internal::IsCharArray<T>::value || internal::IsCxxString<T>::value, uint64_t>
string<Capacity>::find_last_of(const T &t, const uint64_t pos) const noexcept
{
    static_assert(pos<=m_rawstringSize,"substr failed. because pos is too bigger!");
    auto p = pos;
    if(m_rawstringSize - 1U < p){p = m_rawstringSize - 1U;}
    const char * found = nullptr;
    const char* data = internal::GetData<T>::call(t);
    for(; p > 0U; --p){
        found = std::strchr(data, m_rawstring[p]);
        if(nullptr != found){return p;}
    }
    found = std::strchr(data,m_rawstring[p]);
    if(nullptr != found){return 0U;}
    return size() + 1;
}

template<uint64_t Capacity>
template<uint64_t N>
inline string<Capacity>& string<Capacity>::copy(const string<N> &rhs) noexcept
{
    static_assert(N<Capacity,
            "Assignment failed. The capacity of the given fixed string is larger than the capacity of this.");
    uint64_t strSize = rhs.size();
    std::memcpy(&(m_rawstring[0]), rhs.c_str(), strSize);
    m_rawstring[strSize] = '\0';
    m_rawstringSize = strSize;
    return *this;
}

template<uint64_t Capacity>
template<uint64_t N>
inline string<Capacity>& string<Capacity>::move(string<N> &&rhs) noexcept
{
    static_assert(N<Capacity,
                  "Assignment failed. The capacity of the given fixed string is larger than the capacity of this.");
    uint64_t strSize = rhs.size();
    std::memcpy(&(m_rawstring[0]), rhs.c_str(), strSize);
    m_rawstring[strSize] = '\0';
    m_rawstringSize = strSize;
    rhs.m_rawstring[0U] = '\0';
    rhs.m_rawstringSize = 0U;
    return *this;
}
