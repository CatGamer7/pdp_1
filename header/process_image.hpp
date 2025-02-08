#pragma once
#include <chrono>
#include "io_image.hpp"


namespace iio {

    class Process_Bitmap {
    
    public:
        Process_Bitmap(image_array& in_bitmap, Coordinate& in_dimensions, uint8_t in_n_threads);

        image_array& run();
        image_array& run_parallel();
        image_array& run_sequence();

    private:
        void invert_segment(uint32_t start, uint32_t end);
        void run_segment(uint32_t start, uint32_t end);
        void time_it_from_timestamp(std::chrono::steady_clock::time_point start);

        image_array& bitmap;
        image_array out_bitmap;
        Coordinate& dimensions;
        uint8_t n_threads;

    };
}
