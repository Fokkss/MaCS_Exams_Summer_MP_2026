#include "ticket_lib1.hpp"

#include <stdexcept>

Image::Image(size_t width, size_t height)
    : width_(width)
    , height_(height)
    , pixels_(new unsigned char[width * height])
{
    fill(0);
}

Image::Image(size_t size)
    : Image(size, size)
{
}

Image::~Image()
{
    delete[] pixels_;
}

size_t Image::index(size_t row, size_t col) const
{
    if (row >= height_ || col >= width_)
    {
        throw std::out_of_range("index out of range");
    }

    return row * width_ + col;
}

unsigned char& Image::at(size_t row, size_t col)
{
    return pixels_[index(row, col)];
}

unsigned char Image::at(size_t row, size_t col) const
{
    return pixels_[index(row, col)];
}

void Image::fill(unsigned char value)
{
    for (size_t i = 0; i < width_ * height_; ++i)
    {
        pixels_[i] = value;
    }
}

void Image::print(std::ostream& out) const
{
    for (size_t row = 0; row < height_; ++row)
    {
        for (size_t col = 0; col < width_; ++col)
        {
            out << static_cast<int>(at(row, col)) << " ";
        }

        out << std::endl;
    }
}



int main()
{
    Image image(3, 2);

    image.fill(10);
    image.at(0, 1) = 255;
    image.at(1, 2) = 128;

    image.print(std::cout);

    Image icon(4);
    icon.fill(7);
    icon.print(std::cout);

    Image empty;
    std::cout << empty.width() << "x" << empty.height() << std::endl;

    return 0;
}
