#include "FlexRayAnalyzer.h"

FlexRayAnalyzer::FlexRayAnalyzer() : Analyzer2() {
  mSettings = std::auto_ptr<FlexRayAnalyzerSettings>(new FlexRayAnalyzerSettings());
  mSimulationInitialized = false;
  mFrameBuffer = std::vector<FrameSortingWrapper>();
  SetAnalyzerSettings(mSettings.get());
}

FlexRayAnalyzer::~FlexRayAnalyzer() {
  KillThread();
}

void FlexRayAnalyzer::SetupResults() {
  mResults.reset(new FlexRayAnalyzerResults(this, mSettings.get()));
  SetAnalyzerResults(mResults.get());
  mResults->AddChannelBubblesWillAppearOn(mSettings->mInputChannel);
}

void FlexRayAnalyzer::WorkerThread() {
  InitAnalyzer();

  // Keep analysing as long as we are not killed
  U64 starting_sample = mFlexRay->GetSampleNumber();
  std::vector<DecoderBit> bits = std::vector<DecoderBit>();
  while(true){
    // Remove all bits
    bits.clear();

    // Jump to next frame start
    WaitForNextFrame();

    // Shift half a bit to sample mid-bit
    U64 mSamplesPerHalfBit = mSamplesPerBit / 2;
    mFlexRay->Advance(mSamplesPerHalfBit);

    // Skip the TSS
    //starting_sample = mFlexRay->GetSampleNumber();
    //while(SampleBit() == false){}
    //AddResultFrame(TSSField, starting_sample - mSamplesPerHalfBit, mFlexRay->GetSampleNumber() - mSamplesPerBit - mSamplesPerHalfBit);

    // Skip the FSS
    SampleBit();
    starting_sample = mFlexRay->GetSampleNumber() - mSamplesPerBit;
    AddResultFrame(FSSField, starting_sample - mSamplesPerHalfBit, mFlexRay->GetSampleNumber() - mSamplesPerHalfBit);

    // Start sampling the bits until we reach the end of the frame
    while(true){
      // Read BSS
      starting_sample = mFlexRay->GetSampleNumber();
      if(SampleBit(true) != true || SampleBit(true) != false){
        // This byte is wrong!
        break;
      }
      // Don't add the BSS as frame anymore, since this interferes with overlapping data :/
      // AddResultFrame(BSSField, starting_sample - mSamplesPerHalfBit, mFlexRay->GetSampleNumber() - mSamplesPerHalfBit);

      // Read 8 bits to byte
      for(int i=7; i>=0; i--){
        DecoderBit bit;
        bit.start_sample = mFlexRay->GetSampleNumber() - mSamplesPerHalfBit;
        bit.bit = SampleBit();
        bit.end_sample = mFlexRay->GetSampleNumber() - mSamplesPerHalfBit;
        bits.push_back(bit);
      }
    }
    // Decode frame
    FlexRayFrame f = FlexRayFrame();
    f.Decode(bits, &mFrameBuffer);

    // Commit sorted frames to screen
    CommitFrames();

    // Commit packet
    mResults->CommitPacketAndStartNewPacket();

    // Report decoding progress
    ReportProgress(mFlexRay->GetSampleNumber());
  }
}

void FlexRayAnalyzer::AddResultFrame(FlexRayFrameType type, S64 start_sample, S64 end_sample, U64 data1, U64 data2){
  Frame frame;
  frame.mType = type;
  frame.mData1 = data1;
  frame.mData2 = data2;
  frame.mFlags = 0;
  frame.mStartingSampleInclusive = start_sample;
  frame.mEndingSampleInclusive = end_sample;

  FrameSortingWrapper fsw;
  fsw.f = frame;

  mFrameBuffer.push_back(fsw);
}

void FlexRayAnalyzer::CommitFrames(){
  std::sort(mFrameBuffer.begin(), mFrameBuffer.end());
  for(int i=0; i<mFrameBuffer.size(); i++)
    mResults->AddFrame(mFrameBuffer.at(i).f);
  mFrameBuffer.clear();
  mResults->CommitResults();
}

void FlexRayAnalyzer::InitAnalyzer(){
  mSampleRateHz = GetSampleRate();
  mSamplesPerBit = mSampleRateHz / mSettings->mBitRate;
  printf("Samples per bit: %d\n", mSamplesPerBit);
  mFlexRay = GetAnalyzerChannelData(mSettings->mInputChannel);
}

bool FlexRayAnalyzer::SampleBit(bool useless){
  // Get bit
  bool result = (mFlexRay->GetBitState() == mSettings->Recessive());

  // Put dot where sampled
  mResults->AddMarker(mFlexRay->GetSampleNumber(), useless ? AnalyzerResults::ErrorX : AnalyzerResults::Dot, mSettings->mInputChannel);

  // Advance to next bit
  mFlexRay->Advance(mSamplesPerBit);
  return result;
}

void FlexRayAnalyzer::WaitForNextFrame() {
  // Go to a recessive bit
  if(mFlexRay->GetBitState() == mSettings->Dominant())
    mFlexRay->AdvanceToNextEdge();

  // Find a TSS and sync off the last edge (e.g. the FSS)
  while(mFlexRay->WouldAdvancingCauseTransition(MIN_IDLE_LEN * mSamplesPerBit)){
    // Skip that bit
    mFlexRay->AdvanceToNextEdge();
    mFlexRay->AdvanceToNextEdge();
    //printf("Advancing to %d\n", mFlexRay->GetSampleNumber());
  }
  mFlexRay->AdvanceToNextEdge();
  mFlexRay->AdvanceToNextEdge();
}

bool FlexRayAnalyzer::NeedsRerun() {
  return false;
}

U32 FlexRayAnalyzer::GenerateSimulationData(U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels) {
  if (mSimulationInitialized == false) {
    mSimulationDataGenerator.Initialize(GetSimulationSampleRate(), mSettings.get());
    mSimulationInitialized = true;
  }

  return mSimulationDataGenerator.GenerateSimulationData(minimum_sample_index, device_sample_rate, simulation_channels);
}

U32 FlexRayAnalyzer::GetMinimumSampleRateHz() {
  return mSettings->mBitRate * 4;
}

const char* FlexRayAnalyzer::GetAnalyzerName() const {
  return "FlexRay";
}

const char* GetAnalyzerName() {
  return "FlexRay";
}

Analyzer* CreateAnalyzer() {
  return new FlexRayAnalyzer();
}

void DestroyAnalyzer(Analyzer* analyzer) {
  delete analyzer;
}