constexpr const char* USAGE =
 R"(rmat : An RMAT graph generator

  Usage:
      rmat (-h | --help)
      rmat --version
      rmat SCALE [--edgefactor <factor>] [--seed <value> | --device]

  Options:
      --help, -h             Show this screen.
      --version              Show the version.
      --edgefactor <factor>  Half the average degree [default: 16].
      --seed <value>         A seed for random number generation [default: 0].
      --device               Use std::random_device for seed.
)";

#include <docopt.h>
#include <fmt/ranges.h>
#include <cstdint>
#include <cmath>
#include <random>

using i64 = std::int64_t;
using f32 = float;

auto main(int argc, char const* argv[]) -> int
{
    std::map args = docopt::docopt(USAGE, {argv + 1, argv + argc});

    i64 scale = args["SCALE"].asLong();
    if (scale < 0 or 64 <= scale) {
        fmt::print(stderr, "rmat only supports scales from 0 to 63, got SCALE={}\n", scale);
        exit(1);
    }

    i64 edge_factor = args["--edgefactor"].asLong();
    if (edge_factor < 0) {
        fmt::print(stderr, "edge factor must be positive, got --edgefactor={}\n", edge_factor);
        exit(1);
    }

    i64 seed = args["--seed"].asLong();
    if (args["--device"].asBool()) {
        seed = std::random_device{}();
    }

    i64 n_vertices = std::pow(2, scale);
    i64 n_edges = n_vertices * edge_factor;

    fmt::print("n: {:g}, m: {:g}, seed: {}\n", f32(n_vertices), f32(n_edges), seed);

    f32 a = 0.57;
    f32 b = 0.19;
    f32 c = 0.19;

    f32 ab = a + b;
    f32 abc = ab + c;

    std::mt19937 rand(seed);
    std::uniform_real_distribution<f32> dist(f32(0), f32(1));

    // Precompute a vertex id distribution.
    std::vector<i64> permutation(n_vertices);
    std::iota(permutation.begin(), permutation.end(), i64(0));
    std::shuffle(permutation.begin(), permutation.end(), rand);

    // Right now we're just accumulating counts
    std::vector<i64> counts(n_vertices);

    for (i64 i = 0; i < n_edges; ++i) {
        i64 u = 0;
        i64 v = 0;
        for (i64 bit = 0; bit < scale; ++bit, u <<= 1, v <<= 1) {
            f32 f = dist(rand);
            // a% of the time we have 0 bits in both
            // b% of the time we have a 1 bit in the target
            // c% of the time we have a 1 bit in the source
            // the rest of the time we have 1 bits in both
            if (f < a) {
                continue;
            }
            else if (f < ab) {
                v += 1;
            }
            else if (f < abc) {
                u += 1;
            }
            else {
                u += 1;
                v += 1;
            }
        }
        u >>= 1;
        v >>= 1;

        counts[permutation[u]] += 1;
        counts[permutation[v]] += 1;
    }

    fmt::print("{}\n", counts);
    fmt::print("average:{}\n", std::reduce(counts.begin(), counts.end()) / counts.size());

    return 0;
}
