#ifndef FLEXRAY_SIMULATION_DATA_GENERATOR
#define FLEXRAY_SIMULATION_DATA_GENERATOR

#include <AnalyzerHelpers.h>
#include <SimulationChannelDescriptor.h>
#include <string>
#include <cstdio>
#include "FlexRayAnalyzerSettings.h"

class FlexRayAnalyzerSettings;
class FlexRaySimulationDataGenerator {
 public:
  FlexRaySimulationDataGenerator();
  ~FlexRaySimulationDataGenerator();

  void Initialize(U32 simulation_sample_rate, FlexRayAnalyzerSettings* settings);
  U32 GenerateSimulationData(U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel);

 protected:
  FlexRayAnalyzerSettings* mSettings;
  U32 mSimulationSampleRateHz;

 protected:
  void CreateSerialByte();

  SimulationChannelDescriptor mSerialSimulationData;
};
#endif