/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <benchmark/benchmark.h>

#include <iostream>

#include "firfilter.h"
#include "iirfilter.h"
#include "randomop.h"

namespace mcl {

static void FirFilterSequentialBenchmark(benchmark::State& state) {
  const size_t filter_order = state.range(0);
  const size_t batch_size = state.range(1);

  RandomGenerator random_generator;

  std::vector<Real> impulse_response = random_generator.Rand(filter_order);

  FirFilter fir_filter(impulse_response);

  std::vector<Real> input(batch_size);
  std::vector<Real> output(batch_size);

  while (state.KeepRunning()) {
    state.PauseTiming();
    input = random_generator.Rand(batch_size);
    state.ResumeTiming();

    for (size_t i = 0; i < batch_size; ++i) {
      output[i] = fir_filter.ProcessSample(input[i]);
    }
    benchmark::ClobberMemory();  // Force output to be written to memory.
  }
}
BENCHMARK(FirFilterSequentialBenchmark)
    ->RangeMultiplier(8)
    ->Ranges({{1, 1 << 7}, {1, 1 << 8}});

static void FirFilterBenchmark(benchmark::State& state) {
  const size_t filter_order = state.range(0);
  const size_t batch_size = state.range(1);

  RandomGenerator random_generator;

  std::vector<Real> impulse_response = random_generator.Rand(filter_order);

  FirFilter fir_filter(impulse_response);

  std::vector<Real> input(batch_size);
  std::vector<Real> output(batch_size);

  while (state.KeepRunning()) {
    state.PauseTiming();
    input = random_generator.Rand(batch_size);
    state.ResumeTiming();

    fir_filter.ProcessBlock(input.data(), input.size(), output.data());
    benchmark::ClobberMemory();  // Force output to be written to memory.
  }
}
BENCHMARK(FirFilterBenchmark)
    ->RangeMultiplier(8)
    ->Ranges({{1, 1 << 7}, {1, 1 << 8}});

static void IirFilterBenchmark(benchmark::State& state) {
  const size_t filter_order = state.range(0);

  RandomGenerator random_generator;

  std::vector<Real> B = random_generator.Rand(filter_order);
  std::vector<Real> A = random_generator.Rand(filter_order);
  A[0] = 1.0;

  IirFilter iir_filter(B, A);
  Real input;
  Real output;

  while (state.KeepRunning()) {
    state.PauseTiming();
    input = rand();
    state.ResumeTiming();

    output = iir_filter.ProcessSample(input);
    benchmark::ClobberMemory();  // Force output to be written to memory.
  }
}
BENCHMARK(IirFilterBenchmark)->RangeMultiplier(64)->Range(1, 1 << 10);

}  // namespace mcl

BENCHMARK_MAIN();
