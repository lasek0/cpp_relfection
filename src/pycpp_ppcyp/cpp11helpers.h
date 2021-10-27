#pragma once

//c++11
/*
main.cpp:992:55: error: ‘std::enable_if_t’ has not been declared

unfortunatelly c++14 is minimal, since c++11 does not allow auto in lambdas
main.cpp:1545:95: error: use of ‘auto’ in lambda parameter declaration only available with ‘-std=c++14’ or ‘-std=gnu++14’

*/
#if __cplusplus < 201402L

// :(
//mainly due to tuple loop iterations

//https://gist.github.com/ntessore/dc17769676fb3c6daa1f
namespace std {
        //index_sequence
        //make_index_sequence
        template<typename T, T... Ints>
        struct integer_sequence
        {
                typedef T value_type;
                static constexpr std::size_t size() { return sizeof...(Ints); }
        };

        template<std::size_t... Ints>
        using index_sequence = integer_sequence<std::size_t, Ints...>;

        template<typename T, std::size_t N, T... Is>
        struct make_integer_sequence : make_integer_sequence<T, N-1, N-1, Is...> {};

        template<typename T, T... Is>
        struct make_integer_sequence<T, 0, Is...> : integer_sequence<T, Is...> {};

        template<std::size_t N>
        using make_index_sequence = make_integer_sequence<std::size_t, N>;

        template<typename... T>
        using index_sequence_for = make_index_sequence<sizeof...(T)>;


//https://en.cppreference.com/w/cpp/types/enable_if
    template< bool B, class T = void >
    using enable_if_t = typename enable_if<B,T>::type;

};

#endif

