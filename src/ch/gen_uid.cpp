#include <ch/gen_uid.hpp>

#include <random>

std::uint64_t ch::gen_uid()
{
    std::random_device rd;
    std::uniform_int_distribution<std::uint64_t> dist { 1000000000000000, 10000000000000000 };
    return dist(rd);
}