//
// Created by syj on 2023/3/1.
//

#ifndef MIDDLEWARE_STRING_INTERNAL_HPP
#define MIDDLEWARE_STRING_INTERNAL_HPP

#include <string>
#include <cstdint>
#include <cstring>

namespace middleware{
    namespace hoofs{
        namespace cxx_stl{
            template<uint64_t>
            class string;

            namespace internal {
                template<uint64_t N>
                using charArray = char[N];

                template<typename T>
                struct GetCapa {
                    static constexpr uint64_t capa = 0U;
                };
                template<uint64_t N>
                struct GetCapa<string<N>> {
                    static constexpr uint64_t capa = N;
                };
                template<uint64_t N>
                struct GetCapa<char[N]> {
                    static constexpr uint64_t capa = N - 1U;
                };

                template<typename T>
                struct GetSize;

                template<uint64_t N>
                struct GetSize<string<N>> {
                    static uint64_t call(const charArray<N> &data) noexcept {
                        return strnlen(data, N);
                    }
                };

                template<>
                struct GetSize<std::string> {
                    static uint64_t call(const std::string &data) noexcept {
                        return data.size();
                    }
                };

                template<typename T>
                struct GetData;

                template<uint64_t N>
                struct GetData<string<N>> {
                    static const char *call(const string<N> &data) noexcept {
                        return data.c_str();
                    }
                };

                template<uint64_t N>
                struct GetData<char[N]> {
                    static const char *call(const charArray<N> &data) noexcept {
                        return &data[0];
                    }
                };

                template<>
                struct GetData<std::string> {
                    static const char *call(const std::string &data) noexcept {
                        return data.data();
                    }
                };

                template<typename... Targs>
                struct SumCapa;
                template<>
                struct SumCapa<> {
                    static constexpr uint64_t value = 0U;
                };
                template<typename T, typename... Targs>
                struct SumCapa<T, Targs...> {
                    static constexpr uint64_t value = GetCapa<T>::capa + SumCapa<Targs...>::value;
                };

                template<typename T>
                struct IsCharArray {
                    static constexpr bool value = false;
                };
                template<uint64_t N>
                struct IsCharArray<char[N]> {
                    static constexpr bool value = true;
                };
                template<typename T>
                struct IsCxxString {
                    static constexpr bool value = false;
                };
                template<uint64_t N>
                struct IsCxxString<string<N>> {
                    static constexpr bool value = true;
                };
            }
        }
    }
}
#endif //MIDDLEWARE_STRING_INTERNAL_HPP
