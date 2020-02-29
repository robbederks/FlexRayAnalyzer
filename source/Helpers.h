#ifndef HELPERS_H
#define HELPERS_H

#include <vector>
#include <algorithm>
#include <LogicPublicTypes.h>
#include <AnalyzerResults.h>

typedef struct FrameSortingWrapper {
  Frame f;
  bool operator < (const FrameSortingWrapper& f1) const {
    return f.mStartingSampleInclusive < f1.f.mStartingSampleInclusive;
  }
} FrameSortingWrapper;

std::vector<bool> ToBoolVector(U64 input, U8 num_bits);
U64 BoolVectorToInt(const std::vector<bool> &input, U64 start_index, U8 num_bits);
std::vector<bool> ExtendByteSequence(const std::vector<bool> &vec);
U16 DumbCRC(std::vector<bool> input, U32 init, U32 polynomial, U8 num_bits);

#endif