#pragma once

#include <cstdlib>
#include <iostream>

class Image
{
private:
    size_t width_ = 0;
    size_t height_ = 0;
    unsigned char* pixels_ = nullptr;

    size_t index(size_t row, size_t col) const;
public:
    Image() = default;
    Image(size_t wight_, size_t height_);
    Image(size_t size);
    ~Image();

    size_t width() const { return width_; }
    size_t height() const { return height_; }

    unsigned char& at(size_t row, size_t col);
    unsigned char at(size_t row, size_t col) const;
    void fill(unsigned char value);

    void print(std::ostream& os) const;
};

