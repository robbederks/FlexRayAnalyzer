#ifndef FLEXRAY_ANALYZER_RESULTS
#define FLEXRAY_ANALYZER_RESULTS

#include <fstream>
#include <iostream>
#include <sstream>

enum FlexRayFrameType { TSSField,
                        BSSField,
                        FSSField,
                        FESField,
                        FlagsField,
                        FrameIdField,
                        PayloadLengthField,
                        HeaderCRCField,
                        CycleCountField,
                        DataField,
                        CRCField };

#include <AnalyzerResults.h>
#include "FlexRayAnalyzer.h"
#include "FlexRayAnalyzerSettings.h"

class FlexRayAnalyzer;

class FlexRayAnalyzerResults : public AnalyzerResults {
 public:
  FlexRayAnalyzerResults(FlexRayAnalyzer* analyzer, FlexRayAnalyzerSettings* settings);
  virtual ~FlexRayAnalyzerResults();

  virtual void GenerateBubbleText(U64 frame_index, Channel& channel, DisplayBase display_base);
  virtual void GenerateExportFile(const char* file, DisplayBase display_base, U32 export_type_user_id);

  virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base);
  virtual void GeneratePacketTabularText(U64 packet_id, DisplayBase display_base);
  virtual void GenerateTransactionTabularText(U64 transaction_id, DisplayBase display_base);

 protected:  //functions
 protected:  //vars
  FlexRayAnalyzerSettings* mSettings;
  FlexRayAnalyzer* mAnalyzer;
};

#endif
