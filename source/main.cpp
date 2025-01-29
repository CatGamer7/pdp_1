#include <iostream>
#include <string>
#include "io_image.hpp"

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::wcout << L"Usage: a.exe {in_file}.bmp {out_file}.bmp";
		return EXIT_FAILURE;
	};

    std::string in_file = argv[1];
    std::string out_file = argv[2];

    {
        using namespace iio;

        IO_BMP_Image image_class = IO_BMP_Image(in_file);
        auto& a = image_class.get_bitmap();
        image_class.write_bitmap_file(out_file);
    };
}