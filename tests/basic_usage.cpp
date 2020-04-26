#include <mooslib.h>

#include <fmt/format.h>

int main()
{
    using namespace moos;

    fmt::print("Numeric types:\n");
    {
        {
            i8 a = std::numeric_limits<i8>::max();
            i16 b = std::numeric_limits<i16>::max();
            i32 c = std::numeric_limits<i32>::max();
            i64 d = std::numeric_limits<i64>::max();
            fmt::print(" signed integers: i8={}, i16={}, i32={}, i64={}\n", a, b, c, d);
        }

        {
            u8 a = std::numeric_limits<u8>::max();
            u16 b = std::numeric_limits<u16>::max();
            u32 c = std::numeric_limits<u32>::max();
            u64 d = std::numeric_limits<u64>::max();
            fmt::print(" unsigned integers: u8={}, u16={}, u32={}, u64={}\n", a, b, c, d);
        }

        {
            f32 a = std::numeric_limits<f32>::max();
            f64 b = std::numeric_limits<f64>::max();
            fmt::print(" float types: f32={}, u64={}\n", a, b);
        }
    }

    fmt::print("Basic file IO:\n");
    {
        std::string text = readEntireFile("include/mooslib.h");
        fmt::print(" read {} characters from mooslib.h", text.length());
    }

    fmt::print("vec2:\n");
    // TODO

    vec2 fv2 { 1, 1 };
    float fv2l = length(fv2);
    vec2 fv2n = normalize(fv2);

    // should not compile:
    //ivec2 iv2 { 1, 1 };
    //i32 iv2l = length(iv2);
    //ivec2 iv2n = normalize(iv2);

    fmt::print("vec3:\n");
    // TODO

    // etc..
}
