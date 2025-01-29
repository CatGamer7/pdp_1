#include <fstream>
#include "io_image.hpp"

iio::IO_BMP_Image::IO_BMP_Image(const std::string &in_filename) {
    filename = in_filename;
}

iio::image_array& iio::IO_BMP_Image::get_bitmap() {
    if (bitmap.empty()) {
        read_file();
    }
    return bitmap;
}

iio::Coordinate& iio::IO_BMP_Image::get_dimensions() {
    if (bitmap.empty()) {
        read_file();
    }
    return dimensions;
}

uint32_t iio::IO_BMP_Image::read_uint(std::ifstream &stream) {
    uint32_t out = 0;

    for (uint8_t i = 0; i < 4; i++) {
        const uint32_t temp = stream.get() << (8 * i);
        out += temp;
    }

    return out;
}

void iio::IO_BMP_Image::read_file() {
    std::ifstream file = std::ifstream(filename);

    // Get bitmap filesize
    file.seekg(BITMAP_FILESIZE_ADRESS);
    bitmap_filesize = read_uint(file);

    // Get bitmap offset as u_int
    file.seekg(BITMAP_OFFSET_ADRESS);
    const uint32_t array_offset = read_uint(file);
    bitmap_array_offset = array_offset;

    // Get bitmap width as u_int
    file.seekg(BITMAP_WIDTH_ADRESS);
    const uint32_t width = read_uint(file);

    // Get bitmap height as u_int
    file.seekg(BITMAP_HEIGHT_ADRESS);
    const uint32_t height = read_uint(file);
    
    // Get bitmap raw size as u_int
    file.seekg(BITMAP_RAW_SIZE_ADRESS);
    bitmap_raw_size = read_uint(file);

    dimensions = Coordinate(width, height);
    const uint32_t row_byte_size = ((BIT_DEPTH * width + 31) / 32) * 4; // https://en.wikipedia.org/wiki/BMP_file_format#Pixel_storage
    const uint32_t row_pad_size = row_byte_size - (width * CHANNELS);
    bitmap_row_pad_size = row_pad_size;

    file.seekg(array_offset);
    bitmap = iio::image_array(
        CHANNELS,
        iio::channel_array(

            height,
            iio::row_array(

                width,
                0
            )
        )
    );
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            // Bitmap is read as: Blue, Green, Red...
            const uint8_t blue  = file.get();
            const uint8_t green = file.get();
            const uint8_t red   = file.get();

            // ...and stored as: Red, Green, Blue
            bitmap[0][y][x] = red;
            bitmap[1][y][x] = green;
            bitmap[2][y][x] = blue;
        }

        file.seekg(row_pad_size, std::ios_base::cur); // Skip pad
    }

    file.close();
}

void iio::IO_BMP_Image::write_uint(uint32_t to_write, std::ofstream &stream) {
    for (uint8_t i = 1; i < 5; i++) {
        const uint8_t temp = to_write & 255u;
        stream.put(temp);
        to_write = to_write >> (8 * i);
    };
}

void iio::IO_BMP_Image::write_zeroes(uint32_t count, std::ofstream &stream) {
    for (uint8_t i = 0; i < count; i++) {
        stream.put('\0');
    };
}

void iio::IO_BMP_Image::write_bitmap_file(const std::string &out_filename) {
    std::ofstream file = std::ofstream(out_filename);

    // Bitmap header
    file.put('B');
    file.put('M');
    write_uint(bitmap_filesize, file);
    write_zeroes(4, file); // Gap
    write_uint(bitmap_array_offset, file);

    // Bitmap information header
    write_uint(40, file); // Header length
    write_uint(dimensions.x, file); // Bitmap width
    write_uint(dimensions.y, file); // Bitmap height
    file.put(char(1)); // Must be 1 (color plane)
    file.put(char(0)); //  Must be 0 (color plane)
    file.put(char(24)); // Bit Depth
    file.put(char(0)); // Bit Depth (cont)
    write_uint(0, file); // Compression: none
    write_uint(bitmap_raw_size, file);
    write_zeroes(16, file); // Gap

    // Bitmap
    for (uint32_t y = 0; y < dimensions.y; y++) {
        for (uint32_t x = 0; x < dimensions.x; x++) {
            // Bitmap is stored as: Red, Green, Blue...
            const uint8_t red   = bitmap[0][y][x];
            const uint8_t green = bitmap[1][y][x];
            const uint8_t blue  = bitmap[2][y][x];

            // ...and stored as: Blue, Green, Red
            file.put(blue);
            file.put(green);
            file.put(red);
        }

        write_zeroes(bitmap_row_pad_size, file); // Add pad
    };

    file.close();
}
