#pragma once


class Bool{
    bool value = false;

    public:

        Bool() : value(false) {};
        Bool(bool b) : value(b) {};

        Bool& operator=(bool b) {
            value = b;
            return *this;
        }

        operator bool() const{
            return value;
        };

        bool operator==(const Bool& other) const = default;

        Bool operator!() const { return Bool(!value); }

};