#include "helper.h"

static void BM_Transpose_Naive(benchmark::State& state) {
  int N = state.range(0);
  int tile_size = N;
  int n_warmup = 3;
}
BENCHMARK(BM_Transpose_Naive)->MeasureProcessCPUTime()->ArgNames({"matrix_size"})->Arg(2048);

BENCHMARK_MAIN();