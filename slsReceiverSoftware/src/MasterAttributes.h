// SPDX-License-Identifier: LGPL-3.0-or-other
// Copyright (C) 2021 Contributors to the SLS Detector Package
#pragma once

#include "receiver_defs.h"
#include "sls/ToString.h"
#include "sls/logger.h"
#include "sls/sls_detector_defs.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#ifdef HDF5C
#include "H5Cpp.h"
#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif
#endif

#include <chrono>
using ns = std::chrono::nanoseconds;

class MasterAttributes {
  public:
    // (before acquisition)
    slsDetectorDefs::detectorType detType{slsDetectorDefs::GENERIC};
    slsDetectorDefs::timingMode timingMode{slsDetectorDefs::AUTO_TIMING};
    slsDetectorDefs::xy geometry{};
    uint32_t imageSize{0};
    slsDetectorDefs::xy nPixels{};
    uint32_t maxFramesPerFile{0};
    slsDetectorDefs::frameDiscardPolicy frameDiscardMode{
        slsDetectorDefs::NO_DISCARD};
    int framePadding{1};
    slsDetectorDefs::scanParameters scanParams{};
    uint64_t totalFrames{0};
    ns exptime{0};
    ns period{0};
    slsDetectorDefs::burstMode burstMode{slsDetectorDefs::BURST_INTERNAL};
    int numUDPInterfaces{0};
    uint32_t dynamicRange{0};
    uint32_t tenGiga{0};
    int thresholdEnergyeV{0};
    std::array<int, 3> thresholdAllEnergyeV = {{0, 0, 0}};
    ns subExptime{0};
    ns subPeriod{0};
    uint32_t quad{0};
    uint32_t readNRows;
    std::vector<int64_t> ratecorr;
    uint32_t adcmask{0};
    uint32_t analog{0};
    uint32_t analogSamples{0};
    uint32_t digital{0};
    uint32_t digitalSamples{0};
    uint32_t dbitoffset{0};
    uint64_t dbitlist{0};
    slsDetectorDefs::ROI roi{};
    uint32_t counterMask{0};
    std::array<ns, 3> exptimeArray{};
    std::array<ns, 3> gateDelayArray{};
    uint32_t gates;
    std::map<std::string, std::string> additionalJsonHeader;
    uint64_t framesInFile{0};

    MasterAttributes() = default;
    ~MasterAttributes() = default;

    void GetBinaryAttributes(rapidjson::PrettyWriter<rapidjson::StringBuffer> *w);
#ifdef HDF5C
    void WriteHDF5Attributes(H5File *fd, Group *group);
#endif

    void
    GetCommonBinaryAttributes(rapidjson::PrettyWriter<rapidjson::StringBuffer> *w);
    void
    GetFinalBinaryAttributes(rapidjson::PrettyWriter<rapidjson::StringBuffer> *w);
#ifdef HDF5C
    void WriteCommonHDF5Attributes(H5File *fd, Group *group);
    void WriteFinalHDF5Attributes(H5File *fd, Group *group);
    void WriteHDF5Exptime(H5File *fd, Group *group);
    void WriteHDF5Period(H5File *fd, Group *group);
    void WriteHDF5DynamicRange(H5File *fd, Group *group);
    void WriteHDF5TenGiga(H5File *fd, Group *group);
    void WriteHDF5ROI(H5File *fd, Group *group);
    void WriteHDF5NumUDPInterfaces(H5File *fd, Group *group);
    void WriteHDF5ReadNRows(H5File *fd, Group *group);
    void WriteHDF5ThresholdEnergy(H5File *fd, Group *group);
    void WriteHDF5ThresholdEnergies(H5File *fd, Group *group);
    void WriteHDF5SubExpTime(H5File *fd, Group *group);
    void WriteHDF5SubPeriod(H5File *fd, Group *group);
    void WriteHDF5SubQuad(H5File *fd, Group *group);
    void WriteHDF5RateCorrections(H5File *fd, Group *group);
    void WriteHDF5CounterMask(H5File *fd, Group *group);
    void WriteHDF5ExptimeArray(H5File *fd, Group *group);
    void WriteHDF5GateDelayArray(H5File *fd, Group *group);
    void WriteHDF5Gates(H5File *fd, Group *group);
    void WriteHDF5BurstMode(H5File *fd, Group *group);
    void WriteHDF5AdcMask(H5File *fd, Group *group);
    void WriteHDF5AnalogFlag(H5File *fd, Group *group);
    void WriteHDF5AnalogSamples(H5File *fd, Group *group);
    void WriteHDF5DigitalFlag(H5File *fd, Group *group);
    void WriteHDF5DigitalSamples(H5File *fd, Group *group);
    void WriteHDF5DbitOffset(H5File *fd, Group *group);
    void WriteHDF5DbitList(H5File *fd, Group *group);
#endif

    void
    GetGotthardBinaryAttributes(rapidjson::PrettyWriter<rapidjson::StringBuffer> *w);
#ifdef HDF5C
    void WriteGotthardHDF5Attributes(H5File *fd, Group *group);
#endif

    void
    GetJungfrauBinaryAttributes(rapidjson::PrettyWriter<rapidjson::StringBuffer> *w);
#ifdef HDF5C
    void WriteJungfrauHDF5Attributes(H5File *fd, Group *group);
#endif

    void
    GetEigerBinaryAttributes(rapidjson::PrettyWriter<rapidjson::StringBuffer> *w);
#ifdef HDF5C
    void WriteEigerHDF5Attributes(H5File *fd, Group *group);
#endif

    void
    GetMythen3BinaryAttributes(rapidjson::PrettyWriter<rapidjson::StringBuffer> *w);
#ifdef HDF5C
    void WriteMythen3HDF5Attributes(H5File *fd, Group *group);
#endif

    void
    GetGotthard2BinaryAttributes(rapidjson::PrettyWriter<rapidjson::StringBuffer> *w);
#ifdef HDF5C
    void WriteGotthard2HDF5Attributes(H5File *fd, Group *group);
#endif

    void
    GetMoenchBinaryAttributes(rapidjson::PrettyWriter<rapidjson::StringBuffer> *w);
#ifdef HDF5C
    void WriteMoenchHDF5Attributes(H5File *fd, Group *group);
#endif

    void GetCtbBinaryAttributes(rapidjson::PrettyWriter<rapidjson::StringBuffer> *w);
#ifdef HDF5C
    void WriteCtbHDF5Attributes(H5File *fd, Group *group);
#endif
};
