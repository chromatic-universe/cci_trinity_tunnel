// brube godlberg for main_unique  rb.h

#pragma once

#include <memory>
#include <utility>

template<typename T , typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params )
{
        return std::unique_ptr<T>( new T( std::forward<Ts>(params)...) );
}




