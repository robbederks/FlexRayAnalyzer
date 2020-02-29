#include "FlexRayFrame.h"

// Constructor
FlexRayFrame::FlexRayFrame() {}

// Deconstructor
FlexRayFrame::~FlexRayFrame() {}

// Generates a stream of bools at the normal bitrate
std::vector<bool> FlexRayFrame::GenerateStream(bool in_dynamic_section) {
  std::vector<bool> result = std::vector<bool>();
  std::vector<bool> tmp_vector;

  // Add Transmission Start Sequence
  tmp_vector = std::vector<bool>(TSS_LEN, false);
  result.insert(result.end(), tmp_vector.begin(), tmp_vector.end());

  // Add Frame Start Sequence
  result.push_back(true);

  // Start raw data frame
  std::vector<bool> raw_frame_data = std::vector<bool>();

  // Header
  raw_frame_data.push_back(false);             // Reserved bit
  raw_frame_data.push_back(payload_preamble);  // Payload preamble indicator
  raw_frame_data.push_back(null_frame);        // Null frame indicator
  raw_frame_data.push_back(sync_frame);        // Sync frame indicator
  raw_frame_data.push_back(startup_frame);     // Startup frame indicator

  // 0 is not a valid frame_id!
  tmp_vector = ToBoolVector(frame_id, 11);
  raw_frame_data.insert(raw_frame_data.end(), tmp_vector.begin(), tmp_vector.end());

  // Payload length is the number of bytes / 2, and is constant and uniform for all frames in the static segment. 
  // We calculate it on the fly here because we're lazy
  tmp_vector = ToBoolVector((data.size() / 2), 7);
  raw_frame_data.insert(raw_frame_data.end(), tmp_vector.begin(), tmp_vector.end());

  // Header CRC, range = [sync_frame_indicator : payload length], so from bit 3 until bit 22
  tmp_vector = ToBoolVector(DumbCRC(std::vector<bool>(raw_frame_data.begin(), raw_frame_data.end()),CRC11_INIT, CRC11_POLYNOMIAL, 11), 11);
  raw_frame_data.insert(raw_frame_data.end(), tmp_vector.begin(), tmp_vector.end());

  // Cycle count
  tmp_vector = ToBoolVector(cycle_count, 6);
  raw_frame_data.insert(raw_frame_data.end(), tmp_vector.begin(), tmp_vector.end());

  // Actual data
  for (U8 i = 0; i < data.size(); i++) {
    tmp_vector = ToBoolVector(data[i], 8);
    raw_frame_data.insert(raw_frame_data.end(), tmp_vector.begin(), tmp_vector.end());
  }

  // Add BSS and append to result
  tmp_vector = ExtendByteSequence(raw_frame_data);
  result.insert(result.end(), tmp_vector.begin(), tmp_vector.end());

  // Frame CRC, range = full message before adding BSS?
  // Also add BSS to this before adding to result
  // TODO: Check that this is actually correct
  tmp_vector = ExtendByteSequence(ToBoolVector(DumbCRC(raw_frame_data, CRC24_INIT, CRC24_POLYNOMIAL, 24), 24));
  result.insert(result.end(), tmp_vector.begin(), tmp_vector.end());

  // Add Frame End Sequence
  result.push_back(false);
  result.push_back(true);

  // Add DTS if applicable
  if(in_dynamic_section){
    // TODO: Implement
  }

  return result;
}

// TODO: Make this fill in the actual parameters
void FlexRayFrame::Decode(const std::vector<DecoderBit> &bitstream, std::vector<FrameSortingWrapper>* frames){
  int i = 0;

  // Flags
  if(bitstream.size() - i < 5)
    return;

  U64 start_sample = bitstream.at(i).start_sample;
  i++;  // Reserved bit
  payload_preamble = bitstream.at(i++).bit;
  null_frame = bitstream.at(i++).bit;
  sync_frame = bitstream.at(i++).bit;
  startup_frame = bitstream.at(i++).bit;
  U64 flags = ((int) payload_preamble << 3) | ((int) null_frame << 2) | ((int) sync_frame << 1) | ((int) startup_frame << 0);
  AddResultFrame(frames, FlagsField, start_sample, bitstream.at(i-1).end_sample, flags);

  // Frame ID
  if(bitstream.size() - i < 11)
    return;

  start_sample = bitstream.at(i).start_sample;
  U16 frame_id = 0;
  for(int j=10; j>=0; j--)
    frame_id |= (bitstream.at(i++).bit << j);
  AddResultFrame(frames, FrameIdField, start_sample+1, bitstream.at(i-1).end_sample, frame_id);

  // Payload length
  // This is half the length!!!
  if(bitstream.size() - i < 7)
    return;
  
  start_sample = bitstream.at(i).start_sample;
  U16 payload_length = 0;
  for(int j=6; j>=0; j--)
    payload_length |= (bitstream.at(i++).bit << j);
  AddResultFrame(frames, PayloadLengthField, start_sample+1, bitstream.at(i-1).end_sample, payload_length);

  // Header CRC
  if(bitstream.size() - i < 11)
    return;
  
  start_sample = bitstream.at(i).start_sample;
  U16 header_crc = 0;
  for(int j=10; j>=0; j--)
    header_crc |= (bitstream.at(i++).bit << j);
  AddResultFrame(frames, HeaderCRCField, start_sample+1, bitstream.at(i-1).end_sample, header_crc);

  // Cycle count
  if(bitstream.size() - i < 6)
    return;
  
  start_sample = bitstream.at(i).start_sample;
  U16 cycle_count = 0;
  for(int j=5; j>=0; j--)
    cycle_count |= (bitstream.at(i++).bit << j);
  AddResultFrame(frames, CycleCountField, start_sample+1, bitstream.at(i-1).end_sample, cycle_count);

  // Data
  std::vector<U8> data = std::vector<U8>();
  for(int k=0; k<(payload_length*2); k++){
    if(bitstream.size() - i < 8)
      return;

    start_sample = bitstream.at(i).start_sample;
    U8 data_byte = 0;
    for(int j=7; j>=0; j--)
      data_byte |= (bitstream.at(i++).bit << j);
    data.push_back(data_byte);
    AddResultFrame(frames, DataField, start_sample+1, bitstream.at(i-1).end_sample, data_byte, k);
  }

  // CRC
  if(bitstream.size() - i < 24)
    return;
  
  start_sample = bitstream.at(i).start_sample;
  U32 crc = 0;
  for(int j=23; j>=0; j--)
    crc |= (bitstream.at(i++).bit << j);
  AddResultFrame(frames, CRCField, start_sample+1, bitstream.at(i-1).end_sample, crc);
}

void FlexRayFrame::AddResultFrame(std::vector<FrameSortingWrapper>* frames, FlexRayFrameType type, S64 start_sample, S64 end_sample, U64 data1, U64 data2){
  Frame frame;
  frame.mType = type;
  frame.mData1 = data1;
  frame.mData2 = data2;
  frame.mFlags = 0;
  frame.mStartingSampleInclusive = start_sample;
  frame.mEndingSampleInclusive = end_sample;

  FrameSortingWrapper fsw;
  fsw.f = frame;

  frames->push_back(fsw);
}