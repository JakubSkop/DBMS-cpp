#pragma once

namespace DB {

    constexpr size_t HEADER_SIZE = 16652;
    constexpr size_t PAGE_SIZE = 4096;
    constexpr size_t BUFFER_SIZE = 256; //The max number of pages stored at once in the buffer
    constexpr size_t BP_TREE_SIZE = 4;

};