#include "Helpers.h"

std::vector<bool> ToBoolVector(U64 input, U8 num_bits) {
  std::vector<bool> result;
  for (U8 i = 0; i < num_bits; i++)
    result.push_back(input & (1 << i));
  reverse(result.begin(), result.end());
  return result;
}

U64 BoolVectorToInt(const std::vector<bool> &input, U64 start_index, U8 num_bits) {
  U64 result = 0;
  for(U64 i = 0; i<num_bits; i++){
    if(input.at(start_index + i))
      result |= (1 << (num_bits - i - 1));
  }
  return result;
}

std::vector<bool> ExtendByteSequence(const std::vector<bool> &vec) {
  std::vector<bool> result = std::vector<bool>();
  for(int i=0; i<vec.size(); i++){
    // Add byte start sequence before each byte start
    if(i % 8 == 0){
      result.push_back(true);
      result.push_back(false);
    }

    // Add original bits
    result.push_back(vec.at(i));
  }
  return result;
}

// This is not a fast implementation at all. But we don't care
// TODO: Test if this is indeed correct!
U16 DumbCRC(std::vector<bool> input, U32 init, U32 polynomial, U8 num_bits) {
  U32 result = init;
  U64 mask = (1 << num_bits) - 1;

  for(U64 i = 0; i<input.size(); i++){
    // Perform XOR division
    result = result ^ polynomial;
    
    // Shift left and add new bit
    result <<= 1;
    result += (input[i] ? 1 : 0);

    // Mask again to 11 bits
    result &= mask;
  }

  return result;
}