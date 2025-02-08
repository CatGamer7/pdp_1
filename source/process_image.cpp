#include <thread>
#include <iostream>
#include "process_image.hpp"


iio::Process_Bitmap::Process_Bitmap(image_array &in_bitmap, Coordinate &in_dimensions, uint8_t in_n_threads)
    :
    bitmap(in_bitmap),
    dimensions(in_dimensions)
{
    out_bitmap = image_array(in_bitmap);
    n_threads = in_n_threads;
};

iio::image_array& iio::Process_Bitmap::run() {

    auto start = std::chrono::steady_clock::now();

    if (n_threads > 1) {
        iio::image_array& temp = run_parallel();
        time_it_from_timestamp(start);
        return temp;
    }
    else {
        iio::image_array& temp = run_sequence();
        time_it_from_timestamp(start);
        return temp;
    }
}

iio::image_array& iio::Process_Bitmap::run_parallel()
{
    uint32_t segment_length = dimensions.y / n_threads;
    std::vector<std::thread> thread_vector;

    // Launch inversion
    for (uint32_t y = 0; y < dimensions.y; y += segment_length) {
        // last thread also handles any remainder
        if ((y + segment_length) >= dimensions.y) {
            thread_vector.push_back(
                std::thread(
                    [this, y] 
                    {
                        invert_segment(y, dimensions.y);
                    }
                )
            );
        }
        else {
            thread_vector.push_back(
                std::thread(
                    [this, y, segment_length] 
                    {
                        invert_segment(y, y + segment_length);
                    }
                )
            );
        }
    }

    // Join inversion
    for (auto& t : thread_vector) {
        t.join();
    }
    thread_vector.clear();

    // Launch convolution
    for (uint32_t y = 0; y < dimensions.y; y += segment_length) {
        // last thread also handles any remainder
        if ((y + segment_length) >= dimensions.y) {
            thread_vector.push_back(
                std::thread(
                    [this, y] 
                    {
                        run_segment(y, dimensions.y);
                    }
                )
            );
        }
        else {
            thread_vector.push_back(
                std::thread(
                    [this, y, segment_length] 
                    {
                        run_segment(y, y + segment_length);
                    }
                )
            );
        }
    }

    // Join convolution
    for (auto& t : thread_vector) {
        t.join();
    }
    thread_vector.clear();

    return out_bitmap;
}

iio::image_array &iio::Process_Bitmap::run_sequence() {
    invert_segment(0, dimensions.y);
    run_segment(0, dimensions.y);

    return out_bitmap;
}

void iio::Process_Bitmap::invert_segment(uint32_t start, uint32_t end) {
    // Invert
    for (uint32_t y = start; y < end; y++) {  
        for (uint32_t x = 0; x < dimensions.x; x++) {
            for (uint8_t c = 0; c < iio::CHANNELS; c++) {
                bitmap[c][y][x] = uint8_t(255) - bitmap[c][y][x];
            };
        };
    };
}

void iio::Process_Bitmap::run_segment(uint32_t start, uint32_t end) {
    // Convolve
    for (uint32_t y = start; y < end; y++) {  
        for (uint32_t x = 0; x < dimensions.x; x++) {
            for (uint8_t c = 0; c < iio::CHANNELS; c++) {

                int32_t top = 0;
                if (y > 0) {
                    top = bitmap[c][y - 1][x];
                };

                int32_t bottom = 0;
                if (y < dimensions.y - 1) {
                    bottom = bitmap[c][y + 1][x];
                };

                int32_t left = 0;
                if (x > 0) {
                    left = bitmap[c][y][x - 1];
                };

                int32_t right = 0;
                if (x < dimensions.x - 1) {
                    right = bitmap[c][y][x + 1];
                };

                // Prevent 8-bit integer overflow
                int32_t temp = 5 * bitmap[c][y][x] - top - bottom - left - right;

                // Clamp to a valid pixel value
                temp = std::max(0, temp);
                temp = std::min(255, temp);

                out_bitmap[c][y][x] = uint8_t(temp);
            };
        };
    };
}

void iio::Process_Bitmap::time_it_from_timestamp(std::chrono::steady_clock::time_point start) {
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Time elapsed for processing: " << duration << "ms\n";
};
