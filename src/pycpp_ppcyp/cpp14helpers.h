#pragma once
/** handle tuples to iterate through in for loop using variable non-const integer index */
//trick with apply

/** reimplement c++17 std::apply  */
//https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer
#if __cplusplus < 201703L
namespace std {

template<typename F, typename T, size_t ... I>
static auto apply(F f, T& t, std::index_sequence<I ...>) {
    return f(std::get<I>(t) ...);
}

template<typename F, typename T>
static auto apply(F f, T& t) {
    const size_t size = std::tuple_size<T>::value;
    return apply(f, t, std::make_index_sequence<size>{});
}

};
#endif

