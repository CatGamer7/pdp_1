#pragma once
#include <vector>
#include <memory>
#include <string>
#include <fstream>


namespace iio {

    using image_array = std::vector<std::vector<std::vector<uint8_t>>>; //RGB image [channel, width, height]
    using channel_array = std::vector<std::vector<uint8_t>>;
    using row_array = std::vector<uint8_t>;
    
    static constexpr uint8_t CHANNELS = 3;
    static constexpr uint8_t BIT_DEPTH = 24;
    static constexpr uint32_t BITMAP_FILESIZE_ADRESS = 2;
    static constexpr uint32_t BITMAP_OFFSET_ADRESS = 10;
    static constexpr uint32_t BITMAP_WIDTH_ADRESS  = 18;
    static constexpr uint32_t BITMAP_HEIGHT_ADRESS = 22;
    static constexpr uint32_t BITMAP_RAW_SIZE_ADRESS = 34;
    static constexpr uint32_t BITMAP_WIDTH_PPM_ADRESS = 38;
    static constexpr uint32_t BITMAP_HEIGHT_PPM_ADRESS = 42;

    struct Coordinate {
        Coordinate() {};
        Coordinate(uint32_t in_x, uint32_t in_y) {
            x = in_x;
            y = in_y;
        };

        uint32_t x, y;
    };


    class IO_BMP_Image {

    public:
        IO_BMP_Image(const std::string& in_filename);

        image_array& get_bitmap();
        Coordinate& get_dimensions();
        
        void write_bitmap_file(
            const std::string& out_filename,
            const image_array& out_bitmap
        );

    private:        
        uint32_t read_uint(std::ifstream& stream);
        void read_file();

        void write_uint(uint32_t to_write, std::ofstream& stream);
        void write_zeroes(uint32_t count, std::ofstream& stream);

        image_array bitmap;
        std::string filename;
        Coordinate dimensions;
        uint32_t bitmap_array_offset;
        uint32_t bitmap_row_pad_size;
        uint32_t bitmap_filesize;
        uint32_t bitmap_raw_size;
        Coordinate pixels_per_metre;
    };
};
