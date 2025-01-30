#include <iostream>
#include <string>
#include "io_image.hpp"
#include "process_image.hpp"

int main(int argc, char* argv[]) {

    // User input validation
	if ((argc < 3) || (argc > 4)) {
		std::wcout << L"Usage: a.exe {in_file}.bmp {out_file}.bmp [n_threads]";
		return EXIT_FAILURE;
	};

    std::string in_file = argv[1];
    std::string out_file = argv[2];
    uint8_t n_threads = 1;

    if (argc == 4) {
        try {
            n_threads = std::stoi(argv[3]);
        }
        catch (std::invalid_argument) {
            n_threads = 1;
        }
    };

    // Image processing
    {
        using namespace iio;

        // Read from file
        IO_BMP_Image image_obj = IO_BMP_Image(in_file);
        auto& bitmap = image_obj.get_bitmap();
        auto& dimensions = image_obj.get_dimensions();

        // Modify bitmap
        Process_Bitmap process_obj = Process_Bitmap(
            bitmap,
            dimensions,
            n_threads
        );
        auto& out_bitmap = process_obj.run();

        // Write to file
        image_obj.write_bitmap_file(out_file, out_bitmap);
    };
}