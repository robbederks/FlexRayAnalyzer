#ifndef FLEXRAY_FRAME_H
#define FLEXRAY_FRAME_H

#include <LogicPublicTypes.h>

#include <AnalyzerResults.h>
#include "Helpers.h"
#include "FlexRayAnalyzerResults.h"

typedef struct {
  bool bit;
  S64 start_sample;
  S64 end_sample;
} DecoderBit;

class FlexRayFrame {
 public:
  FlexRayFrame();
  ~FlexRayFrame();

  std::vector<bool> GenerateStream(bool in_dynamic_section = false);
  void Decode(const std::vector<DecoderBit> &bitstream, std::vector<FrameSortingWrapper>* frames);
  void AddResultFrame(std::vector<FrameSortingWrapper>* frames, FlexRayFrameType type, S64 start_sample, S64 end_sample, U64 data1=0, U64 data2=0);

 public:
  bool payload_preamble;
  bool null_frame;
  bool sync_frame;
  bool startup_frame;
  U16 header_crc;
  U16 frame_id;
  U8 cycle_count;
  std::vector<U8> data;
  U32 crc;
};

#endif