#pragma once

#include <array>
#include <string>
#include <numeric>
#include <iostream>


template <size_t N>
class FixedString{
    std::array<char, N> characters;

    public:

        FixedString(){
            characters[0] = '\0';
        }

        FixedString(const std::string& str){
            size_t i{0};
            for (; i < std::min(str.length(), N); i++){
                characters[i] = str[i];
            }
            if (i != N){
                characters[i] = '\0';
            }    
        };
        
        char& operator[] (size_t pos){
            return characters[pos];
        };

        operator std::string() const{
            std::string result = "";
            size_t counter = 0;
            while (characters[counter] != '\0' and counter < N){
                result += (characters[counter]);
                counter++;
            }
            return result;
        }

};

template <size_t N>
std::ostream& operator<<(std::ostream& os, const FixedString<N>& obj){
    os << (std::string)obj;
    return os;
};