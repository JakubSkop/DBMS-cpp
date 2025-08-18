#pragma once

#include <cstddef>
#include <variant>
#include <vector>
#include <utility>

template<typename... Ts> 
struct Typelist{
    static constexpr size_t count{sizeof...(Ts)};
};

//template<typename T> 
//struct Typetag{};

//template<class... Ts>
//std::tuple<Ts...> typelist_to_tuple(Typelist<Ts...>);

template<class... Ts>
std::variant<Ts...> typelist_to_variant(Typelist<Ts...>);

template<template<typename> class W, class... Ts>
std::variant<W<Ts>...> typelist_to_variant_of_wrapped_types(Typelist<Ts...>);

//template<class T> 
//using tupleOfTypes = decltype(typelist_to_tuple(std::declval<T>()));

template<class T> 
using variantOfTypes = decltype(typelist_to_variant(std::declval<T>()));

template<template <typename> class W, class T> 
using variantofWrappedTypes = decltype(typelist_to_variant_of_wrapped_types<W>(std::declval<T>()));

template<class T> 
using variantofVectors = variantofWrappedTypes<std::vector, T>;


