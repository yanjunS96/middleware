//
// Created by syj on 2023/3/1.
//

#ifndef MIDDLEWARE_STRING_HPP
#define MIDDLEWARE_STRING_HPP

#include <iostream>
#include <algorithm>
#include <cassert>

#include "string_internal.hpp"
#include "logger/mlog.h"

namespace middleware{
    namespace hoofs{
        namespace cxx_stl{
            template<typename T1, typename T2>
            typename std::enable_if<(internal::IsCharArray<T1>::value || internal::IsCxxString<T1>::value)
                                           && (internal::IsCharArray<T2>::value || internal::IsCxxString<T2>::value),
                    string<internal::GetCapa<T1>::capa + internal::GetCapa<T2>::capa>>::type
            concatenate(const T1& t1, const T2& t2) noexcept;
            template<typename T1, typename T2, typename... Args>
            typename std::enable_if<(internal::IsCharArray<T1>::value || internal::IsCxxString<T1>::value)
                                    && (internal::IsCharArray<T2>::value || internal::IsCxxString<T2>::value),
                    string<internal::SumCapa<T1,T2,Args...>::value>>::type
            concatenate(const T1& t1, const T2& t2, const Args&... args) noexcept;

            template<typename T1,typename T2>
            typename std::enable_if<(internal::IsCharArray<T1>::value && internal::IsCxxString<T2>::value)
                        || (internal::IsCxxString<T1>::value && internal::IsCharArray<T2>::value)
                        || (internal::IsCxxString<T1>::value && internal::IsCxxString<T2>::value)
                        ,string<internal::GetCapa<T1>::capa + internal::GetCapa<T2>::capa>>::type
            operator+(const T1& t1, const T2& t2)noexcept;
            
            template<uint64_t Capacity>
            class string{
                static_assert(Capacity>0U, "The capacity of the fixed string must be greater than 0!");
            public:
                constexpr string() noexcept = default;
                string(const string & other) noexcept;
                string(string && other) noexcept;
                string& operator=(const string & rhs) noexcept;
                string& operator=(string&& rhs)noexcept;

                template<uint64_t N>
                string(const string<N>& other) noexcept;
                template<uint64_t N>
                string(string<N>&& other) noexcept;
                template<uint64_t N>
                string& operator=(const string<N> & rhs) noexcept;
                template<uint64_t N>
                string& operator=(string<N>&& rhs)noexcept;

                template<uint64_t N>
                string(const char (&other)[N])noexcept;

                string(const char* const other)noexcept;
                string(const std::string& other)noexcept;
                string(const char* const other, const uint64_t count)noexcept;

                template<uint64_t N>
                string& operator=(const char (&rhs)[N])noexcept;

                template<uint64_t N>
                string& assign(const string<N>& str)noexcept;
                template<uint64_t N>
                string& assign(const char (&rhs)[N])noexcept;

                bool unsafe_assign(const char* const str)noexcept;
                bool unsafe_assign(const std::string& str)noexcept;

                template<uint64_t N>
                int64_t compare(const string<N>& other) const noexcept;

                template<uint64_t N>
                bool operator==(const string<N>& rhs) const noexcept;
                template<uint64_t N>
                bool operator!=(const string<N>& rhs) const noexcept;
                template<uint64_t N>
                bool operator<(const string<N>& rhs) const noexcept;
                template<uint64_t N>
                bool operator<=(const string<N>& rhs) const noexcept;
                template<uint64_t N>
                bool operator>(const string<N>& rhs) const noexcept;
                template<uint64_t N>
                bool operator>=(const string<N>& rhs) const noexcept;

                bool operator==(char const* const rhs) const noexcept;
                bool operator!=(char const* const rhs) const noexcept;

                const char* c_str() const noexcept;

                constexpr uint64_t size() const noexcept;
                static constexpr uint64_t capacity() noexcept;
                constexpr bool empty() const noexcept;
                operator std::string() const noexcept;

                template<typename T>
                string& operator+=(const T&) noexcept;

                template<typename T>
                typename std::enable_if<internal::IsCharArray<T>::value || internal::IsCxxString<T>::value, string&>::type
                append(const T& t)noexcept;

                template<typename T>
                typename std::enable_if<internal::IsCharArray<T>::value || internal::IsCxxString<T>::value, bool>::type
                unsafe_append(const T& t)noexcept;

                string substr(const uint64_t pos, const uint64_t count) const noexcept;
                string substr(const uint64_t pos = 0U) const noexcept;

                template<typename T>
                typename std::enable_if<std::is_same<T, std::string>::value || internal::IsCharArray<T>::value
                                        || internal::IsCxxString<T>::value, uint64_t>
                find(const T& t, const uint64_t pos = 0U) const noexcept;
                template<typename T>
                typename std::enable_if<std::is_same<T, std::string>::value || internal::IsCharArray<T>::value
                                        || internal::IsCxxString<T>::value, uint64_t>
                find_first_of(const T& t, const uint64_t pos = 0U) const noexcept;
                template<typename T>
                typename std::enable_if<std::is_same<T, std::string>::value || internal::IsCharArray<T>::value
                                        || internal::IsCxxString<T>::value, uint64_t>
                find_last_of(const T& t, const uint64_t pos = 0U) const noexcept;

                template<uint64_t N> friend class string;

                template<typename T1, typename T2>
                friend typename std::enable_if<(internal::IsCharArray<T1>::value || internal::IsCxxString<T1>::value)
                            && (internal::IsCharArray<T2>::value || internal::IsCxxString<T2>::value),
                        string<internal::GetCapa<T1>::capa + internal::GetCapa<T2>::capa>>::type
                concatenate(const T1& t1, const T2& t2) noexcept;

            private:
                template<uint64_t N>
                string& copy(const string<N>& rhs) noexcept;

                template<uint64_t N>
                string& move(string<N>&& rhs) noexcept;

                char m_rawstring[Capacity + 1U]{'\0'};
                uint64_t m_rawstringSize{0U};
            };

            template<uint64_t Capacity>
            inline bool operator==(const std::string& lhs, const string<Capacity>& rhs) noexcept;
            template<uint64_t Capacity>
            inline bool operator==(const string<Capacity>& rhs, const std::string& lhs) noexcept;
            template<uint64_t Capacity>
            inline bool operator!=(const std::string& lhs, const string<Capacity>& rhs) noexcept;
            template<uint64_t Capacity>
            inline bool operator!=(const string<Capacity>& rhs, const std::string& lhs) noexcept;
            template<uint64_t Capacity>
            inline bool operator==(const char* const lhs, const string<Capacity>& rhs) noexcept;
            template<uint64_t Capacity>
            inline bool operator!=(const char* const lhs, const string<Capacity>& rhs) noexcept;
            template<uint64_t Capacity>
            inline std::ostream& operator<<(std::ostream& stream, const string<Capacity>& str) noexcept;
        }
    }
}

#endif //MIDDLEWARE_STRING_HPP
