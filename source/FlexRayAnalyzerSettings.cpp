#include "FlexRayAnalyzerSettings.h"

FlexRayAnalyzerSettings::FlexRayAnalyzerSettings() {
  mInputChannel = UNDEFINED_CHANNEL;
  mBitRate = 10000000;
  mInverted = false;

  mInputChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
  mInputChannelInterface->SetTitleAndTooltip("RX", "RX channel on FlexRay transciever");
  mInputChannelInterface->SetChannel(mInputChannel);

  mBitRateInterface.reset(new AnalyzerSettingInterfaceInteger());
  mBitRateInterface->SetTitleAndTooltip("Bit Rate (Bits/S)", "Specify the bit rate in bits per second.");
  mBitRateInterface->SetMax(10000000);
  mBitRateInterface->SetMin(1);
  mBitRateInterface->SetInteger(mBitRate);

  mInvertedInterface.reset(new AnalyzerSettingInterfaceBool());
  mInvertedInterface->SetTitleAndTooltip("", "Invert the captured bits");
  mInvertedInterface->SetCheckBoxText("Inverted");
  mInvertedInterface->SetValue(mInverted);

  AddInterface(mInputChannelInterface.get());
  AddInterface(mBitRateInterface.get());
  AddInterface(mInvertedInterface.get());

  AddExportOption(0, "Export as text/csv file");
  AddExportExtension(0, "text", "txt");
  AddExportExtension(0, "csv", "csv");

  ClearChannels();
  AddChannel(mInputChannel, "RX", false);
}

FlexRayAnalyzerSettings::~FlexRayAnalyzerSettings() {
}

bool FlexRayAnalyzerSettings::SetSettingsFromInterfaces() {
  mInputChannel = mInputChannelInterface->GetChannel();
  mBitRate = mBitRateInterface->GetInteger();
  mInverted = mInvertedInterface->GetValue();

  ClearChannels();
  AddChannel(mInputChannel, "FlexRay", true);

  return true;
}

void FlexRayAnalyzerSettings::UpdateInterfacesFromSettings() {
  mInputChannelInterface->SetChannel(mInputChannel);
  mBitRateInterface->SetInteger(mBitRate);
  mInvertedInterface->SetValue(mInverted);
}

void FlexRayAnalyzerSettings::LoadSettings(const char* settings) {
  SimpleArchive text_archive;
  text_archive.SetString(settings);

  text_archive >> mInputChannel;
  text_archive >> mBitRate;
  text_archive >> mInverted;

  ClearChannels();
  AddChannel(mInputChannel, "FlexRay", true);

  UpdateInterfacesFromSettings();
}

const char* FlexRayAnalyzerSettings::SaveSettings() {
  SimpleArchive text_archive;

  text_archive << mInputChannel;
  text_archive << mBitRate;
  text_archive << mInverted;

  return SetReturnString(text_archive.GetString());
}

BitState FlexRayAnalyzerSettings::Recessive() {
  if (mInverted)
    return BIT_LOW;
  return BIT_HIGH;
}

BitState FlexRayAnalyzerSettings::Dominant() {
  if (mInverted)
    return BIT_HIGH;
  return BIT_LOW;
}
