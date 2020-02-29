#include "FlexRaySimulationDataGenerator.h"

#include "FlexRayFrame.h"

FlexRaySimulationDataGenerator::FlexRaySimulationDataGenerator() {
}

FlexRaySimulationDataGenerator::~FlexRaySimulationDataGenerator() {}

void FlexRaySimulationDataGenerator::Initialize(U32 simulation_sample_rate, FlexRayAnalyzerSettings *settings) {
  mSimulationSampleRateHz = simulation_sample_rate;
  mSettings = settings;

  mSerialSimulationData.SetChannel(mSettings->mInputChannel);
  mSerialSimulationData.SetSampleRate(simulation_sample_rate);
  mSerialSimulationData.SetInitialBitState(BIT_HIGH);
}

U32 FlexRaySimulationDataGenerator::GenerateSimulationData(U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor **simulation_channel) {
  U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample(largest_sample_requested, sample_rate, mSimulationSampleRateHz);
  U32 samples_per_bit = mSimulationSampleRateHz / mSettings->mBitRate;

  FlexRayFrame test_frame;
  test_frame.payload_preamble = false;
  test_frame.null_frame = false;
  test_frame.sync_frame = false;
  test_frame.startup_frame = false;
  test_frame.frame_id = 0x12;
  test_frame.cycle_count = 10;

  U8 data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  test_frame.data = std::vector<U8>(data, data + (sizeof(data) / sizeof(U8)));

  std::vector<bool> bitstream;
  while (mSerialSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested) {
    // Generate frame
    test_frame.cycle_count++;
    test_frame.cycle_count %= 64;
    bitstream = test_frame.GenerateStream();

    // Idle for IDLE_PERIOD_LEN bits
    mSerialSimulationData.Advance(samples_per_bit * IDLE_PERIOD_LEN);

    // Send frame
    for (int i = 0; i < bitstream.size(); i++) {
      bool bit = bitstream.at(i);
      BitState bit_s = bit ? BIT_HIGH : BIT_LOW;
      mSerialSimulationData.TransitionIfNeeded(bit_s);
      mSerialSimulationData.Advance(samples_per_bit);
    }
    mSerialSimulationData.TransitionIfNeeded(BIT_HIGH);
  }

  // Idle for IDLE_PERIOD_LEN bits
  mSerialSimulationData.Advance(samples_per_bit * IDLE_PERIOD_LEN);

  *simulation_channel = &mSerialSimulationData;
  return 1;
}