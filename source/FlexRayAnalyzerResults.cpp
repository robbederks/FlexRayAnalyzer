#include "FlexRayAnalyzerResults.h"

FlexRayAnalyzerResults::FlexRayAnalyzerResults(FlexRayAnalyzer* analyzer, FlexRayAnalyzerSettings* settings) : AnalyzerResults() {
  mSettings = settings;
  mAnalyzer = analyzer;
}

FlexRayAnalyzerResults::~FlexRayAnalyzerResults() {
}

void FlexRayAnalyzerResults::GenerateBubbleText(U64 frame_index, Channel& channel, DisplayBase display_base) {
  ClearResultStrings();
  Frame frame = GetFrame(frame_index);
  char tmp_str[128];

  switch (frame.mType) {
    case TSSField:
      // Short
      AddResultString("TSS");

      // Long
      AddResultString("Transmission Start Sequence");
      break;

    case BSSField:
      // Short
      AddResultString("BSS");

      // Long
      AddResultString("Byte Start Sequence");
      break;

    case FSSField:
      // Short
      AddResultString("FSS");

      // Long
      AddResultString("Frame Start Sequence");
      break;

    case FESField:
      // Short
      AddResultString("FES");

      // Long
      AddResultString("Frame End Sequence");
      break;

    case FlagsField:
      // Short
      AddResultString("Fl");

      // Medium
      AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 5, tmp_str, 128);
      AddResultString("Fl.: ", tmp_str);

      // Long
      AddResultString("Flags: ", tmp_str);

      // Extra long
      sprintf(
          tmp_str,
          "Reserved: %d Payload Preamble: %d Null frame: %d Sync frame: %d Startup frame: %d",
          (frame.mData1 & (1 << 4) != 0),
          (frame.mData1 & (1 << 3) != 0),
          (frame.mData1 & (1 << 2) != 0),
          (frame.mData1 & (1 << 1) != 0),
          (frame.mData1 & (1 << 0) != 0));
      AddResultString(tmp_str);
      break;

    case FrameIdField:
      // Short
      AddResultString("FID");

      // Medium
      AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 11, tmp_str, 128);
      AddResultString("FID.: ", tmp_str);

      // Long
      AddResultString("Frame ID: ", tmp_str);
      break;

    case PayloadLengthField:
      // Short
      AddResultString("PL");

      // Medium
      AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 7, tmp_str, 128);
      AddResultString("P.L.: ", tmp_str);

      // Long
      AddResultString("Payload Length: ", tmp_str);
      break;

    case HeaderCRCField:
      // Short
      AddResultString("H.CRC");

      // Medium
      AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 11, tmp_str, 128);
      AddResultString("H.CRC: ", tmp_str);

      // Long
      AddResultString("Header CRC: ", tmp_str);
      break;

    case CycleCountField:
      // Short
      AddResultString("C.CNT");

      // Medium
      AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 6, tmp_str, 128);
      AddResultString("C.CNT: ", tmp_str);

      // Long
      AddResultString("Cycle Count: ", tmp_str);
      break;

    case DataField:
      // Short
      AddResultString("D");

      // Medium Short
      char index_str[10];
      sprintf(index_str, "%d", frame.mData2);
      AddResultString("D", index_str);

      // Medium
      AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 6, tmp_str, 128);
      AddResultString("D", index_str, ": ", tmp_str);

      // Long
      AddResultString("Data ", index_str, ": ", tmp_str);
      break;

    case CRCField:
      // Short
      AddResultString("CRC");

      // Medium
      AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 24, tmp_str, 128);
      AddResultString("CRC: ", tmp_str);

      // Long
      AddResultString("Frame CRC: ", tmp_str);
      break;

    default:
      break;
  }

  AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8, tmp_str, 128);
  AddResultString(tmp_str);
}

void FlexRayAnalyzerResults::GenerateExportFile(const char* file, DisplayBase display_base, U32 export_type_user_id) {
  std::stringstream ss;
  void* result_file = AnalyzerHelpers::StartFile(file);

  U64 trigger_sample = mAnalyzer->GetTriggerSample();
  U32 sample_rate = mAnalyzer->GetSampleRate();

  // CSV header
  ss << "Time [s],Flags,FrameId,PayloadLength,HeaderCRC,CycleCount,Data,CRC" << std::endl;

  U64 num_frames = GetNumFrames();
  U64 num_packets = GetNumPackets();
  for (U32 i = 0; i < num_packets; i++) {
    U64 first_frame_id;
    U64 last_frame_id;
    GetFramesContainedInPacket(i, &first_frame_id, &last_frame_id);

    // Loop over frames to extract data. Sigh...
    FlexRayFrame f;
    f.data = std::vector<U8>(256, 0);

    U8 flags;
    U8 payload_length;
    for (U64 frame_id = first_frame_id; frame_id <= last_frame_id; frame_id++) {
      Frame frame = GetFrame(frame_id);
      switch (frame.mType) {
        case TSSField:
          break;
        case BSSField:
          break;
        case FSSField:
          break;
        case FESField:
          break;
        case FlagsField:
          // Useless to split this up and combine it again
          flags = frame.mData1;
          break;
        case FrameIdField:
          f.frame_id = frame.mData1;
          break;
        case PayloadLengthField:
          payload_length = frame.mData1;
          break;
        case HeaderCRCField:
          f.header_crc = frame.mData1;
          break;
        case CycleCountField:
          f.cycle_count = frame.mData1;
          break;
        case DataField:
          f.data.at(frame.mData2) = frame.mData1;
          break;
        case CRCField:
          f.crc = frame.mData1;
          break;
        default:
          break;
      }
    }

    char temp_str[128];

    // Time
    AnalyzerHelpers::GetTimeString(GetFrame(first_frame_id).mStartingSampleInclusive, trigger_sample, sample_rate, temp_str, 128);
    ss << temp_str << ',';

    // Flags
    AnalyzerHelpers::GetNumberString(flags, Decimal, 0, temp_str, 128);
    ss << temp_str << ',';

    // FrameId
    AnalyzerHelpers::GetNumberString(f.frame_id, Decimal, 0, temp_str, 128);
    ss << temp_str << ',';

    // PayloadLength
    AnalyzerHelpers::GetNumberString(payload_length, Decimal, 0, temp_str, 128);
    ss << temp_str << ',';

    // HeaderCRC
    AnalyzerHelpers::GetNumberString(f.header_crc, Decimal, 0, temp_str, 128);
    ss << temp_str << ',';

    // CycleCount
    AnalyzerHelpers::GetNumberString(f.cycle_count, Decimal, 0, temp_str, 128);
    ss << temp_str << ',';

    // Data
    for (int j = 0; j < (payload_length * 2); j++) {
      AnalyzerHelpers::GetNumberString(f.data.at(j), Decimal, 0, temp_str, 128);
      ss << temp_str;
      if (j != ((payload_length * 2) - 1))
        ss << '|';
    }
    ss << ",";

    // CRC
    AnalyzerHelpers::GetNumberString(f.crc, Decimal, 0, temp_str, 128);
    ss << temp_str << std::endl;

    // Append string to file
    AnalyzerHelpers::AppendToFile((U8*)ss.str().c_str(), ss.str().length(), result_file);
    ss.str(std::string());

    // Check if we want to cancel
    if (UpdateExportProgressAndCheckForCancel(i, num_packets) == true) {
      AnalyzerHelpers::EndFile(result_file);
      return;
    }
  }

  UpdateExportProgressAndCheckForCancel(num_frames, num_frames);
  AnalyzerHelpers::EndFile(result_file);
}

void FlexRayAnalyzerResults::GenerateFrameTabularText(U64 frame_index, DisplayBase display_base) {
#ifdef SUPPORTS_PROTOCOL_SEARCH
  Frame frame = GetFrame(frame_index);
  ClearTabularText();

  char tmp_str[128];
  AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8, tmp_str, 128);
  AddTabularText(tmp_str);
#endif
}

void FlexRayAnalyzerResults::GeneratePacketTabularText(U64 packet_id, DisplayBase display_base) {
  //not supported
}

void FlexRayAnalyzerResults::GenerateTransactionTabularText(U64 transaction_id, DisplayBase display_base) {
  //not supported
}