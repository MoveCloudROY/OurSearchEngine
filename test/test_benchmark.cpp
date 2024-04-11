#include "Queryer.h"
#include <spdlog/spdlog.h>
#include <benchmark/benchmark.h>
using namespace SG;


static void BM_Query(benchmark::State &state) {
    Queryer &queryer = Queryer::getInstance();
    for (auto _ : state) {
        auto ans = queryer.get("寿命", 0, 10);
        benchmark::DoNotOptimize(ans);
    }
}
BENCHMARK(BM_Query);

BENCHMARK_MAIN();