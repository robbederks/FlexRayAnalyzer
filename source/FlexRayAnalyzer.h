#ifndef FLEXRAY_ANALYZER_H
#define FLEXRAY_ANALYZER_H

#include <algorithm>
#include <Analyzer.h>
#include <AnalyzerChannelData.h>

#include "FlexRayAnalyzerResults.h"
#include "FlexRaySimulationDataGenerator.h"
#include "Helpers.h"
#include "FlexRayFrame.h"
#include "FlexRayParameters.h"

class FlexRayAnalyzerResults;
class FlexRayAnalyzerSettings;
class ANALYZER_EXPORT FlexRayAnalyzer : public Analyzer2 {
  public:
    FlexRayAnalyzer();
    virtual ~FlexRayAnalyzer();

    virtual void SetupResults();
    virtual void WorkerThread();

    virtual U32 GenerateSimulationData(U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels);
    virtual U32 GetMinimumSampleRateHz();

    virtual const char* GetAnalyzerName() const;
    virtual bool NeedsRerun();

    void AddResultFrame(FlexRayFrameType type, S64 start_sample, S64 end_sample, U64 data1=0, U64 data2=0);

  protected:  // Variables
    std::auto_ptr<FlexRayAnalyzerSettings> mSettings;
    std::auto_ptr<FlexRayAnalyzerResults> mResults;
    AnalyzerChannelData* mFlexRay;

    FlexRaySimulationDataGenerator mSimulationDataGenerator;
    bool mSimulationInitialized;

  protected: // Analysis functions
    void WaitForNextFrame();
    void InitAnalyzer();
    bool SampleBit(bool useless = false);
    void CommitFrames();

  protected: // Analysis variables
    U32 mSampleRateHz;
    U32 mSamplesPerBit;
    std::vector<FrameSortingWrapper> mFrameBuffer;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer();
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer(Analyzer* analyzer);

#endif  //FLEXRAY_ANALYZER_H
