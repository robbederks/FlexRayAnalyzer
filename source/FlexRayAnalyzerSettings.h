#ifndef FLEXRAY_ANALYZER_SETTINGS
#define FLEXRAY_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>
#include <AnalyzerHelpers.h>

class FlexRayAnalyzerSettings : public AnalyzerSettings {
 public:
  FlexRayAnalyzerSettings();
  virtual ~FlexRayAnalyzerSettings();

  virtual bool SetSettingsFromInterfaces();
  void UpdateInterfacesFromSettings();
  virtual void LoadSettings(const char* settings);
  virtual const char* SaveSettings();

  Channel mInputChannel;
  U32 mBitRate;
  bool mInverted;

  BitState Recessive();
  BitState Dominant();

 protected:
  std::auto_ptr<AnalyzerSettingInterfaceChannel> mInputChannelInterface;
  std::auto_ptr<AnalyzerSettingInterfaceInteger> mBitRateInterface;
  std::auto_ptr<AnalyzerSettingInterfaceBool> mInvertedInterface;
};

#endif