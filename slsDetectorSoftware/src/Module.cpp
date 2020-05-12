#include "Module.h"
#include "ClientSocket.h"
#include "SharedMemory.h"
#include "ToString.h"
#include "file_utils.h"
#include "network_utils.h"
#include "sls_detector_exceptions.h"
#include "string_utils.h"
#include "versionAPI.h"

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>

namespace sls {

// create shm
Module::Module(detectorType type, int multi_id, int det_id, bool verify)
    : detId(det_id), shm(multi_id, det_id) {

    // ensure shared memory was not created before
    if (shm.IsExisting()) {
        LOG(logWARNING) << "This shared memory should have been "
                           "deleted before! "
                        << shm.GetName() << ". Freeing it again";
        shm.RemoveSharedMemory();
    }

    initSharedMemory(type, multi_id, verify);
}

// pick up from shm
Module::Module(int multi_id, int det_id, bool verify)
    : detId(det_id), shm(multi_id, det_id) {

    // getDetectorType From shm will check if it was already existing
    detectorType type = getDetectorTypeFromShm(multi_id, verify);
    initSharedMemory(type, multi_id, verify);
}

Module::~Module() = default;

bool Module::isFixedPatternSharedMemoryCompatible() {
    return (shm()->shmversion >= SLS_SHMAPIVERSION);
}

void Module::checkDetectorVersionCompatibility() {
    int fnum = F_CHECK_VERSION;
    int64_t arg = 0;

    // get api version number for detector server
    switch (shm()->myDetectorType) {
    case EIGER:
        arg = APIEIGER;
        break;
    case JUNGFRAU:
        arg = APIJUNGFRAU;
        break;
    case GOTTHARD:
        arg = APIGOTTHARD;
        break;
    case CHIPTESTBOARD:
        arg = APICTB;
        break;
    case MOENCH:
        arg = APIMOENCH;
        break;
    case MYTHEN3:
        arg = APIMYTHEN3;
        break;
    case GOTTHARD2:
        arg = APIGOTTHARD2;
        break;
    default:
        throw NotImplementedError(
            "Check version compatibility is not implemented for this detector");
    }
    LOG(logDEBUG1) << "Checking version compatibility with detector with value "
                   << std::hex << arg << std::dec;

    sendToDetector(fnum, arg, nullptr);
    sendToDetectorStop(fnum, arg, nullptr);
}

void Module::checkReceiverVersionCompatibility() {
    // TODO! Verify that this works as intended when version don't match
    int64_t arg = APIRECEIVER;
    LOG(logDEBUG1) << "Checking version compatibility with receiver with value "
                   << std::hex << arg << std::dec;
    sendToReceiver(F_RECEIVER_CHECK_VERSION, arg, nullptr);
}

int64_t Module::getFirmwareVersion() {
    return sendToDetector<int64_t>(F_GET_FIRMWARE_VERSION);
}

int64_t Module::getDetectorServerVersion() {
    return sendToDetector<int64_t>(F_GET_SERVER_VERSION);
}

int64_t Module::getSerialNumber() {
    return sendToDetector<int64_t>(F_GET_SERIAL_NUMBER);
}

int64_t Module::getReceiverSoftwareVersion() const {
    if (shm()->useReceiverFlag) {
        return sendToReceiver<int64_t>(F_GET_RECEIVER_VERSION);
    }
    return -1;
}

void Module::sendToDetector(int fnum, const void *args, size_t args_size,
                            void *retval, size_t retval_size) {
    auto client = DetectorSocket(shm()->hostname, shm()->controlPort);
    client.sendCommandThenRead(fnum, args, args_size, retval, retval_size);
    client.close();
}

template <typename Arg, typename Ret>
void Module::sendToDetector(int fnum, const Arg &args, Ret &retval) {
    sendToDetector(fnum, &args, sizeof(args), &retval, sizeof(retval));
}

template <typename Arg>
void Module::sendToDetector(int fnum, const Arg &args, std::nullptr_t) {
    sendToDetector(fnum, &args, sizeof(args), nullptr, 0);
}

template <typename Ret>
void Module::sendToDetector(int fnum, std::nullptr_t, Ret &retval) {
    sendToDetector(fnum, nullptr, 0, &retval, sizeof(retval));
}

void Module::sendToDetector(int fnum) {
    LOG(logDEBUG1) << "Sending: ["
                   << getFunctionNameFromEnum(
                          static_cast<slsDetectorDefs::detFuncs>(fnum))
                   << "]";
    sendToDetector(fnum, nullptr, 0, nullptr, 0);
}

template <typename Ret> Ret Module::sendToDetector(int fnum) {
    LOG(logDEBUG1) << "Sending: ["
                   << getFunctionNameFromEnum(
                          static_cast<slsDetectorDefs::detFuncs>(fnum))
                   << ", nullptr, 0, " << typeid(Ret).name() << ", "
                   << sizeof(Ret) << "]";
    Ret retval{};
    sendToDetector(fnum, nullptr, 0, &retval, sizeof(retval));
    LOG(logDEBUG1) << "Got back: " << retval;
    return retval;
}

template <typename Ret, typename Arg>
Ret Module::sendToDetector(int fnum, const Arg &args) {
    LOG(logDEBUG1) << "Sending: ["
                   << getFunctionNameFromEnum(
                          static_cast<slsDetectorDefs::detFuncs>(fnum))
                   << ", " << args << ", " << sizeof(args) << ", "
                   << typeid(Ret).name() << ", " << sizeof(Ret) << "]";
    Ret retval{};
    sendToDetector(fnum, &args, sizeof(args), &retval, sizeof(retval));
    LOG(logDEBUG1) << "Got back: " << retval;
    return retval;
}

void Module::sendToDetectorStop(int fnum, const void *args, size_t args_size,
                                void *retval, size_t retval_size) {
    static_cast<const Module &>(*this).sendToDetectorStop(fnum, args, args_size,
                                                          retval, retval_size);
}

void Module::sendToDetectorStop(int fnum, const void *args, size_t args_size,
                                void *retval, size_t retval_size) const {
    auto stop = DetectorSocket(shm()->hostname, shm()->stopPort);
    stop.sendCommandThenRead(fnum, args, args_size, retval, retval_size);
    stop.close();
}

template <typename Arg, typename Ret>
void Module::sendToDetectorStop(int fnum, const Arg &args, Ret &retval) {
    sendToDetectorStop(fnum, &args, sizeof(args), &retval, sizeof(retval));
}

template <typename Arg, typename Ret>
void Module::sendToDetectorStop(int fnum, const Arg &args, Ret &retval) const {
    sendToDetectorStop(fnum, &args, sizeof(args), &retval, sizeof(retval));
}

template <typename Arg>
void Module::sendToDetectorStop(int fnum, const Arg &args, std::nullptr_t) {
    sendToDetectorStop(fnum, &args, sizeof(args), nullptr, 0);
}

template <typename Arg>
void Module::sendToDetectorStop(int fnum, const Arg &args,
                                std::nullptr_t) const {
    sendToDetectorStop(fnum, &args, sizeof(args), nullptr, 0);
}

template <typename Ret>
void Module::sendToDetectorStop(int fnum, std::nullptr_t, Ret &retval) {
    sendToDetectorStop(fnum, nullptr, 0, &retval, sizeof(retval));
}

template <typename Ret>
void Module::sendToDetectorStop(int fnum, std::nullptr_t, Ret &retval) const {
    sendToDetectorStop(fnum, nullptr, 0, &retval, sizeof(retval));
}

void Module::sendToDetectorStop(int fnum) {
    LOG(logDEBUG1) << "Sending to detector stop: ["
                   << getFunctionNameFromEnum(
                          static_cast<slsDetectorDefs::detFuncs>(fnum))
                   << "]";
    sendToDetectorStop(fnum, nullptr, 0, nullptr, 0);
}

void Module::sendToDetectorStop(int fnum) const {
    sendToDetectorStop(fnum, nullptr, 0, nullptr, 0);
}

void Module::sendToReceiver(int fnum, const void *args, size_t args_size,
                            void *retval, size_t retval_size) {
    static_cast<const Module &>(*this).sendToReceiver(fnum, args, args_size,
                                                      retval, retval_size);
}

void Module::sendToReceiver(int fnum, const void *args, size_t args_size,
                            void *retval, size_t retval_size) const {
    if (!shm()->useReceiverFlag) {
        std::ostringstream oss;
        oss << "Set rx_hostname first to use receiver parameters, ";
        oss << getFunctionNameFromEnum(
            static_cast<slsDetectorDefs::detFuncs>(fnum));
        throw RuntimeError(oss.str());
    }
    auto receiver = ReceiverSocket(shm()->rxHostname, shm()->rxTCPPort);
    receiver.sendCommandThenRead(fnum, args, args_size, retval, retval_size);
    receiver.close();
}

template <typename Arg, typename Ret>
void Module::sendToReceiver(int fnum, const Arg &args, Ret &retval) {
    sendToReceiver(fnum, &args, sizeof(args), &retval, sizeof(retval));
}

template <typename Arg, typename Ret>
void Module::sendToReceiver(int fnum, const Arg &args, Ret &retval) const {
    sendToReceiver(fnum, &args, sizeof(args), &retval, sizeof(retval));
}

template <typename Arg>
void Module::sendToReceiver(int fnum, const Arg &args, std::nullptr_t) {
    sendToReceiver(fnum, &args, sizeof(args), nullptr, 0);
}

template <typename Arg>
void Module::sendToReceiver(int fnum, const Arg &args, std::nullptr_t) const {
    sendToReceiver(fnum, &args, sizeof(args), nullptr, 0);
}

template <typename Ret>
void Module::sendToReceiver(int fnum, std::nullptr_t, Ret &retval) {
    sendToReceiver(fnum, nullptr, 0, &retval, sizeof(retval));
}

template <typename Ret>
void Module::sendToReceiver(int fnum, std::nullptr_t, Ret &retval) const {
    sendToReceiver(fnum, nullptr, 0, &retval, sizeof(retval));
}

template <typename Ret> Ret Module::sendToReceiver(int fnum) {
    LOG(logDEBUG1) << "Sending: ["
                   << getFunctionNameFromEnum(
                          static_cast<slsDetectorDefs::detFuncs>(fnum))
                   << ", nullptr, 0, " << typeid(Ret).name() << ", "
                   << sizeof(Ret) << "]";
    Ret retval{};
    sendToReceiver(fnum, nullptr, 0, &retval, sizeof(retval));
    LOG(logDEBUG1) << "Got back: " << retval;
    return retval;
}

template <typename Ret> Ret Module::sendToReceiver(int fnum) const {
    LOG(logDEBUG1) << "Sending: ["
                   << getFunctionNameFromEnum(
                          static_cast<slsDetectorDefs::detFuncs>(fnum))
                   << ", nullptr, 0, " << typeid(Ret).name() << ", "
                   << sizeof(Ret) << "]";
    Ret retval{};
    sendToReceiver(fnum, nullptr, 0, &retval, sizeof(retval));
    LOG(logDEBUG1) << "Got back: " << retval;
    return retval;
}

template <typename Ret, typename Arg>
Ret Module::sendToReceiver(int fnum, const Arg &args) {
    LOG(logDEBUG1) << "Sending: ["
                   << getFunctionNameFromEnum(
                          static_cast<slsDetectorDefs::detFuncs>(fnum))
                   << ", " << args << ", " << sizeof(args) << ", "
                   << typeid(Ret).name() << ", " << sizeof(Ret) << "]";
    Ret retval{};
    sendToReceiver(fnum, &args, sizeof(args), &retval, sizeof(retval));
    LOG(logDEBUG1) << "Got back: " << retval;
    return retval;
}

template <typename Ret, typename Arg>
Ret Module::sendToReceiver(int fnum, const Arg &args) const {
    LOG(logDEBUG1) << "Sending: ["
                   << getFunctionNameFromEnum(
                          static_cast<slsDetectorDefs::detFuncs>(fnum))
                   << ", " << args << ", " << sizeof(args) << ", "
                   << typeid(Ret).name() << ", " << sizeof(Ret) << "]";
    Ret retval{};
    sendToReceiver(fnum, &args, sizeof(args), &retval, sizeof(retval));
    LOG(logDEBUG1) << "Got back: " << retval;
    return retval;
}

// void Module::sendToReceiver(int fnum) {
//     sendToReceiver(fnum, nullptr, 0, nullptr, 0);
// }

// void Module::sendToReceiver(int fnum) const {
//     sendToReceiver(fnum, nullptr, 0, nullptr, 0);
// }

void Module::freeSharedMemory() {
    if (shm.IsExisting()) {
        shm.RemoveSharedMemory();
    }
}

void Module::setHostname(const std::string &hostname,
                         const bool initialChecks) {
    sls::strcpy_safe(shm()->hostname, hostname.c_str());
    auto client = DetectorSocket(shm()->hostname, shm()->controlPort);
    client.close();
    try {
        checkDetectorVersionCompatibility();
        LOG(logINFO) << "Detector Version Compatibility - Success";
    } catch (const DetectorError &e) {
        if (!initialChecks) {
            LOG(logWARNING) << "Bypassing Initial Checks at your own risk!";
        } else {
            throw;
        }
    }
    if (shm()->myDetectorType == EIGER) {
        setActivate(true);
    }
}

std::string Module::getHostname() const { return shm()->hostname; }

void Module::initSharedMemory(detectorType type, int multi_id, bool verify) {
    shm = SharedMemory<sharedSlsDetector>(multi_id, detId);
    if (!shm.IsExisting()) {
        shm.CreateSharedMemory();
        initializeDetectorStructure(type);
    } else {
        shm.OpenSharedMemory();
        if (verify && shm()->shmversion != SLS_SHMVERSION) {
            std::ostringstream ss;
            ss << "Single shared memory (" << multi_id << "-" << detId
               << ":) version mismatch (expected 0x" << std::hex
               << SLS_SHMVERSION << " but got 0x" << shm()->shmversion << ")"
               << std::dec << ". Clear Shared memory to continue.";
            throw SharedMemoryError(ss.str());
        }
    }
}

void Module::initializeDetectorStructure(detectorType type) {
    shm()->shmversion = SLS_SHMVERSION;
    memset(shm()->hostname, 0, MAX_STR_LENGTH);
    shm()->myDetectorType = type;
    shm()->multiSize.x = 0;
    shm()->multiSize.y = 0;
    shm()->controlPort = DEFAULT_PORTNO;
    shm()->stopPort = DEFAULT_PORTNO + 1;
    sls::strcpy_safe(shm()->settingsDir, getenv("HOME"));
    sls::strcpy_safe(shm()->rxHostname, "none");
    shm()->rxTCPPort = DEFAULT_PORTNO + 2;
    shm()->useReceiverFlag = false;
    shm()->zmqport = DEFAULT_ZMQ_CL_PORTNO +
                     (detId * ((shm()->myDetectorType == EIGER) ? 2 : 1));
    shm()->zmqip = IpAddr{};
    shm()->numUDPInterfaces = 1;
    shm()->stoppedFlag = false;

    // get the detector parameters based on type
    detParameters parameters{type};
    shm()->nChan.x = parameters.nChanX;
    shm()->nChan.y = parameters.nChanY;
    shm()->nChip.x = parameters.nChipX;
    shm()->nChip.y = parameters.nChipY;
    shm()->nDacs = parameters.nDacs;
}

int Module::sendModule(sls_detector_module *myMod, sls::ClientSocket &client) {
    TLogLevel level = logDEBUG1;
    LOG(level) << "Sending Module";
    int ts = 0;
    int n = 0;
    n = client.Send(&(myMod->serialnumber), sizeof(myMod->serialnumber));
    ts += n;
    LOG(level) << "Serial number sent. " << n
               << " bytes. serialno: " << myMod->serialnumber;

    n = client.Send(&(myMod->nchan), sizeof(myMod->nchan));
    ts += n;
    LOG(level) << "nchan sent. " << n << " bytes. nchan: " << myMod->nchan;

    n = client.Send(&(myMod->nchip), sizeof(myMod->nchip));
    ts += n;
    LOG(level) << "nchip sent. " << n << " bytes. nchip: " << myMod->nchip;

    n = client.Send(&(myMod->ndac), sizeof(myMod->ndac));
    ts += n;
    LOG(level) << "ndac sent. " << n << " bytes. ndac: " << myMod->ndac;

    n = client.Send(&(myMod->reg), sizeof(myMod->reg));
    ts += n;
    LOG(level) << "reg sent. " << n << " bytes. reg: " << myMod->reg;

    n = client.Send(&(myMod->iodelay), sizeof(myMod->iodelay));
    ts += n;
    LOG(level) << "iodelay sent. " << n
               << " bytes. iodelay: " << myMod->iodelay;

    n = client.Send(&(myMod->tau), sizeof(myMod->tau));
    ts += n;
    LOG(level) << "tau sent. " << n << " bytes. tau: " << myMod->tau;

    n = client.Send(&(myMod->eV), sizeof(myMod->eV));
    ts += n;
    LOG(level) << "ev sent. " << n << " bytes. ev: " << myMod->eV;

    n = client.Send(myMod->dacs, sizeof(int) * (myMod->ndac));
    ts += n;
    LOG(level) << "dacs sent. " << n << " bytes";

    if (shm()->myDetectorType == EIGER || shm()->myDetectorType == MYTHEN3) {
        n = client.Send(myMod->chanregs, sizeof(int) * (myMod->nchan));
        ts += n;
        LOG(level) << "channels sent. " << n << " bytes";
    }
    return ts;
}

int Module::receiveModule(sls_detector_module *myMod,
                          sls::ClientSocket &client) {
    int ts = 0;
    ts += client.Receive(&(myMod->serialnumber), sizeof(myMod->serialnumber));
    ts += client.Receive(&(myMod->nchan), sizeof(myMod->nchan));
    ts += client.Receive(&(myMod->nchip), sizeof(myMod->nchip));
    ts += client.Receive(&(myMod->ndac), sizeof(myMod->ndac));
    ts += client.Receive(&(myMod->reg), sizeof(myMod->reg));
    ts += client.Receive(&(myMod->iodelay), sizeof(myMod->iodelay));
    ts += client.Receive(&(myMod->tau), sizeof(myMod->tau));
    ts += client.Receive(&(myMod->eV), sizeof(myMod->eV));

    ts += client.Receive(myMod->dacs, sizeof(int) * (myMod->ndac));
    LOG(logDEBUG1) << "received dacs of size " << ts;
    if (shm()->myDetectorType == EIGER || shm()->myDetectorType == MYTHEN3) {
        ts += client.Receive(myMod->chanregs, sizeof(int) * (myMod->nchan));
        LOG(logDEBUG1) << " nchan= " << myMod->nchan
                       << " nchip= " << myMod->nchip
                       << "received chans of size " << ts;
    }
    LOG(logDEBUG1) << "received module of size " << ts << " register "
                   << myMod->reg;
    return ts;
}

slsDetectorDefs::detectorType Module::getDetectorTypeFromShm(int multi_id,
                                                             bool verify) {
    if (!shm.IsExisting()) {
        throw SharedMemoryError("Shared memory " + shm.GetName() +
                                "does not exist.\n Corrupted Multi Shared "
                                "memory. Please free shared memory.");
    }

    shm.OpenSharedMemory();
    if (verify && shm()->shmversion != SLS_SHMVERSION) {
        std::ostringstream ss;
        ss << "Single shared memory (" << multi_id << "-" << detId
           << ":)version mismatch (expected 0x" << std::hex << SLS_SHMVERSION
           << " but got 0x" << shm()->shmversion << ")" << std::dec
           << ". Clear Shared memory to continue.";
        shm.UnmapSharedMemory();
        throw SharedMemoryError(ss.str());
    }
    auto type = shm()->myDetectorType;
    return type;
}

// static function
slsDetectorDefs::detectorType
Module::getTypeFromDetector(const std::string &hostname, int cport) {
    int fnum = F_GET_DETECTOR_TYPE;
    int ret = FAIL;
    detectorType retval = GENERIC;
    LOG(logDEBUG1) << "Getting detector type ";
    sls::ClientSocket cs("Detector", hostname, cport);
    cs.Send(reinterpret_cast<char *>(&fnum), sizeof(fnum));
    cs.Receive(reinterpret_cast<char *>(&ret), sizeof(ret));
    cs.Receive(reinterpret_cast<char *>(&retval), sizeof(retval));
    LOG(logDEBUG1) << "Detector type is " << retval;
    return retval;
}

slsDetectorDefs::detectorType Module::getDetectorType() const {
    return shm()->myDetectorType;
}

void Module::updateNumberOfChannels() {
    if (shm()->myDetectorType == CHIPTESTBOARD ||
        shm()->myDetectorType == MOENCH) {
        LOG(logDEBUG1) << "Updating number of channels";
        std::array<int, 2> retvals{};
        sendToDetector(F_GET_NUM_CHANNELS, nullptr, retvals);
        LOG(logDEBUG1) << "Number of channels retval: [" << retvals[0] << ", "
                       << retvals[1] << ']';
        shm()->nChan.x = retvals[0];
        shm()->nChan.y = retvals[1];
    }
}

slsDetectorDefs::xy Module::getNumberOfChannels() const {
    slsDetectorDefs::xy coord{};
    coord.x = (shm()->nChan.x * shm()->nChip.x);
    coord.y = (shm()->nChan.y * shm()->nChip.y);
    return coord;
}

bool Module::getQuad() { return sendToDetector<int>(F_GET_QUAD) != 0; }

void Module::setQuad(const bool enable) {
    int value = enable ? 1 : 0;
    LOG(logDEBUG1) << "Setting Quad type to " << value;
    sendToDetector(F_SET_QUAD, value, nullptr);
    LOG(logDEBUG1) << "Setting Quad type to " << value << " in Receiver";
    if (shm()->useReceiverFlag) {
        sendToReceiver(F_SET_RECEIVER_QUAD, value, nullptr);
    }
}

void Module::setReadNLines(const int value) {
    LOG(logDEBUG1) << "Setting read n lines to " << value;
    sendToDetector(F_SET_READ_N_LINES, value, nullptr);
    LOG(logDEBUG1) << "Setting read n lines to " << value << " in Receiver";
    if (shm()->useReceiverFlag) {
        sendToReceiver(F_SET_RECEIVER_READ_N_LINES, value, nullptr);
    }
}

int Module::getReadNLines() { return sendToDetector<int>(F_GET_READ_N_LINES); }

void Module::updateMultiSize(slsDetectorDefs::xy det) {
    shm()->multiSize = det;
    int args[2] = {shm()->multiSize.y, detId};
    sendToDetector(F_SET_POSITION, args, nullptr);
}

int Module::setControlPort(int port_number) {
    int retval = -1;
    LOG(logDEBUG1) << "Setting control port to " << port_number;
    if (port_number >= 0 && port_number != shm()->controlPort) {
        if (strlen(shm()->hostname) > 0) {
            sendToDetector(F_SET_PORT, port_number, retval);
            shm()->controlPort = retval;
            LOG(logDEBUG1) << "Control port: " << retval;
        } else {
            shm()->controlPort = port_number;
        }
    }
    return shm()->controlPort;
}

int Module::setStopPort(int port_number) {
    int retval = -1;
    LOG(logDEBUG1) << "Setting stop port to " << port_number;
    if (port_number >= 0 && port_number != shm()->stopPort) {
        if (strlen(shm()->hostname) > 0) {
            sendToDetectorStop(F_SET_PORT, port_number, retval);
            shm()->stopPort = retval;
            LOG(logDEBUG1) << "Stop port: " << retval;
        } else {
            shm()->stopPort = port_number;
        }
    }
    return shm()->stopPort;
}

int Module::setReceiverPort(int port_number) {
    LOG(logDEBUG1) << "Setting reciever port to " << port_number;
    if (port_number >= 0 && port_number != shm()->rxTCPPort) {
        if (shm()->useReceiverFlag) {
            int retval = -1;
            sendToReceiver(F_SET_RECEIVER_PORT, port_number, retval);
            shm()->rxTCPPort = retval;
            LOG(logDEBUG1) << "Receiver port: " << retval;

        } else {
            shm()->rxTCPPort = port_number;
        }
    }
    return shm()->rxTCPPort;
}

int Module::getReceiverPort() const { return shm()->rxTCPPort; }

int Module::getControlPort() const { return shm()->controlPort; }

int Module::getStopPort() const { return shm()->stopPort; }

bool Module::lockServer(int lock) {
    return sendToDetector<int>(F_LOCK_SERVER, lock) != 0;
}

sls::IpAddr Module::getLastClientIP() {
    return sendToDetector<sls::IpAddr>(F_GET_LAST_CLIENT_IP);
}

void Module::exitServer() { sendToDetector(F_EXIT_SERVER); }

void Module::execCommand(const std::string &cmd) {
    char arg[MAX_STR_LENGTH]{};
    char retval[MAX_STR_LENGTH]{};
    sls::strcpy_safe(arg, cmd.c_str());
    LOG(logDEBUG1) << "Sending command to detector " << arg;
    sendToDetector(F_EXEC_COMMAND, arg, retval);
    if (strlen(retval) != 0U) {
        LOG(logINFO) << "Detector " << detId << " returned:\n" << retval;
    }
}

std::vector<std::string> Module::getConfigFileCommands() {
    std::vector<std::string> base{"hostname",    "port",       "stopport",
                                  "settingsdir", "fpath",      "lock",
                                  "zmqport",     "rx_zmqport", "zmqip",
                                  "rx_zmqip",    "rx_tcpport"};

    switch (shm()->myDetectorType) {
    case GOTTHARD:
        base.emplace_back("detectormac");
        base.emplace_back("detectorip");
        base.emplace_back("rx_udpport");
        base.emplace_back("rx_udpip");
        base.emplace_back("rx_udpmac");
        base.emplace_back("extsig");
        break;
    case EIGER:
        base.emplace_back("detectormac");
        base.emplace_back("detectorip");
        base.emplace_back("rx_udpport");
        base.emplace_back("rx_udpport2");
        base.emplace_back("rx_udpip");
        base.emplace_back("rx_udpmac");
        base.emplace_back("trimen");
        base.emplace_back("iodelay");
        base.emplace_back("tengiga");
        break;
    case JUNGFRAU:
        base.emplace_back("detectormac");
        base.emplace_back("detectormac2");
        base.emplace_back("detectorip");
        base.emplace_back("detectorip2");
        base.emplace_back("rx_udpport");
        base.emplace_back("rx_udpport2");
        base.emplace_back("rx_udpip");
        base.emplace_back("rx_udpip2");
        base.emplace_back("rx_udpmac");
        base.emplace_back("rx_udpmac2");
        base.emplace_back("powerchip");
        break;
    case CHIPTESTBOARD:
        base.emplace_back("detectormac");
        base.emplace_back("detectorip");
        base.emplace_back("rx_udpport");
        base.emplace_back("rx_udpip");
        base.emplace_back("rx_udpmac");
        break;
    case MOENCH:
        base.emplace_back("detectormac");
        base.emplace_back("detectorip");
        base.emplace_back("rx_udpport");
        base.emplace_back("rx_udpip");
        base.emplace_back("rx_udpmac");
        break;
    default:
        throw RuntimeError(
            "Write configuration file called with unknown detector: " +
            std::to_string(shm()->myDetectorType));
    }

    base.emplace_back("vhighvoltage");
    base.emplace_back("rx_hostname");
    base.emplace_back("r_readfreq");
    base.emplace_back("rx_udpsocksize");
    base.emplace_back("rx_realudpsocksize");

    std::vector<std::string> commands;
    for (const auto &cmd : base) {
        std::ostringstream os;
        os << detId << ':' << cmd;
        commands.emplace_back(os.str());
    }
    return commands;
}

slsDetectorDefs::detectorSettings Module::getSettings() {
    auto r = sendToDetector<int>(F_SET_SETTINGS, -1);
    return static_cast<detectorSettings>(r);
}

void Module::setSettings(detectorSettings isettings) {
    if (shm()->myDetectorType == EIGER) {
        throw RuntimeError(
            "Cannot set settings for Eiger. Use threshold energy.");
    }
    int arg = static_cast<int>(isettings);
    int retval = -1;
    LOG(logDEBUG1) << "Setting settings to " << arg;
    sendToDetector(F_SET_SETTINGS, arg, retval);
}

int Module::getThresholdEnergy() {
    // moench - get threshold energy from processor (due to different clients,
    // diff shm)
    if (shm()->myDetectorType == MOENCH) {
        // get json from rxr, parse for threshold and update shm
        getAdditionalJsonHeader();
        std::string result = getAdditionalJsonParameter("threshold");
        // convert to integer
        try {
            return std::stoi(result);
        }
        // not found or cannot scan integer
        catch (...) {
            return -1;
        }
    }
    return sendToDetector<int>(F_GET_THRESHOLD_ENERGY);
}

void Module::setThresholdEnergy(int e_eV, detectorSettings isettings, int tb) {
    // check as there is client processing
    if (shm()->myDetectorType == EIGER) {
        setThresholdEnergyAndSettings(e_eV, isettings, tb);
    }

    // moench - send threshold energy to processor
    else if (shm()->myDetectorType == MOENCH) {
        setAdditionalJsonParameter("threshold", std::to_string(e_eV));
    }

    else {
        throw RuntimeError(
            "Set threshold energy not implemented for this detector");
    }
}

void Module::setThresholdEnergyAndSettings(int e_eV, detectorSettings isettings,
                                           int tb) {

    // if settings provided, use that, else use the shared memory variable
    detectorSettings is =
        ((isettings != GET_SETTINGS) ? isettings : getSettings());

    // verify e_eV exists in trimEneregies[]
    if (shm()->trimEnergies.empty() || (e_eV < shm()->trimEnergies.front()) ||
        (e_eV > shm()->trimEnergies.back())) {
        throw RuntimeError("This energy " + std::to_string(e_eV) +
                           " not defined for this module!");
    }

    bool interpolate =
        std::all_of(shm()->trimEnergies.begin(), shm()->trimEnergies.end(),
                    [e_eV](const int &e) { return e != e_eV; });

    sls_detector_module myMod{shm()->myDetectorType};

    if (!interpolate) {
        std::string settingsfname = getTrimbitFilename(is, e_eV);
        LOG(logDEBUG1) << "Settings File is " << settingsfname;
        myMod = readSettingsFile(settingsfname, tb);
    } else {
        // find the trim values
        int trim1 = -1, trim2 = -1;
        for (size_t i = 0; i < shm()->trimEnergies.size(); ++i) {
            if (e_eV < shm()->trimEnergies[i]) {
                trim2 = shm()->trimEnergies[i];
                trim1 = shm()->trimEnergies[i - 1];
                break;
            }
        }
        std::string settingsfname1 = getTrimbitFilename(is, trim1);
        std::string settingsfname2 = getTrimbitFilename(is, trim2);
        LOG(logDEBUG1) << "Settings Files are " << settingsfname1 << " and "
                       << settingsfname2;
        auto myMod1 = readSettingsFile(settingsfname1, tb);
        auto myMod2 = readSettingsFile(settingsfname2, tb);
        if (myMod1.iodelay != myMod2.iodelay) {
            throw RuntimeError("setThresholdEnergyAndSettings: Iodelays do not "
                               "match between files");
        }
        myMod = interpolateTrim(&myMod1, &myMod2, e_eV, trim1, trim2, tb);
        myMod.iodelay = myMod1.iodelay;
        myMod.tau =
            linearInterpolation(e_eV, trim1, trim2, myMod1.tau, myMod2.tau);
    }

    myMod.reg = is;
    myMod.eV = e_eV;
    setModule(myMod, tb);
    if (getSettings() != is) {
        throw RuntimeError("setThresholdEnergyAndSettings: Could not set "
                           "settings in detector");
    }
}

std::string Module::getTrimbitFilename(detectorSettings s, int e_eV) {
    std::string ssettings;
    switch (s) {
    case STANDARD:
        ssettings = "/standard";
        break;
    case HIGHGAIN:
        ssettings = "/highgain";
        break;
    case LOWGAIN:
        ssettings = "/lowgain";
        break;
    case VERYHIGHGAIN:
        ssettings = "/veryhighgain";
        break;
    case VERYLOWGAIN:
        ssettings = "/verylowgain";
        break;
    default:
        std::ostringstream ss;
        ss << "Unknown settings " << ToString(s) << " for this detector!";
        throw RuntimeError(ss.str());
    }
    std::ostringstream ostfn;
    ostfn << shm()->settingsDir << ssettings << "/" << e_eV << "eV"
          << "/noise.sn" << std::setfill('0') << std::setw(3) << std::dec
          << getSerialNumber() << std::setbase(10);
    return ostfn.str();
}

std::string Module::getSettingsDir() { return std::string(shm()->settingsDir); }

std::string Module::setSettingsDir(const std::string &dir) {
    sls::strcpy_safe(shm()->settingsDir, dir.c_str());
    return shm()->settingsDir;
}

void Module::loadSettingsFile(const std::string &fname) {
    std::string fn = fname;
    std::ostringstream ostfn;
    ostfn << fname;

    // find specific file if it has detid in file name (.snxxx)
    if (shm()->myDetectorType == EIGER || shm()->myDetectorType == MYTHEN3) {
        if (fname.find(".sn") == std::string::npos &&
            fname.find(".trim") == std::string::npos &&
            fname.find(".settings") == std::string::npos) {
            ostfn << ".sn" << std::setfill('0') << std::setw(3) << std::dec
                  << getSerialNumber();
        }
    }
    fn = ostfn.str();
    auto myMod = readSettingsFile(fn);
    setModule(myMod);
}

void Module::saveSettingsFile(const std::string &fname) {
    std::string fn = fname;
    std::ostringstream ostfn;
    ostfn << fname;

    // find specific file if it has detid in file name (.snxxx)
    if (shm()->myDetectorType == EIGER) {
        ostfn << ".sn" << std::setfill('0') << std::setw(3) << std::dec
              << getSerialNumber();
    }
    fn = ostfn.str();
    sls_detector_module myMod = getModule();
    writeSettingsFile(fn, myMod);
}

slsDetectorDefs::runStatus Module::getRunStatus() const {
    runStatus retval = ERROR;
    LOG(logDEBUG1) << "Getting status";
    sendToDetectorStop(F_GET_RUN_STATUS, nullptr, retval);
    LOG(logDEBUG1) << "Detector status: " << ToString(retval);
    return retval;
}

void Module::prepareAcquisition() { sendToDetector(F_PREPARE_ACQUISITION); }

void Module::startAcquisition() {
    shm()->stoppedFlag = false;
    sendToDetector(F_START_ACQUISITION);
}

void Module::stopAcquisition() {
    // get status before stopping acquisition
    runStatus s = ERROR, r = ERROR;
    bool zmqstreaming = false;
    if (shm()->useReceiverFlag && getReceiverStreaming()) {
        zmqstreaming = true;
        s = getRunStatus();
        r = getReceiverStatus();
    }
    LOG(logDEBUG1) << "Stopping Acquisition";
    sendToDetectorStop(F_STOP_ACQUISITION);
    shm()->stoppedFlag = true;
    LOG(logDEBUG1) << "Stopping Acquisition successful";
    // if rxr streaming and acquisition finished, restream dummy stop packet
    if (zmqstreaming && (s == IDLE) && (r == IDLE)) {
        restreamStopFromReceiver();
    }
}

void Module::sendSoftwareTrigger() { sendToDetectorStop(F_SOFTWARE_TRIGGER); }

void Module::startAndReadAll() {
    shm()->stoppedFlag = false;
    sendToDetector(F_START_AND_READ_ALL);
}

void Module::startReadOut() { sendToDetector(F_START_READOUT); }

void Module::readAll() { sendToDetector(F_READ_ALL); }

void Module::setStartingFrameNumber(uint64_t value) {
    LOG(logDEBUG1) << "Setting starting frame number to " << value;
    sendToDetector(F_SET_STARTING_FRAME_NUMBER, value, nullptr);
}

uint64_t Module::getStartingFrameNumber() {
    return sendToDetector<uint64_t>(F_GET_STARTING_FRAME_NUMBER);
}

int64_t Module::getNumberOfFrames() {
    return sendToDetector<int64_t>(F_GET_NUM_FRAMES);
}

void Module::setNumberOfFrames(int64_t value) {
    LOG(logDEBUG1) << "Setting number of frames to " << value;
    sendToDetector(F_SET_NUM_FRAMES, value, nullptr);
    if (shm()->useReceiverFlag) {
        LOG(logDEBUG1) << "Sending number of frames to Receiver: " << value;
        sendToReceiver(F_RECEIVER_SET_NUM_FRAMES, value, nullptr);
    }
}

int64_t Module::getNumberOfTriggers() {
    return sendToDetector<int64_t>(F_GET_NUM_TRIGGERS);
}

void Module::setNumberOfTriggers(int64_t value) {
    LOG(logDEBUG1) << "Setting number of triggers to " << value;
    sendToDetector(F_SET_NUM_TRIGGERS, value, nullptr);
    if (shm()->useReceiverFlag) {
        LOG(logDEBUG1) << "Sending number of triggers to Receiver: " << value;
        sendToReceiver(F_SET_RECEIVER_NUM_TRIGGERS, value, nullptr);
    }
}

int64_t Module::getNumberOfBursts() {
    return sendToDetector<int64_t>(F_GET_NUM_BURSTS);
}

void Module::setNumberOfBursts(int64_t value) {
    LOG(logDEBUG1) << "Setting number of bursts to " << value;
    sendToDetector(F_SET_NUM_BURSTS, value, nullptr);
    if (shm()->useReceiverFlag) {
        LOG(logDEBUG1) << "Sending number of bursts to Receiver: " << value;
        sendToReceiver(F_SET_RECEIVER_NUM_BURSTS, value, nullptr);
    }
}

int Module::getNumberOfAdditionalStorageCells() {
    return sendToDetector<int>(F_GET_NUM_ADDITIONAL_STORAGE_CELLS);
}

void Module::setNumberOfAdditionalStorageCells(int value) {
    LOG(logDEBUG1) << "Setting number of storage cells to " << value;
    sendToDetector(F_SET_NUM_ADDITIONAL_STORAGE_CELLS, value, nullptr);
}

int Module::getNumberOfAnalogSamples() {
    return sendToDetector<int>(F_GET_NUM_ANALOG_SAMPLES);
}

void Module::setNumberOfAnalogSamples(int value) {
    LOG(logDEBUG1) << "Setting number of analog samples to " << value;
    sendToDetector(F_SET_NUM_ANALOG_SAMPLES, value, nullptr);
    // update #nchan, as it depends on #samples, adcmask
    updateNumberOfChannels();
    if (shm()->useReceiverFlag) {
        LOG(logDEBUG1) << "Sending number of analog samples to Receiver: "
                       << value;
        sendToReceiver(F_RECEIVER_SET_NUM_ANALOG_SAMPLES, value, nullptr);
    }
}

int Module::getNumberOfDigitalSamples() {
    return sendToDetector<int>(F_GET_NUM_DIGITAL_SAMPLES);
}

void Module::setNumberOfDigitalSamples(int value) {
    LOG(logDEBUG1) << "Setting number of digital samples to " << value;
    sendToDetector(F_SET_NUM_DIGITAL_SAMPLES, value, nullptr);
    // update #nchan, as it depends on #samples, adcmask
    updateNumberOfChannels();
    if (shm()->useReceiverFlag) {
        LOG(logDEBUG1) << "Sending number of digital samples to Receiver: "
                       << value;
        sendToReceiver(F_RECEIVER_SET_NUM_DIGITAL_SAMPLES, value, nullptr);
    }
}

int64_t Module::getExptime() { return sendToDetector<int64_t>(F_GET_EXPTIME); }

void Module::setExptime(int64_t value) {
    int64_t prevVal = value;
    if (shm()->myDetectorType == EIGER) {
        prevVal = getExptime();
    }
    LOG(logDEBUG1) << "Setting exptime to " << value << "ns";
    sendToDetector(F_SET_EXPTIME, value, nullptr);
    if (shm()->useReceiverFlag) {
        LOG(logDEBUG1) << "Sending exptime to Receiver: " << value;
        sendToReceiver(F_RECEIVER_SET_EXPTIME, value, nullptr);
    }
    if (prevVal != value) {
        updateRateCorrection();
    }
}

int64_t Module::getPeriod() { return sendToDetector<int64_t>(F_GET_PERIOD); }

void Module::setPeriod(int64_t value) {
    LOG(logDEBUG1) << "Setting period to " << value << "ns";
    sendToDetector(F_SET_PERIOD, value, nullptr);
    if (shm()->useReceiverFlag) {
        LOG(logDEBUG1) << "Sending period to Receiver: " << value;
        sendToReceiver(F_RECEIVER_SET_PERIOD, value, nullptr);
    }
}

int64_t Module::getDelayAfterTrigger() {
    return sendToDetector<int64_t>(F_GET_DELAY_AFTER_TRIGGER);
}

void Module::setDelayAfterTrigger(int64_t value) {
    LOG(logDEBUG1) << "Setting delay after trigger to " << value << "ns";
    sendToDetector(F_SET_DELAY_AFTER_TRIGGER, value, nullptr);
}

int64_t Module::getBurstPeriod() {
    return sendToDetector<int64_t>(F_GET_BURST_PERIOD);
}

void Module::setBurstPeriod(int64_t value) {
    LOG(logDEBUG1) << "Setting burst period to " << value << "ns";
    sendToDetector(F_SET_BURST_PERIOD, value, nullptr);
}

int64_t Module::getSubExptime() {
    return sendToDetector<int64_t>(F_GET_SUB_EXPTIME);
}

void Module::setSubExptime(int64_t value) {
    int64_t prevVal = value;
    if (shm()->myDetectorType == EIGER) {
        prevVal = getSubExptime();
    }
    LOG(logDEBUG1) << "Setting sub exptime to " << value << "ns";
    sendToDetector(F_SET_SUB_EXPTIME, value, nullptr);
    if (shm()->useReceiverFlag) {
        LOG(logDEBUG1) << "Sending sub exptime to Receiver: " << value;
        sendToReceiver(F_RECEIVER_SET_SUB_EXPTIME, value, nullptr);
    }
    if (prevVal != value) {
        updateRateCorrection();
    }
}

int64_t Module::getSubDeadTime() {
    return sendToDetector<int64_t>(F_GET_SUB_DEADTIME);
}

void Module::setSubDeadTime(int64_t value) {
    LOG(logDEBUG1) << "Setting sub deadtime to " << value << "ns";
    sendToDetector(F_SET_SUB_DEADTIME, value, nullptr);
    if (shm()->useReceiverFlag) {
        LOG(logDEBUG1) << "Sending sub deadtime to Receiver: " << value;
        sendToReceiver(F_RECEIVER_SET_SUB_DEADTIME, value, nullptr);
    }
}

int64_t Module::getStorageCellDelay() {
    return sendToDetector<int64_t>(F_GET_STORAGE_CELL_DELAY);
}

void Module::setStorageCellDelay(int64_t value) {
    LOG(logDEBUG1) << "Setting storage cell delay to " << value << "ns";
    sendToDetector(F_SET_STORAGE_CELL_DELAY, value, nullptr);
}

int64_t Module::getNumberOfFramesLeft() const {
    int64_t retval = -1;
    sendToDetectorStop(F_GET_FRAMES_LEFT, nullptr, retval);
    LOG(logDEBUG1) << "number of frames left :" << retval;
    return retval;
}

int64_t Module::getNumberOfTriggersLeft() const {
    int64_t retval = -1;
    sendToDetectorStop(F_GET_TRIGGERS_LEFT, nullptr, retval);
    LOG(logDEBUG1) << "number of triggers left :" << retval;
    return retval;
}

int64_t Module::getDelayAfterTriggerLeft() const {
    int64_t retval = -1;
    sendToDetectorStop(F_GET_DELAY_AFTER_TRIGGER_LEFT, nullptr, retval);
    LOG(logDEBUG1) << "delay after trigger left :" << retval << "ns";
    return retval;
}

int64_t Module::getExptimeLeft() const {
    int64_t retval = -1;
    sendToDetectorStop(F_GET_EXPTIME_LEFT, nullptr, retval);
    LOG(logDEBUG1) << "exptime left :" << retval << "ns";
    return retval;
}

int64_t Module::getPeriodLeft() const {
    int64_t retval = -1;
    sendToDetectorStop(F_GET_PERIOD_LEFT, nullptr, retval);
    LOG(logDEBUG1) << "period left :" << retval << "ns";
    return retval;
}

int64_t Module::getMeasuredPeriod() const {
    int64_t retval = -1;
    sendToDetectorStop(F_GET_MEASURED_PERIOD, nullptr, retval);
    LOG(logDEBUG1) << "measured period :" << retval << "ns";
    return retval;
}

int64_t Module::getMeasuredSubFramePeriod() const {
    int64_t retval = -1;
    sendToDetectorStop(F_GET_MEASURED_SUBPERIOD, nullptr, retval);
    LOG(logDEBUG1) << "exptime :" << retval << "ns";
    return retval;
}

int64_t Module::getNumberOfFramesFromStart() const {
    int64_t retval = -1;
    sendToDetectorStop(F_GET_FRAMES_FROM_START, nullptr, retval);
    LOG(logDEBUG1) << "number of frames from start :" << retval;
    return retval;
}

int64_t Module::getActualTime() const {
    int64_t retval = -1;
    sendToDetectorStop(F_GET_ACTUAL_TIME, nullptr, retval);
    LOG(logDEBUG1) << "actual time :" << retval << "ns";
    return retval;
}

int64_t Module::getMeasurementTime() const {
    int64_t retval = -1;
    sendToDetectorStop(F_GET_MEASUREMENT_TIME, nullptr, retval);
    LOG(logDEBUG1) << "measurement time :" << retval << "ns";
    return retval;
}

slsDetectorDefs::timingMode Module::getTimingMode() {
    return sendToDetector<timingMode>(F_SET_TIMING_MODE, -1);
}

void Module::setTimingMode(timingMode value) {
    timingMode retval = GET_TIMING_MODE;
    LOG(logDEBUG1) << "Setting timing mode to " << value;
    sendToDetector(F_SET_TIMING_MODE, static_cast<int>(value), retval);
    if (shm()->useReceiverFlag) {
        LOG(logDEBUG1) << "Sending timing mode to Receiver: " << value;
        sendToReceiver(F_SET_RECEIVER_TIMING_MODE, value, nullptr);
    }
}

int Module::getDynamicRange() {
    return sendToDetector<int>(F_SET_DYNAMIC_RANGE, -1);
}

void Module::setDynamicRange(int n) {
    int prev_val = n;
    if (shm()->myDetectorType == EIGER) {
        prev_val = getDynamicRange();
    }

    int retval = -1;
    LOG(logDEBUG1) << "Setting dynamic range to " << n;
    sendToDetector(F_SET_DYNAMIC_RANGE, n, retval);
    LOG(logDEBUG1) << "Dynamic Range: " << retval;

    if (shm()->useReceiverFlag) {
        int arg = retval;
        retval = -1;
        LOG(logDEBUG1) << "Sending dynamic range to receiver: " << arg;
        sendToReceiver(F_SET_RECEIVER_DYNAMIC_RANGE, arg, retval);
        LOG(logDEBUG1) << "Receiver Dynamic range: " << retval;
    }

    // changes in dr
    if (n != prev_val) {
        // update speed for usability
        if (n == 32) {
            LOG(logINFO) << "Setting Clock to Quarter Speed to cope with "
                            "Dynamic Range of 32";
            setClockDivider(RUN_CLOCK, 2);
        } else if (prev_val == 32) {
            LOG(logINFO) << "Setting Clock to Full Speed for Dynamic Range of "
                         << n;
            setClockDivider(RUN_CLOCK, 0);
        }
        updateRateCorrection();
    }
}

int Module::setDAC(int val, dacIndex index, int mV) {
    int args[]{static_cast<int>(index), mV, val};
    int retval = -1;
    LOG(logDEBUG1) << "Setting DAC " << index << " to " << val
                   << (mV != 0 ? "mV" : "dac units");
    sendToDetector(F_SET_DAC, args, retval);
    LOG(logDEBUG1) << "Dac index " << index << ": " << retval
                   << (mV != 0 ? "mV" : "dac units");
    return retval;
}

int Module::getOnChipDAC(slsDetectorDefs::dacIndex index, int chipIndex) {
    int args[]{static_cast<int>(index), chipIndex};
    int retval = -1;
    sendToDetector(F_GET_ON_CHIP_DAC, args, retval);
    LOG(logDEBUG1) << "On chip DAC " << index << " (chip index:" << chipIndex
                   << "): " << retval;
    return retval;
}

void Module::setOnChipDAC(slsDetectorDefs::dacIndex index, int chipIndex,
                          int value) {
    int args[]{static_cast<int>(index), chipIndex, value};
    LOG(logDEBUG1) << "Setting On chip DAC " << index
                   << " (chip index:" << chipIndex << ") to " << value;
    sendToDetector(F_SET_ON_CHIP_DAC, args, nullptr);
}

int Module::getADC(dacIndex index) {
    return sendToDetector<int>(F_GET_ADC, static_cast<int>(index));
}

slsDetectorDefs::externalSignalFlag
Module::setExternalSignalFlags(externalSignalFlag pol) {
    LOG(logDEBUG1) << "Setting signal flag to " << pol;
    return sendToDetector<slsDetectorDefs::externalSignalFlag>(F_SET_EXTERNAL_SIGNAL_FLAG, pol);
}

void Module::setParallelMode(const bool enable) {
    LOG(logDEBUG1) << "Setting parallel mode to " << enable;
    sendToDetector(F_SET_PARALLEL_MODE, static_cast<int>(enable), nullptr);
}

bool Module::getParallelMode() {
    auto r = sendToDetector<int>(F_GET_PARALLEL_MODE);
    return static_cast<bool>(r);
}

void Module::setOverFlowMode(const bool enable) {
    int arg = static_cast<int>(enable);
    LOG(logDEBUG1) << "Setting overflow mode to " << arg;
    sendToDetector(F_SET_OVERFLOW_MODE, arg, nullptr);
}

bool Module::getOverFlowMode() {
    auto r = sendToDetector<int>(F_GET_OVERFLOW_MODE);
    return static_cast<bool>(r);
}

void Module::setStoreInRamMode(const bool enable) {
    int arg = static_cast<int>(enable);
    LOG(logDEBUG1) << "Setting store in ram mode to " << arg;
    sendToDetector(F_SET_STOREINRAM_MODE, arg, nullptr);
}

bool Module::getStoreInRamMode() {
    auto r = sendToDetector<int>(F_GET_STOREINRAM_MODE);
    return static_cast<bool>(r);
}

void Module::setReadoutMode(const slsDetectorDefs::readoutMode mode) {
    auto arg = static_cast<uint32_t>(mode);
    LOG(logDEBUG1) << "Setting readout mode to " << arg;
    sendToDetector(F_SET_READOUT_MODE, arg, nullptr);
    // update #nchan, as it depends on #samples, adcmask,
    if (shm()->myDetectorType == CHIPTESTBOARD) {
        updateNumberOfChannels();
    }
    if (shm()->useReceiverFlag) {
        sendToReceiver(F_RECEIVER_SET_READOUT_MODE, mode, nullptr);
    }
}

slsDetectorDefs::readoutMode Module::getReadoutMode() {
    auto r = sendToDetector<int>(F_GET_READOUT_MODE);
    return static_cast<readoutMode>(r);
}

void Module::setInterruptSubframe(const bool enable) {
    int arg = static_cast<int>(enable);
    LOG(logDEBUG1) << "Setting Interrupt subframe to " << arg;
    sendToDetector(F_SET_INTERRUPT_SUBFRAME, arg, nullptr);
}

bool Module::getInterruptSubframe() {
    auto r = sendToDetector<int>(F_GET_INTERRUPT_SUBFRAME);
    return static_cast<bool>(r);
}

uint32_t Module::writeRegister(uint32_t addr, uint32_t val) {
    uint32_t args[]{addr, val};
    return sendToDetector<uint32_t>(F_WRITE_REGISTER, args);
}

uint32_t Module::readRegister(uint32_t addr) {
    return sendToDetector<uint32_t>(F_READ_REGISTER, addr);
}

uint32_t Module::setBit(uint32_t addr, int n) {
    if (n < 0 || n > 31) {
        throw RuntimeError("Bit number " + std::to_string(n) + " out of Range");
    } else {
        uint32_t val = readRegister(addr);
        return writeRegister(addr, val | 1 << n);
    }
}

uint32_t Module::clearBit(uint32_t addr, int n) {
    if (n < 0 || n > 31) {
        throw RuntimeError("Bit number " + std::to_string(n) + " out of Range");
    } else {
        uint32_t val = readRegister(addr);
        return writeRegister(addr, val & ~(1 << n));
    }
}

void Module::setReceiverHostname(const std::string &receiverIP) {
    LOG(logDEBUG1) << "Setting up Receiver with " << receiverIP;
    // recieverIP is none
    if (receiverIP == "none") {
        memset(shm()->rxHostname, 0, MAX_STR_LENGTH);
        sls::strcpy_safe(shm()->rxHostname, "none");
        shm()->useReceiverFlag = false;
    }

    // stop acquisition if running
    if (getRunStatus() == RUNNING) {
        LOG(logWARNING) << "Acquisition already running, Stopping it.";
        stopAcquisition();
    }

    // start updating
    std::string host = receiverIP;
    auto res = sls::split(host, ':');
    if (res.size() > 1) {
        host = res[0];
        shm()->rxTCPPort = std::stoi(res[1]);
    }
    sls::strcpy_safe(shm()->rxHostname, host.c_str());
    shm()->useReceiverFlag = true;
    checkReceiverVersionCompatibility();

    // populate parameters from detector
    rxParameters retval;
    sendToDetector(F_GET_RECEIVER_PARAMETERS, nullptr, retval);

    // populate from shared memory
    retval.detType = shm()->myDetectorType;
    retval.multiSize.x = shm()->multiSize.x;
    retval.multiSize.y = shm()->multiSize.y;
    retval.detId = detId;
    memset(retval.hostname, 0, sizeof(retval.hostname));
    strcpy_safe(retval.hostname, shm()->hostname);

    LOG(logDEBUG1) << "detType:" << retval.detType << std::endl
                   << "multiSize.x:" << retval.multiSize.x << std::endl
                   << "multiSize.y:" << retval.multiSize.y << std::endl
                   << "detId:" << retval.detId << std::endl
                   << "hostname:" << retval.hostname << std::endl
                   << "udpInterfaces:" << retval.udpInterfaces << std::endl
                   << "udp_dstport:" << retval.udp_dstport << std::endl
                   << "udp_dstip:" << sls::IpAddr(retval.udp_dstip) << std::endl
                   << "udp_dstmac:" << sls::MacAddr(retval.udp_dstmac)
                   << std::endl
                   << "udp_dstport2:" << retval.udp_dstport2 << std::endl
                   << "udp_dstip2:" << sls::IpAddr(retval.udp_dstip2)
                   << std::endl
                   << "udp_dstmac2:" << sls::MacAddr(retval.udp_dstmac2)
                   << std::endl
                   << "frames:" << retval.frames << std::endl
                   << "triggers:" << retval.triggers << std::endl
                   << "bursts:" << retval.bursts << std::endl
                   << "analogSamples:" << retval.analogSamples << std::endl
                   << "digitalSamples:" << retval.digitalSamples << std::endl
                   << "expTimeNs:" << retval.expTimeNs << std::endl
                   << "periodNs:" << retval.periodNs << std::endl
                   << "subExpTimeNs:" << retval.subExpTimeNs << std::endl
                   << "subDeadTimeNs:" << retval.subDeadTimeNs << std::endl
                   << "activate:" << retval.activate << std::endl
                   << "quad:" << retval.quad << std::endl
                   << "dynamicRange:" << retval.dynamicRange << std::endl
                   << "timMode:" << retval.timMode << std::endl
                   << "tenGiga:" << retval.tenGiga << std::endl
                   << "roMode:" << retval.roMode << std::endl
                   << "adcMask:" << retval.adcMask << std::endl
                   << "adc10gMask:" << retval.adc10gMask << std::endl
                   << "roi.xmin:" << retval.roi.xmin << std::endl
                   << "roi.xmax:" << retval.roi.xmax << std::endl
                   << "countermask:" << retval.countermask << std::endl
                   << "burstType:" << retval.burstType << std::endl;

    sls::MacAddr retvals[2];
    sendToReceiver(F_SETUP_RECEIVER, retval, retvals);
    // update detectors with dest mac
    if (retval.udp_dstmac == 0 && retvals[0] != 0) {
        LOG(logINFO) << "Setting destination udp mac of "
                        "detector "
                     << detId << " to " << retvals[0];
        sendToDetector(F_SET_DEST_UDP_MAC, retvals[0], nullptr);
    }
    if (retval.udp_dstmac2 == 0 && retvals[1] != 0) {
        LOG(logINFO) << "Setting destination udp mac2 of "
                        "detector "
                     << detId << " to " << retvals[1];
        sendToDetector(F_SET_DEST_UDP_MAC2, retvals[1], nullptr);
    }

    // update numinterfaces if different
    shm()->numUDPInterfaces = retval.udpInterfaces;

    if (shm()->myDetectorType == MOENCH) {
        setAdditionalJsonParameter("adcmask_1g",
                                   std::to_string(retval.adcMask));
        setAdditionalJsonParameter("adcmask_10g",
                                   std::to_string(retval.adc10gMask));
    }

    // to use rx_hostname if empty and also update client zmqip
    updateReceiverStreamingIP();
}

std::string Module::getReceiverHostname() const {
    return std::string(shm()->rxHostname);
}

void Module::setSourceUDPMAC(const sls::MacAddr mac) {
    LOG(logDEBUG1) << "Setting source udp mac to " << mac;
    if (mac == 0) {
        throw RuntimeError("Invalid source udp mac address");
    }
    sendToDetector(F_SET_SOURCE_UDP_MAC, mac, nullptr);
}

sls::MacAddr Module::getSourceUDPMAC() {
    return sendToDetector<sls::MacAddr>(F_GET_SOURCE_UDP_MAC);
}

void Module::setSourceUDPMAC2(const sls::MacAddr mac) {
    LOG(logDEBUG1) << "Setting source udp mac2 to " << mac;
    if (mac == 0) {
        throw RuntimeError("Invalid source udp mac address2");
    }
    sendToDetector(F_SET_SOURCE_UDP_MAC2, mac, nullptr);
}

sls::MacAddr Module::getSourceUDPMAC2() {
    return sendToDetector<sls::MacAddr>(F_GET_SOURCE_UDP_MAC2);
}

void Module::setSourceUDPIP(const IpAddr ip) {
    LOG(logDEBUG1) << "Setting source udp ip to " << ip;
    if (ip == 0) {
        throw RuntimeError("Invalid source udp ip address");
    }
    sendToDetector(F_SET_SOURCE_UDP_IP, ip, nullptr);
}

sls::IpAddr Module::getSourceUDPIP() {
    return sendToDetector<sls::IpAddr>(F_GET_SOURCE_UDP_IP);
}

void Module::setSourceUDPIP2(const IpAddr ip) {
    LOG(logDEBUG1) << "Setting source udp ip2 to " << ip;
    if (ip == 0) {
        throw RuntimeError("Invalid source udp ip address2");
    }
    sendToDetector(F_SET_SOURCE_UDP_IP2, ip, nullptr);
}

sls::IpAddr Module::getSourceUDPIP2() {
    return sendToDetector<sls::IpAddr>(F_GET_SOURCE_UDP_IP2);
}

void Module::setDestinationUDPIP(const IpAddr ip) {
    LOG(logDEBUG1) << "Setting destination udp ip to " << ip;
    if (ip == 0) {
        throw RuntimeError("Invalid destination udp ip address");
    }
    sendToDetector(F_SET_DEST_UDP_IP, ip, nullptr);
    if (shm()->useReceiverFlag) {
        sls::MacAddr retval(0LU);
        sendToReceiver(F_SET_RECEIVER_UDP_IP, ip, retval);
        LOG(logINFO) << "Setting destination udp mac of detector " << detId
                     << " to " << retval;
        sendToDetector(F_SET_DEST_UDP_MAC, retval, nullptr);
    }
}

sls::IpAddr Module::getDestinationUDPIP() {
    return sendToDetector<sls::IpAddr>(F_GET_DEST_UDP_IP);
}

void Module::setDestinationUDPIP2(const IpAddr ip) {
    LOG(logDEBUG1) << "Setting destination udp ip2 to " << ip;
    if (ip == 0) {
        throw RuntimeError("Invalid destination udp ip address2");
    }

    sendToDetector(F_SET_DEST_UDP_IP2, ip, nullptr);
    if (shm()->useReceiverFlag) {
        sls::MacAddr retval(0LU);
        sendToReceiver(F_SET_RECEIVER_UDP_IP2, ip, retval);
        LOG(logINFO) << "Setting destination udp mac2 of detector " << detId
                     << " to " << retval;
        sendToDetector(F_SET_DEST_UDP_MAC2, retval, nullptr);
    }
}

sls::IpAddr Module::getDestinationUDPIP2() {
    return sendToDetector<sls::IpAddr>(F_GET_DEST_UDP_IP2);
}

void Module::setDestinationUDPMAC(const MacAddr mac) {
    LOG(logDEBUG1) << "Setting destination udp mac to " << mac;
    if (mac == 0) {
        throw RuntimeError("Invalid destination udp mac address");
    }
    sendToDetector(F_SET_DEST_UDP_MAC, mac, nullptr);
}

sls::MacAddr Module::getDestinationUDPMAC() {
    return sendToDetector<sls::MacAddr>(F_GET_DEST_UDP_MAC);
}

void Module::setDestinationUDPMAC2(const MacAddr mac) {
    LOG(logDEBUG1) << "Setting destination udp mac2 to " << mac;
    if (mac == 0) {
        throw RuntimeError("Invalid desinaion udp mac address2");
    }
    sendToDetector(F_SET_DEST_UDP_MAC2, mac, nullptr);
}

sls::MacAddr Module::getDestinationUDPMAC2() {
    return sendToDetector<sls::MacAddr>(F_GET_DEST_UDP_MAC2);
}

void Module::setDestinationUDPPort(const int port) {
    LOG(logDEBUG1) << "Setting destination udp port to " << port;
    sendToDetector(F_SET_DEST_UDP_PORT, port, nullptr);
    if (shm()->useReceiverFlag) {
        sendToReceiver(F_SET_RECEIVER_UDP_PORT, port, nullptr);
    }
}

int Module::getDestinationUDPPort() {
    return sendToDetector<int>(F_GET_DEST_UDP_PORT);
}

void Module::setDestinationUDPPort2(const int port) {
    LOG(logDEBUG1) << "Setting destination udp port2 to " << port;
    sendToDetector(F_SET_DEST_UDP_PORT2, port, nullptr);
    if (shm()->useReceiverFlag) {
        sendToReceiver(F_SET_RECEIVER_UDP_PORT2, port, nullptr);
    }
}

int Module::getDestinationUDPPort2() {
    return sendToDetector<int>(F_GET_DEST_UDP_PORT2);
}

void Module::setNumberofUDPInterfaces(int n) {
    LOG(logDEBUG1) << "Setting number of udp interfaces to " << n;
    sendToDetector(F_SET_NUM_INTERFACES, n, nullptr);
    shm()->numUDPInterfaces = n;
    if (shm()->useReceiverFlag) {
        sendToReceiver(F_SET_RECEIVER_NUM_INTERFACES, n, nullptr);
    }
}

int Module::getNumberofUDPInterfacesFromShm() {
    return shm()->numUDPInterfaces;
}

int Module::getNumberofUDPInterfaces() {
    int retval = -1;
    LOG(logDEBUG1) << "Getting number of udp interfaces";
    sendToDetector(F_GET_NUM_INTERFACES, nullptr, retval);
    LOG(logDEBUG1) << "Number of udp interfaces: " << retval;
    shm()->numUDPInterfaces = retval;
    return shm()->numUDPInterfaces;
}

void Module::selectUDPInterface(int n) {
    LOG(logDEBUG1) << "Setting selected udp interface to " << n;
    sendToDetector(F_SET_INTERFACE_SEL, n, nullptr);
}

int Module::getSelectedUDPInterface() {
    return sendToDetector<int>(F_GET_INTERFACE_SEL);
}

void Module::setClientStreamingPort(int port) { shm()->zmqport = port; }

int Module::getClientStreamingPort() { return shm()->zmqport; }

void Module::setReceiverStreamingPort(int port) {
    sendToReceiver(F_SET_RECEIVER_STREAMING_PORT, port, nullptr);
}

int Module::getReceiverStreamingPort() {
    return sendToReceiver<int>(F_GET_RECEIVER_STREAMING_PORT);
}

void Module::setClientStreamingIP(const sls::IpAddr ip) {
    LOG(logDEBUG1) << "Setting client zmq ip to " << ip;
    if (ip == 0) {
        throw RuntimeError("Invalid client zmq ip address");
    }
    shm()->zmqip = ip;
}

sls::IpAddr Module::getClientStreamingIP() { return shm()->zmqip; }

void Module::setReceiverStreamingIP(const sls::IpAddr ip) {
    if (ip == 0) {
        throw RuntimeError("Invalid receiver zmq ip address");
    }
    // if client zmqip is empty, update it
    if (shm()->zmqip == 0) {
        shm()->zmqip = ip;
    }
    sendToReceiver(F_SET_RECEIVER_STREAMING_SRC_IP, ip, nullptr);
}

sls::IpAddr Module::getReceiverStreamingIP() {
    return sendToReceiver<sls::IpAddr>(F_GET_RECEIVER_STREAMING_SRC_IP);
}

void Module::updateReceiverStreamingIP() {
    auto ip = getReceiverStreamingIP();
    if (ip == 0) {
        // Hostname could be ip try to decode otherwise look up the hostname
        ip = sls::IpAddr{shm()->rxHostname};
        if (ip == 0) {
            ip = HostnameToIp(shm()->rxHostname);
        }
        LOG(logINFO) << "Setting default receiver " << detId
                     << " streaming zmq ip to " << ip;
    }
    setReceiverStreamingIP(ip);
}

bool Module::getTenGigaFlowControl() {
    return sendToDetector<int>(F_GET_TEN_GIGA_FLOW_CONTROL);
}

void Module::setTenGigaFlowControl(bool enable) {
    int arg = static_cast<int>(enable);
    LOG(logDEBUG1) << "Setting ten giga flow control to " << arg;
    sendToDetector(F_SET_TEN_GIGA_FLOW_CONTROL, arg, nullptr);
}

int Module::getTransmissionDelayFrame() {
    return sendToDetector<int>(F_GET_TRANSMISSION_DELAY_FRAME);
}

void Module::setTransmissionDelayFrame(int value) {
    LOG(logDEBUG1) << "Setting transmission delay frame to " << value;
    sendToDetector(F_SET_TRANSMISSION_DELAY_FRAME, value, nullptr);
}

int Module::getTransmissionDelayLeft() {
    return sendToDetector<int>(F_GET_TRANSMISSION_DELAY_LEFT);
}

void Module::setTransmissionDelayLeft(int value) {
    LOG(logDEBUG1) << "Setting transmission delay left to " << value;
    sendToDetector(F_SET_TRANSMISSION_DELAY_LEFT, value, nullptr);
}

int Module::getTransmissionDelayRight() {
    return sendToDetector<int>(F_GET_TRANSMISSION_DELAY_RIGHT);
}

void Module::setTransmissionDelayRight(int value) {
    LOG(logDEBUG1) << "Setting transmission delay right to " << value;
    sendToDetector(F_SET_TRANSMISSION_DELAY_RIGHT, value, nullptr);
}

void Module::setAdditionalJsonHeader(
    const std::map<std::string, std::string> &jsonHeader) {
    if (!shm()->useReceiverFlag) {
        throw RuntimeError("Set rx_hostname first to use receiver parameters "
                           "(zmq json header)");
    }
    for (auto &it : jsonHeader) {
        if (it.first.empty() || it.first.length() > SHORT_STR_LENGTH ||
            it.second.length() > SHORT_STR_LENGTH) {
            throw RuntimeError(
                it.first + " or " + it.second +
                " pair has invalid size. "
                "Key cannot be empty. Both can have max 20 characters");
        }
    }
    const int size = jsonHeader.size();
    int fnum = F_SET_ADDITIONAL_JSON_HEADER;
    int ret = FAIL;
    LOG(logDEBUG) << "Sending to receiver additional json header "
                  << ToString(jsonHeader);
    auto client = ReceiverSocket(shm()->rxHostname, shm()->rxTCPPort);
    client.Send(&fnum, sizeof(fnum));
    client.Send(&size, sizeof(size));
    if (size > 0) {
        char args[size * 2][SHORT_STR_LENGTH];
        memset(args, 0, sizeof(args));
        int iarg = 0;
        for (auto &it : jsonHeader) {
            sls::strcpy_safe(args[iarg], it.first.c_str());
            sls::strcpy_safe(args[iarg + 1], it.second.c_str());
            iarg += 2;
        }
        client.Send(args, sizeof(args));
    }
    client.Receive(&ret, sizeof(ret));
    if (ret == FAIL) {
        char mess[MAX_STR_LENGTH]{};
        client.Receive(mess, MAX_STR_LENGTH);
        throw RuntimeError("Receiver " + std::to_string(detId) +
                           " returned error: " + std::string(mess));
    }
}

std::map<std::string, std::string> Module::getAdditionalJsonHeader() {
    if (!shm()->useReceiverFlag) {
        throw RuntimeError("Set rx_hostname first to use receiver parameters "
                           "(zmq json header)");
    }
    int fnum = F_GET_ADDITIONAL_JSON_HEADER;
    int ret = FAIL;
    int size = 0;
    auto client = ReceiverSocket(shm()->rxHostname, shm()->rxTCPPort);
    client.Send(&fnum, sizeof(fnum));
    client.Receive(&ret, sizeof(ret));
    if (ret == FAIL) {
        char mess[MAX_STR_LENGTH]{};
        client.Receive(mess, MAX_STR_LENGTH);
        throw RuntimeError("Receiver " + std::to_string(detId) +
                           " returned error: " + std::string(mess));
    } else {
        client.Receive(&size, sizeof(size));
        std::map<std::string, std::string> retval;
        if (size > 0) {
            char retvals[size * 2][SHORT_STR_LENGTH];
            memset(retvals, 0, sizeof(retvals));
            client.Receive(retvals, sizeof(retvals));
            for (int i = 0; i < size; ++i) {
                retval[retvals[2 * i]] = retvals[2 * i + 1];
            }
        }
        LOG(logDEBUG) << "Getting additional json header " << ToString(retval);
        return retval;
    }
}

void Module::setAdditionalJsonParameter(const std::string &key,
                                        const std::string &value) {
    if (key.empty() || key.length() > SHORT_STR_LENGTH ||
        value.length() > SHORT_STR_LENGTH) {
        throw RuntimeError(
            key + " or " + value +
            " pair has invalid size. "
            "Key cannot be empty. Both can have max 2 characters");
    }
    char args[2][SHORT_STR_LENGTH]{};
    sls::strcpy_safe(args[0], key.c_str());
    sls::strcpy_safe(args[1], value.c_str());
    sendToReceiver(F_SET_ADDITIONAL_JSON_PARAMETER, args, nullptr);
}

std::string Module::getAdditionalJsonParameter(const std::string &key) {
    char arg[SHORT_STR_LENGTH]{};
    sls::strcpy_safe(arg, key.c_str());
    char retval[SHORT_STR_LENGTH]{};
    sendToReceiver(F_GET_ADDITIONAL_JSON_PARAMETER, arg, retval);
    return retval;
}

int64_t Module::setReceiverUDPSocketBufferSize(int64_t udpsockbufsize) {
    return sendToReceiver<int64_t>(F_RECEIVER_UDP_SOCK_BUF_SIZE, udpsockbufsize);
}

int64_t Module::getReceiverUDPSocketBufferSize() {
    return setReceiverUDPSocketBufferSize();
}

int64_t Module::getReceiverRealUDPSocketBufferSize() const {
    return sendToReceiver<int64_t>(F_RECEIVER_REAL_UDP_SOCK_BUF_SIZE);
}

void Module::executeFirmwareTest() {
    LOG(logDEBUG1) << "Executing firmware test";
    sendToDetector(F_SET_FIRMWARE_TEST);
}

void Module::executeBusTest() {
    LOG(logDEBUG1) << "Executing bus test";
    sendToDetector(F_SET_BUS_TEST);
}

int Module::getImageTestMode() {
    return sendToDetector<int>(F_GET_IMAGE_TEST_MODE);
}

void Module::setImageTestMode(const int value) {
    LOG(logDEBUG1) << "Sending image test mode " << value;
    sendToDetector(F_SET_IMAGE_TEST_MODE, value, nullptr);
}

std::array<int, 2> Module::getInjectChannel() {
    std::array<int, 2> retvals{};
    sendToDetector(F_GET_INJECT_CHANNEL, nullptr, retvals);
    LOG(logDEBUG1) << "Inject Channel: [offset: " << retvals[0]
                   << ", increment: " << retvals[1] << ']';
    return retvals;
}

void Module::setInjectChannel(const int offsetChannel,
                              const int incrementChannel) {
    int args[]{offsetChannel, incrementChannel};
    LOG(logDEBUG1) << "Setting inject channels [offset: " << offsetChannel
                   << ", increment: " << incrementChannel << ']';
    sendToDetector(F_SET_INJECT_CHANNEL, args, nullptr);
}

std::vector<int> Module::getVetoPhoton(const int chipIndex) {
    int fnum = F_GET_VETO_PHOTON;
    int ret = FAIL;
    auto client = DetectorSocket(shm()->hostname, shm()->controlPort);
    client.Send(&fnum, sizeof(fnum));
    client.Send(&chipIndex, sizeof(chipIndex));
    client.Receive(&ret, sizeof(ret));
    if (ret == FAIL) {
        char mess[MAX_STR_LENGTH]{};
        client.Receive(mess, MAX_STR_LENGTH);
        throw RuntimeError("Detector " + std::to_string(detId) +
                           " returned error: " + std::string(mess));
    } else {
        int nch = -1;
        client.Receive(&nch, sizeof(nch));

        int adus[nch];
        memset(adus, 0, sizeof(adus));
        client.Receive(adus, sizeof(adus));
        std::vector<int> retvals(adus, adus + nch);
        LOG(logDEBUG1) << "Getting veto photon [" << chipIndex << "]: " << nch
                       << " channels\n";
        return retvals;
    }
}

void Module::setVetoPhoton(const int chipIndex, const int numPhotons,
                           const int energy, const std::string &fname) {
    if (shm()->myDetectorType != GOTTHARD2) {
        throw RuntimeError(
            "Set Veto reference is not implemented for this detector");
    }
    if (chipIndex < -1 || chipIndex >= shm()->nChip.x) {
        throw RuntimeError("Could not set veto photon. Invalid chip index: " +
                           std::to_string(chipIndex));
    }
    if (numPhotons < 1) {
        throw RuntimeError(
            "Could not set veto photon. Invalid number of photons: " +
            std::to_string(numPhotons));
    }
    if (energy < 1) {
        throw RuntimeError("Could not set veto photon. Invalid energy: " +
                           std::to_string(energy));
    }
    std::ifstream infile(fname.c_str());
    if (!infile.is_open()) {
        throw RuntimeError("Could not set veto photon. Could not open file: " +
                           fname);
    }

    int totalEnergy = numPhotons * energy;
    int ch = shm()->nChan.x;
    int gainIndex = 2;
    int nRead = 0;
    int value[ch];
    memset(value, 0, sizeof(value));
    bool firstLine = true;

    while (infile.good()) {
        std::string line;
        getline(infile, line);
        if (line.find('#') != std::string::npos) {
            line.erase(line.find('#'));
        }
        if (line.length() < 1) {
            continue;
        }
        std::istringstream ss(line);
        // first line: caluclate gain index from gain thresholds from file
        if (firstLine) {
            int g0 = -1, g1 = -1;
            if (!(ss >> g0 >> g1)) {
                throw RuntimeError(
                    "Could not set veto photon. Invalid gain thresholds");
            }
            // set gain index and gain bit values
            if (totalEnergy < g0) {
                gainIndex = 0;
            } else if (totalEnergy < g1) {
                gainIndex = 1;
            }
            LOG(logINFO) << "Setting veto photon. Reading Gain " << gainIndex
                         << " values";
            firstLine = false;
        }
        // read pedestal and gain values
        else {
            double p[3] = {-1, -1, -1}, g[3] = {-1, -1, -1};
            if (!(ss >> p[0] >> p[1] >> p[2] >> g[0] >> g[1] >> g[2])) {
                throw RuntimeError("Could not set veto photon. Invalid "
                                   "pedestal or gain values for channel " +
                                   std::to_string(nRead));
            }
            value[nRead] =
                p[gainIndex] +
                (g[gainIndex] *
                 totalEnergy); // ADU value = pedestal  + gain * total energy
            ++nRead;
            if (nRead >= ch) {
                break;
            }
        }
    }
    if (nRead != ch) {
        throw RuntimeError("Could not set veto photon. Insufficient pedestal "
                           "pr gain values: " +
                           std::to_string(nRead));
    }

    int fnum = F_SET_VETO_PHOTON;
    int ret = FAIL;
    int args[]{chipIndex, gainIndex, ch};
    LOG(logDEBUG) << "Sending veto photon value to detector [chip:" << chipIndex
                  << ", G" << gainIndex << "]: " << args;
    auto client = DetectorSocket(shm()->hostname, shm()->controlPort);
    client.Send(&fnum, sizeof(fnum));
    client.Send(args, sizeof(args));
    client.Send(value, sizeof(value));
    client.Receive(&ret, sizeof(ret));
    if (ret == FAIL) {
        char mess[MAX_STR_LENGTH]{};
        client.Receive(mess, MAX_STR_LENGTH);
        throw RuntimeError("Detector " + std::to_string(detId) +
                           " returned error: " + std::string(mess));
    }
}

void Module::setVetoReference(const int gainIndex, const int value) {
    int args[]{gainIndex, value};
    LOG(logDEBUG1) << "Setting veto reference [gainIndex: " << gainIndex
                   << ", value: 0x" << std::hex << value << std::dec << ']';
    sendToDetector(F_SET_VETO_REFERENCE, args, nullptr);
}

slsDetectorDefs::burstMode Module::getBurstMode() {
    auto r = sendToDetector<int>(F_GET_BURST_MODE);
    return static_cast<slsDetectorDefs::burstMode>(r);
}

void Module::setBurstMode(slsDetectorDefs::burstMode value) {
    int arg = static_cast<int>(value);
    LOG(logDEBUG1) << "Setting burst mode to " << arg;
    sendToDetector(F_SET_BURST_MODE, arg, nullptr);
    if (shm()->useReceiverFlag) {
        LOG(logDEBUG1) << "Sending burst mode to Receiver: " << value;
        sendToReceiver(F_SET_RECEIVER_BURST_MODE, value, nullptr);
    }
}

bool Module::getCurrentSource() {
    return sendToDetector<int>(F_GET_CURRENT_SOURCE);
}

void Module::setCurrentSource(bool value) {
    sendToDetector(F_SET_CURRENT_SOURCE, static_cast<int>(value), nullptr);
}

slsDetectorDefs::timingSourceType Module::getTimingSource() {
    auto r = sendToDetector<int>(F_GET_TIMING_SOURCE);
    return static_cast<slsDetectorDefs::timingSourceType>(r);
}

void Module::setTimingSource(slsDetectorDefs::timingSourceType value) {
    sendToDetector(F_SET_TIMING_SOURCE, static_cast<int>(value), nullptr);
}

int Module::setCounterBit(int cb) {
    return sendToDetector<int>(F_SET_COUNTER_BIT, cb);
}

void Module::clearROI() {
    LOG(logDEBUG1) << "Clearing ROI";
    setROI(slsDetectorDefs::ROI{});
}

void Module::setROI(slsDetectorDefs::ROI arg) {
    if (arg.xmin < 0 || arg.xmax >= getNumberOfChannels().x) {
        arg.xmin = -1;
        arg.xmax = -1;
    }
    LOG(logDEBUG) << "Sending ROI to detector [" << arg.xmin << ", " << arg.xmax
                  << "]";
    sendToDetector(F_SET_ROI, arg, nullptr);
    if (shm()->useReceiverFlag) {
        LOG(logDEBUG1) << "Sending ROI to receiver";
        sendToReceiver(F_RECEIVER_SET_ROI, arg, nullptr);
    }
}

slsDetectorDefs::ROI Module::getROI() {
    return sendToDetector<slsDetectorDefs::ROI>(F_GET_ROI);
}

void Module::setADCEnableMask(uint32_t mask) {
    uint32_t arg = mask;
    LOG(logDEBUG1) << "Setting ADC Enable mask to 0x" << std::hex << arg
                   << std::dec;
    sendToDetector(F_SET_ADC_ENABLE_MASK, &arg, sizeof(arg), nullptr, 0);

    // update #nchan, as it depends on #samples, adcmask,
    updateNumberOfChannels();

    // send to processor
    if (shm()->myDetectorType == MOENCH)
        setAdditionalJsonParameter("adcmask_1g", std::to_string(mask));

    if (shm()->useReceiverFlag) {
        int fnum = F_RECEIVER_SET_ADC_MASK;
        int retval = -1;
        LOG(logDEBUG1) << "Setting ADC Enable mask to 0x" << std::hex << mask
                       << std::dec << " in receiver";
        sendToReceiver(fnum, mask, retval);
    }
}

uint32_t Module::getADCEnableMask() {
    return sendToDetector<uint32_t>(F_GET_ADC_ENABLE_MASK);
}

void Module::setTenGigaADCEnableMask(uint32_t mask) {
    uint32_t arg = mask;
    LOG(logDEBUG1) << "Setting 10Gb ADC Enable mask to 0x" << std::hex << arg
                   << std::dec;
    sendToDetector(F_SET_ADC_ENABLE_MASK_10G, &arg, sizeof(arg), nullptr, 0);

    // update #nchan, as it depends on #samples, adcmask,
    updateNumberOfChannels();

    // send to processor
    if (shm()->myDetectorType == MOENCH)
        setAdditionalJsonParameter("adcmask_10g", std::to_string(mask));

    if (shm()->useReceiverFlag) {
        int fnum = F_RECEIVER_SET_ADC_MASK_10G;
        int retval = -1;
        LOG(logDEBUG1) << "Setting 10Gb ADC Enable mask to 0x" << std::hex
                       << mask << std::dec << " in receiver";
        sendToReceiver(fnum, mask, retval);
    }
}

uint32_t Module::getTenGigaADCEnableMask() {
    return sendToDetector<uint32_t>(F_GET_ADC_ENABLE_MASK_10G);
}

void Module::setADCInvert(uint32_t value) {
    LOG(logDEBUG1) << "Setting ADC Invert to 0x" << std::hex << value
                   << std::dec;
    sendToDetector(F_SET_ADC_INVERT, value, nullptr);
}

uint32_t Module::getADCInvert() {
    return sendToDetector<uint32_t>(F_GET_ADC_INVERT);
}

int Module::setExternalSamplingSource(int value) {
    return sendToDetector<int>(F_EXTERNAL_SAMPLING_SOURCE, value);
}

int Module::getExternalSamplingSource() {
    return setExternalSamplingSource(-1);
}

int Module::setExternalSampling(int value) {
    return sendToDetector<int>(F_EXTERNAL_SAMPLING, value);
}

int Module::getExternalSampling() { return setExternalSampling(-1); }

void Module::setReceiverDbitList(const std::vector<int> &list) {
    LOG(logDEBUG1) << "Setting Receiver Dbit List";
    if (list.size() > 64) {
        throw sls::RuntimeError("Dbit list size cannot be greater than 64\n");
    }
    for (auto &it : list) {
        if (it < 0 || it > 63) {
            throw sls::RuntimeError(
                "Dbit list value must be between 0 and 63\n");
        }
    }
    sls::FixedCapacityContainer<int, MAX_RX_DBIT> arg = list;
    sendToReceiver(F_SET_RECEIVER_DBIT_LIST, arg, nullptr);
}

std::vector<int> Module::getReceiverDbitList() const {
    return sendToReceiver<sls::FixedCapacityContainer<int, MAX_RX_DBIT>>(
        F_GET_RECEIVER_DBIT_LIST);
}

void Module::setReceiverDbitOffset(int value) {
    sendToReceiver(F_SET_RECEIVER_DBIT_OFFSET, value, nullptr);
}

int Module::getReceiverDbitOffset() {
    return sendToReceiver<int>(F_GET_RECEIVER_DBIT_OFFSET);
}

void Module::writeAdcRegister(uint32_t addr, uint32_t val) {
    uint32_t args[]{addr, val};
    LOG(logDEBUG1) << "Writing to ADC register 0x" << std::hex << addr
                   << "data: 0x" << std::hex << val << std::dec;
    sendToDetector(F_WRITE_ADC_REG, args, nullptr);
}

bool Module::getActivate() {
    int retval = -1, retval2 = -1;
    int arg = -1;
    sendToDetector(F_ACTIVATE, arg, retval);
    sendToDetectorStop(F_ACTIVATE, arg, retval2);
    if (retval != retval2) {
        std::ostringstream oss;
        oss << "Inconsistent activate state. Control Server: " << retval
            << ". Stop Server: " << retval2;
        throw RuntimeError(oss.str());
    }
    return retval;
}

void Module::setActivate(const bool enable) {
    int retval = -1;
    int arg = static_cast<int>(enable);
    LOG(logDEBUG1) << "Setting activate flag to " << enable;
    sendToDetector(F_ACTIVATE, arg, retval);
    sendToDetectorStop(F_ACTIVATE, arg, retval);
    if (shm()->useReceiverFlag) {
        sendToReceiver(F_RECEIVER_ACTIVATE, retval, nullptr);
    }
}

bool Module::getDeactivatedRxrPaddingMode() {
    return sendToReceiver<int>(F_GET_RECEIVER_DEACTIVATED_PADDING);
}

void Module::setDeactivatedRxrPaddingMode(bool padding) {
    sendToReceiver(F_SET_RECEIVER_DEACTIVATED_PADDING, static_cast<int>(padding), nullptr);
}

bool Module::getFlippedDataX() {
    return sendToReceiver<int>(F_SET_FLIPPED_DATA_RECEIVER, -1);
}

void Module::setFlippedDataX(bool value) {
    sendToReceiver<int>(F_SET_FLIPPED_DATA_RECEIVER, static_cast<int>(value));
}

int Module::getAllTrimbits() {
    return sendToDetector<int>(F_SET_ALL_TRIMBITS, -1);
}

void Module::setAllTrimbits(int val) {
    sendToDetector<int>(F_SET_ALL_TRIMBITS, val);
}

int Module::setTrimEn(const std::vector<int> &energies) {
    if (shm()->myDetectorType != EIGER) {
        throw RuntimeError("setTrimEn not implemented for this detector.");
    }
    if (energies.size() > MAX_TRIMEN) {
        std::ostringstream os;
        os << "Size of trim energies: " << energies.size()
           << " exceeds what can be stored in shared memory: " << MAX_TRIMEN
           << "\n";
        throw RuntimeError(os.str());
    }
    shm()->trimEnergies = energies;
    return shm()->trimEnergies.size();
}

std::vector<int> Module::getTrimEn() {
    if (shm()->myDetectorType != EIGER) {
        throw RuntimeError("getTrimEn not implemented for this detector.");
    }
    return std::vector<int>(shm()->trimEnergies.begin(),
                            shm()->trimEnergies.end());
}

void Module::pulsePixel(int n, int x, int y) {
    int args[]{n, x, y};
    LOG(logDEBUG1) << "Pulsing pixel " << n << " number of times at (" << x
                   << "," << y << ")";
    sendToDetector(F_PULSE_PIXEL, args, nullptr);
}

void Module::pulsePixelNMove(int n, int x, int y) {
    int args[]{n, x, y};
    LOG(logDEBUG1) << "Pulsing pixel " << n
                   << " number of times and move by delta (" << x << "," << y
                   << ")";
    sendToDetector(F_PULSE_PIXEL_AND_MOVE, args, nullptr);
}

void Module::pulseChip(int n_pulses) {
    LOG(logDEBUG1) << "Pulsing chip " << n_pulses << " number of times";
    sendToDetector(F_PULSE_CHIP, n_pulses, nullptr);
}

int Module::setThresholdTemperature(int val) {
    int retval = -1;
    LOG(logDEBUG1) << "Setting threshold temperature to " << val;
    sendToDetectorStop(F_THRESHOLD_TEMP, val, retval);
    LOG(logDEBUG1) << "Threshold temperature: " << retval;
    return retval;
}

int Module::setTemperatureControl(int val) {
    int retval = -1;
    LOG(logDEBUG1) << "Setting temperature control to " << val;
    sendToDetectorStop(F_TEMP_CONTROL, val, retval);
    LOG(logDEBUG1) << "Temperature control: " << retval;
    return retval;
}

int Module::setTemperatureEvent(int val) {
    int retval = -1;
    LOG(logDEBUG1) << "Setting temperature event to " << val;
    sendToDetectorStop(F_TEMP_EVENT, val, retval);
    LOG(logDEBUG1) << "Temperature event: " << retval;
    return retval;
}

int Module::setStoragecellStart(int pos) {
    return sendToDetector<int>(F_STORAGE_CELL_START, pos);
}

void Module::programFPGA(std::vector<char> buffer) {
    switch (shm()->myDetectorType) {
    case JUNGFRAU:
    case CHIPTESTBOARD:
    case MOENCH:
        programFPGAviaBlackfin(buffer);
        break;
    case MYTHEN3:
    case GOTTHARD2:
        programFPGAviaNios(buffer);
        break;
    default:
        throw RuntimeError("Program FPGA is not implemented for this detector");
    }
}

void Module::programFPGAviaBlackfin(std::vector<char> buffer) {
    uint64_t filesize = buffer.size();

    // send program from memory to detector
    int fnum = F_PROGRAM_FPGA;
    int ret = FAIL;
    char mess[MAX_STR_LENGTH] = {0};
    LOG(logINFO) << "Sending programming binary (from pof) to detector "
                 << detId << " (" << shm()->hostname << ")";

    auto client = DetectorSocket(shm()->hostname, shm()->controlPort);
    client.Send(&fnum, sizeof(fnum));
    client.Send(&filesize, sizeof(filesize));
    client.Receive(&ret, sizeof(ret));
    // error in detector at opening file pointer to flash
    if (ret == FAIL) {
        client.Receive(mess, sizeof(mess));
        std::ostringstream os;
        os << "Detector " << detId << " (" << shm()->hostname << ")"
           << " returned error: " << mess;
        throw RuntimeError(os.str());
    }

    // erasing flash
    LOG(logINFO) << "Erasing Flash for detector " << detId << " ("
                 << shm()->hostname << ")";
    printf("%d%%\r", 0);
    std::cout << std::flush;
    // erasing takes 65 seconds, printing here (otherwise need threads
    // in server-unnecessary)
    const int ERASE_TIME = 65;
    int count = ERASE_TIME + 1;
    while (count > 0) {
        usleep(1 * 1000 * 1000);
        --count;
        printf(
            "%d%%\r",
            static_cast<int>(
                (static_cast<double>(ERASE_TIME - count) / ERASE_TIME) * 100));
        std::cout << std::flush;
    }
    printf("\n");
    LOG(logINFO) << "Writing to Flash to detector " << detId << " ("
                 << shm()->hostname << ")";
    printf("%d%%\r", 0);
    std::cout << std::flush;

    // sending program in parts of 2mb each
    uint64_t unitprogramsize = 0;
    int currentPointer = 0;
    uint64_t totalsize = filesize;
    while (filesize > 0) {
        unitprogramsize = MAX_FPGAPROGRAMSIZE; // 2mb
        if (unitprogramsize > filesize) {      // less than 2mb
            unitprogramsize = filesize;
        }
        LOG(logDEBUG1) << "unitprogramsize:" << unitprogramsize
                       << "\t filesize:" << filesize;

        client.Send(&buffer[currentPointer], unitprogramsize);
        client.Receive(&ret, sizeof(ret));
        if (ret == FAIL) {
            printf("\n");
            client.Receive(mess, sizeof(mess));
            std::ostringstream os;
            os << "Detector " << detId << " (" << shm()->hostname << ")"
               << " returned error: " << mess;
            throw RuntimeError(os.str());
        }
        filesize -= unitprogramsize;
        currentPointer += unitprogramsize;

        // print progress
        printf(
            "%d%%\r",
            static_cast<int>(
                (static_cast<double>(totalsize - filesize) / totalsize) * 100));
        std::cout << std::flush;
    }
    printf("\n");
    LOG(logINFO) << "FPGA programmed successfully";
    rebootController();
}

void Module::programFPGAviaNios(std::vector<char> buffer) {
    uint64_t filesize = buffer.size();
    int fnum = F_PROGRAM_FPGA;
    int ret = FAIL;
    char mess[MAX_STR_LENGTH] = {0};
    LOG(logINFO) << "Sending programming binary (from rbf) to detector "
                 << detId << " (" << shm()->hostname << ")";

    auto client = DetectorSocket(shm()->hostname, shm()->controlPort);
    client.Send(&fnum, sizeof(fnum));
    // filesize
    client.Send(&filesize, sizeof(filesize));
    client.Receive(&ret, sizeof(ret));
    if (ret == FAIL) {
        client.Receive(mess, sizeof(mess));
        std::ostringstream os;
        os << "Detector " << detId << " (" << shm()->hostname << ")"
           << " returned error: " << mess;
        throw RuntimeError(os.str());
    }
    // program
    client.Send(&buffer[0], filesize);
    client.Receive(&ret, sizeof(ret));
    if (ret == FAIL) {
        client.Receive(mess, sizeof(mess));
        std::ostringstream os;
        os << "Detector " << detId << " (" << shm()->hostname << ")"
           << " returned error: " << mess;
        throw RuntimeError(os.str());
    }
    LOG(logINFO) << "FPGA programmed successfully";
    rebootController();
}

void Module::resetFPGA() { return sendToDetector(F_RESET_FPGA); }

void Module::copyDetectorServer(const std::string &fname,
                                const std::string &hostname) {
    char args[2][MAX_STR_LENGTH]{};
    sls::strcpy_safe(args[0], fname.c_str());
    sls::strcpy_safe(args[1], hostname.c_str());
    LOG(logINFO) << "Sending detector server " << args[0] << " from host "
                 << args[1];
    sendToDetector(F_COPY_DET_SERVER, args, nullptr);
}

void Module::rebootController() {
    LOG(logDEBUG1) << "Rebooting Controller";
    sendToDetector(F_REBOOT_CONTROLLER, nullptr, nullptr);
    LOG(logINFO) << "Controller rebooted successfully!";
}

int Module::powerChip(int ival) {
    return sendToDetector<int>(F_POWER_CHIP, ival);
}

int Module::setAutoComparatorDisableMode(int ival) {
    return sendToDetector<int>(F_AUTO_COMP_DISABLE, ival);
}

void Module::setModule(sls_detector_module &module, int tb) {
    int fnum = F_SET_MODULE;
    int ret = FAIL;
    LOG(logDEBUG1) << "Setting module with tb:" << tb;
    // to exclude trimbits
    if (tb == 0) {
        module.nchan = 0;
        module.nchip = 0;
    }
    auto client = DetectorSocket(shm()->hostname, shm()->controlPort);
    client.Send(&fnum, sizeof(fnum));
    sendModule(&module, client);
    client.Receive(&ret, sizeof(ret));
    if (ret == FAIL) {
        char mess[MAX_STR_LENGTH] = {0};
        client.Receive(mess, sizeof(mess));
        throw RuntimeError("Detector " + std::to_string(detId) +
                           " returned error: " + mess);
    }
}

sls_detector_module Module::getModule() {
    int fnum = F_GET_MODULE;
    LOG(logDEBUG1) << "Getting module";
    sls_detector_module myMod{shm()->myDetectorType};
    auto client = DetectorSocket(shm()->hostname, shm()->controlPort);
    client.sendCommandThenRead(fnum, nullptr, 0, nullptr, 0);
    receiveModule(&myMod, client);
    return myMod;
}

void Module::setDefaultRateCorrection() {
    LOG(logDEBUG1) << "Setting Default Rate Correction";
    int64_t arg = -1;
    sendToDetector(F_SET_RATE_CORRECT, arg, nullptr);
}

void Module::setRateCorrection(int64_t t) {
    LOG(logDEBUG1) << "Setting Rate Correction to " << t;
    sendToDetector(F_SET_RATE_CORRECT, t, nullptr);
}

int64_t Module::getRateCorrection() {
    return sendToDetector<int64_t>(F_GET_RATE_CORRECT);
}

void Module::updateRateCorrection() {
    sendToDetector(F_UPDATE_RATE_CORRECTION);
}

std::string Module::printReceiverConfiguration() {
    std::ostringstream os;
    os << "\n\nDetector " << detId << "\nReceiver Hostname:\t"
       << getReceiverHostname();

    if (shm()->myDetectorType == JUNGFRAU) {
        os << "\nNumber of Interfaces:\t" << getNumberofUDPInterfaces()
           << "\nSelected Interface:\t" << getSelectedUDPInterface();
    }

    os << "\nDetector UDP IP:\t" << getSourceUDPIP() << "\nDetector UDP MAC:\t"
       << getSourceUDPMAC() << "\nReceiver UDP IP:\t" << getDestinationUDPIP()
       << "\nReceiver UDP MAC:\t" << getDestinationUDPMAC();

    if (shm()->myDetectorType == JUNGFRAU) {
        os << "\nDetector UDP IP2:\t" << getSourceUDPIP2()
           << "\nDetector UDP MAC2:\t" << getSourceUDPMAC2()
           << "\nReceiver UDP IP2:\t" << getDestinationUDPIP2()
           << "\nReceiver UDP MAC2:\t" << getDestinationUDPMAC2();
    }
    os << "\nReceiver UDP Port:\t" << getDestinationUDPPort();
    if (shm()->myDetectorType == JUNGFRAU || shm()->myDetectorType == EIGER) {
        os << "\nReceiver UDP Port2:\t" << getDestinationUDPPort2();
    }
    os << "\n";
    return os.str();
}

bool Module::getUseReceiverFlag() const { return shm()->useReceiverFlag; }

int Module::lockReceiver(int lock) {
    return sendToReceiver<int>(F_LOCK_RECEIVER, lock);
}

sls::IpAddr Module::getReceiverLastClientIP() const {
    return sendToReceiver<sls::IpAddr>(F_GET_LAST_RECEIVER_CLIENT_IP);
}

void Module::exitReceiver() {
    LOG(logDEBUG1) << "Sending exit command to receiver server";
    sendToReceiver(F_EXIT_RECEIVER, nullptr, nullptr);
}

std::string Module::getFilePath() {
    char ret[MAX_STR_LENGTH]{};
    sendToReceiver(F_GET_RECEIVER_FILE_PATH, nullptr, ret);
    return ret;
}

void Module::setFilePath(const std::string &path) {
    if (path.empty()) {
        throw RuntimeError("Cannot set empty file path");
    }
    char args[MAX_STR_LENGTH]{};
    sls::strcpy_safe(args, path.c_str());
    sendToReceiver(F_SET_RECEIVER_FILE_PATH, args, nullptr);
}

std::string Module::getFileName() {
    char retvals[MAX_STR_LENGTH]{};
    sendToReceiver(F_GET_RECEIVER_FILE_NAME, nullptr, retvals);
    return std::string(retvals);
}

void Module::setFileName(const std::string &fname) {
    if (fname.empty()) {
        throw RuntimeError("Cannot set empty file name prefix");
    }
    char args[MAX_STR_LENGTH]{};
    sls::strcpy_safe(args, fname.c_str());
    sendToReceiver(F_SET_RECEIVER_FILE_NAME, args, nullptr);
}

int64_t Module::getFileIndex() {
    return sendToReceiver<int64_t>(F_GET_RECEIVER_FILE_INDEX);
}

void Module::setFileIndex(int64_t file_index) {
    sendToReceiver(F_SET_RECEIVER_FILE_INDEX, file_index, nullptr);
}

void Module::incrementFileIndex() {
    sendToReceiver(F_INCREMENT_FILE_INDEX, nullptr, nullptr);
}

slsDetectorDefs::fileFormat Module::getFileFormat() {
    return static_cast<fileFormat>(
        sendToReceiver<int>(F_GET_RECEIVER_FILE_FORMAT));
}

void Module::setFileFormat(fileFormat f) {
    sendToReceiver(F_SET_RECEIVER_FILE_FORMAT, static_cast<int>(f), nullptr);
}

int Module::getFramesPerFile() {
    return sendToReceiver<int>(F_GET_RECEIVER_FRAMES_PER_FILE);
}

void Module::setFramesPerFile(int n_frames) {
    sendToReceiver(F_SET_RECEIVER_FRAMES_PER_FILE, n_frames, nullptr);
}

slsDetectorDefs::frameDiscardPolicy Module::getReceiverFramesDiscardPolicy() {
    return static_cast<frameDiscardPolicy>(
        sendToReceiver<int>(F_GET_RECEIVER_DISCARD_POLICY));
}

void Module::setReceiverFramesDiscardPolicy(frameDiscardPolicy f) {
    sendToReceiver(F_SET_RECEIVER_DISCARD_POLICY, static_cast<int>(f), nullptr);
}

bool Module::getPartialFramesPadding() {
    return sendToReceiver<int>(F_GET_RECEIVER_PADDING);
}

void Module::setPartialFramesPadding(bool padding) {
    sendToReceiver(F_SET_RECEIVER_PADDING, static_cast<int>(padding), nullptr);
}

void Module::startReceiver() {
    LOG(logDEBUG1) << "Starting Receiver";
    shm()->stoppedFlag = false;
    sendToReceiver(F_START_RECEIVER, nullptr, nullptr);
}

void Module::stopReceiver() {
    LOG(logDEBUG1) << "Stopping Receiver";
    int arg = static_cast<int>(shm()->stoppedFlag);
    sendToReceiver(F_STOP_RECEIVER, arg, nullptr);
}

slsDetectorDefs::runStatus Module::getReceiverStatus() const {
    return sendToReceiver<runStatus>(F_GET_RECEIVER_STATUS);
}

int64_t Module::getFramesCaughtByReceiver() const {
    return sendToReceiver<int64_t>(F_GET_RECEIVER_FRAMES_CAUGHT);
}

std::vector<uint64_t> Module::getNumMissingPackets() const {
    //TODO!(Erik) Refactor
    LOG(logDEBUG1) << "Getting num missing packets";
    if (shm()->useReceiverFlag) {
        int fnum = F_GET_NUM_MISSING_PACKETS;
        int ret = FAIL;
        auto client = ReceiverSocket(shm()->rxHostname, shm()->rxTCPPort);
        client.Send(&fnum, sizeof(fnum));
        client.Receive(&ret, sizeof(ret));
        if (ret == FAIL) {
            char mess[MAX_STR_LENGTH]{};
            client.Receive(mess, MAX_STR_LENGTH);
            throw RuntimeError("Receiver " + std::to_string(detId) +
                               " returned error: " + std::string(mess));
        } else {
            int nports = -1;
            client.Receive(&nports, sizeof(nports));
            uint64_t mp[nports];
            memset(mp, 0, sizeof(mp));
            client.Receive(mp, sizeof(mp));
            std::vector<uint64_t> retval(mp, mp + nports);
            LOG(logDEBUG1) << "Missing packets of Receiver" << detId << ": "
                           << sls::ToString(retval);
            return retval;
        }
    }
    throw RuntimeError("No receiver to get missing packets.");
}

uint64_t Module::getReceiverCurrentFrameIndex() const {
    return sendToReceiver<uint64_t>(F_GET_RECEIVER_FRAME_INDEX);
}

int Module::getReceiverProgress() const {
    return sendToReceiver<int>(F_GET_RECEIVER_PROGRESS);
}

void Module::setFileWrite(bool value) {
    sendToReceiver(F_SET_RECEIVER_FILE_WRITE, static_cast<int>(value), nullptr);
}

bool Module::getFileWrite() {
    return sendToReceiver<int>(F_GET_RECEIVER_FILE_WRITE);
}

void Module::setMasterFileWrite(bool value) {
    sendToReceiver(F_SET_RECEIVER_MASTER_FILE_WRITE, static_cast<int>(value), nullptr);
}

bool Module::getMasterFileWrite() {
    return sendToReceiver<int>(F_GET_RECEIVER_MASTER_FILE_WRITE);
}

void Module::setFileOverWrite(bool value) {
    sendToReceiver(F_SET_RECEIVER_OVERWRITE, static_cast<int>(value), nullptr);
}

bool Module::getFileOverWrite() {
    return sendToReceiver<int>(F_GET_RECEIVER_OVERWRITE);
}

int Module::getReceiverStreamingFrequency() {
    return sendToReceiver<int>(F_GET_RECEIVER_STREAMING_FREQUENCY);
}

void Module::setReceiverStreamingFrequency(int freq) {
    if (freq < 0) {
        throw RuntimeError("Invalid streaming frequency " +
                           std::to_string(freq));
    }
    sendToReceiver(F_SET_RECEIVER_STREAMING_FREQUENCY, freq, nullptr);
}

int Module::setReceiverStreamingTimer(int time_in_ms) {
    return sendToReceiver<int>(F_RECEIVER_STREAMING_TIMER, time_in_ms);
}

bool Module::getReceiverStreaming() {
    return sendToReceiver<int>(F_GET_RECEIVER_STREAMING);
}

void Module::setReceiverStreaming(bool enable) {
    sendToReceiver(F_SET_RECEIVER_STREAMING, static_cast<int>(enable), nullptr);
}

bool Module::enableTenGigabitEthernet(int value) {
    int retval = -1;
    LOG(logDEBUG1) << "Enabling / Disabling 10Gbe: " << value;
    sendToDetector(F_ENABLE_TEN_GIGA, value, retval);
    if (value != -1) {
        int stopRetval = -1;
        sendToDetectorStop(F_ENABLE_TEN_GIGA, value, stopRetval);
    }
    LOG(logDEBUG1) << "10Gbe: " << retval;
    value = retval;
    if (shm()->useReceiverFlag && value != -1) {
        int retval = -1;
        LOG(logDEBUG1) << "Sending 10Gbe enable to receiver: " << value;
        sendToReceiver(F_ENABLE_RECEIVER_TEN_GIGA, value, retval);
        LOG(logDEBUG1) << "Receiver 10Gbe enable: " << retval;
    }
    return static_cast<bool>(retval);
}

int Module::setReceiverFifoDepth(int n_frames) {
    int retval = -1;
    LOG(logDEBUG1) << "Sending Receiver Fifo Depth: " << n_frames;
    if (shm()->useReceiverFlag) {
        sendToReceiver(F_SET_RECEIVER_FIFO_DEPTH, n_frames, retval);
        LOG(logDEBUG1) << "Receiver Fifo Depth: " << retval;
    }
    return retval;
}

bool Module::getReceiverSilentMode() {
    return sendToReceiver<int>(F_GET_RECEIVER_SILENT_MODE);
}

void Module::setReceiverSilentMode(bool enable) {
    sendToReceiver(F_SET_RECEIVER_SILENT_MODE, static_cast<int>(enable),
                   nullptr);
}

void Module::restreamStopFromReceiver() {
    LOG(logDEBUG1) << "Restream stop dummy from Receiver via zmq";
    if (shm()->useReceiverFlag) {
        sendToReceiver(F_RESTREAM_STOP_FROM_RECEIVER, nullptr, nullptr);
    }
}

void Module::setPattern(const std::string &fname) {
    uint64_t word;
    uint64_t addr = 0;
    FILE *fd = fopen(fname.c_str(), "r");
    if (fd != nullptr) {
        while (fread(&word, sizeof(word), 1, fd) != 0U) {
            setPatternWord(addr, word); // TODO! (Erik) do we need to send
                                        // pattern in 64bit chunks?
            ++addr;
        }
        fclose(fd);
    } else {
        throw RuntimeError("Could not open file to set pattern");
    }
}

uint64_t Module::setPatternIOControl(uint64_t word) {
    LOG(logDEBUG1) << "Setting Pattern IO Control, word: 0x" << std::hex << word
                   << std::dec;
    return sendToDetector<uint64_t>(F_SET_PATTERN_IO_CONTROL, word);
}

uint64_t Module::setPatternClockControl(uint64_t word) {
    LOG(logDEBUG1) << "Setting Pattern Clock Control, word: 0x" << std::hex
                   << word << std::dec;
    return sendToDetector<uint64_t>(F_SET_PATTERN_CLOCK_CONTROL, word);
}

uint64_t Module::setPatternWord(int addr, uint64_t word) {
    uint64_t args[]{static_cast<uint64_t>(addr), word};

    LOG(logDEBUG1) << "Setting Pattern word, addr: 0x" << std::hex << addr
                   << ", word: 0x" << word << std::dec;
    return sendToDetector<uint64_t>(F_SET_PATTERN_WORD, args);
}

std::array<int, 2> Module::setPatternLoopAddresses(int level, int start,
                                                   int stop) {
    int args[]{level, start, stop};
    std::array<int, 2> retvals{};
    LOG(logDEBUG1) << "Setting Pat Loop Addresses, level: " << level
                   << ", start: " << start << ", stop: " << stop;
    sendToDetector(F_SET_PATTERN_LOOP_ADDRESSES, args, retvals);
    LOG(logDEBUG1) << "Set Pat Loop Addresses: " << retvals[0] << ", "
                   << retvals[1];
    return retvals;
}

int Module::setPatternLoopCycles(int level, int n) {
    int args[]{level, n};
    LOG(logDEBUG1) << "Setting Pat Loop cycles, level: " << level
                   << ",nloops: " << n;
    return sendToDetector<int>(F_SET_PATTERN_LOOP_CYCLES, args);
}

int Module::setPatternWaitAddr(int level, int addr) {
    int args[]{level, addr};
    LOG(logDEBUG1) << "Setting Pat Wait Addr, level: " << level << ", addr: 0x"
                   << std::hex << addr << std::dec;
    return sendToDetector<int>(F_SET_PATTERN_WAIT_ADDR, args);
}

uint64_t Module::setPatternWaitTime(int level, uint64_t t) {
    uint64_t args[]{static_cast<uint64_t>(level), t};
    return sendToDetector<uint64_t>(F_SET_PATTERN_WAIT_TIME, args);
}

void Module::setPatternMask(uint64_t mask) {
    LOG(logDEBUG1) << "Setting Pattern Mask " << std::hex << mask << std::dec;
    sendToDetector(F_SET_PATTERN_MASK, mask, nullptr);
}

uint64_t Module::getPatternMask() {
    return sendToDetector<uint64_t>(F_GET_PATTERN_MASK);
}

void Module::setPatternBitMask(uint64_t mask) {
    LOG(logDEBUG1) << "Setting Pattern Bit Mask " << std::hex << mask
                   << std::dec;
    sendToDetector(F_SET_PATTERN_BIT_MASK, mask, nullptr);
    LOG(logDEBUG1) << "Pattern Bit Mask successful";
}

uint64_t Module::getPatternBitMask() {
    return sendToDetector<uint64_t>(F_GET_PATTERN_BIT_MASK);
}

int Module::setLEDEnable(int enable) {
    return sendToDetector<int>(F_LED, enable);
}

void Module::setDigitalIODelay(uint64_t pinMask, int delay) {
    uint64_t args[]{pinMask, static_cast<uint64_t>(delay)};
    LOG(logDEBUG1) << "Sending Digital IO Delay, pin mask: " << std::hex
                   << args[0] << ", delay: " << std::dec << args[1] << " ps";
    sendToDetector(F_DIGITAL_IO_DELAY, args, nullptr);
    LOG(logDEBUG1) << "Digital IO Delay successful";
}

int Module::getClockFrequency(int clkIndex) {
    return sendToDetector<int>(F_GET_CLOCK_FREQUENCY, clkIndex);
}

void Module::setClockFrequency(int clkIndex, int value) {
    int args[]{clkIndex, value};
    LOG(logDEBUG1) << "Setting Clock " << clkIndex << " frequency to " << value;
    sendToDetector(F_SET_CLOCK_FREQUENCY, args, nullptr);
}

int Module::getClockPhase(int clkIndex, bool inDegrees) {
    int args[]{clkIndex, static_cast<int>(inDegrees)};
    int retval = -1;
    LOG(logDEBUG1) << "Getting Clock " << clkIndex << " phase "
                   << (inDegrees ? "in degrees" : "");
    sendToDetector(F_GET_CLOCK_PHASE, args, retval);
    LOG(logDEBUG1) << "Clock " << clkIndex << " frequency: " << retval
                   << (inDegrees ? "degrees" : "");
    return retval;
}

void Module::setClockPhase(int clkIndex, int value, bool inDegrees) {
    int args[]{clkIndex, value, static_cast<int>(inDegrees)};
    LOG(logDEBUG1) << "Setting Clock " << clkIndex << " phase to " << value
                   << (inDegrees ? "degrees" : "");
    sendToDetector(F_SET_CLOCK_PHASE, args, nullptr);
}

int Module::getMaxClockPhaseShift(int clkIndex) {
    return sendToDetector<int>(F_GET_MAX_CLOCK_PHASE_SHIFT, clkIndex);
}

int Module::getClockDivider(int clkIndex) {
    return sendToDetector<int>(F_GET_CLOCK_DIVIDER, clkIndex);
}

void Module::setClockDivider(int clkIndex, int value) {
    int args[]{clkIndex, value};
    LOG(logDEBUG1) << "Setting Clock " << clkIndex << " divider to " << value;
    sendToDetector(F_SET_CLOCK_DIVIDER, args, nullptr);
}

int Module::getPipeline(int clkIndex) {
    return sendToDetector<int>(F_GET_PIPELINE, clkIndex);
}

void Module::setPipeline(int clkIndex, int value) {
    int args[]{clkIndex, value};
    LOG(logDEBUG1) << "Setting Clock " << clkIndex << " pipeline to " << value;
    sendToDetector(F_SET_PIPELINE, args, nullptr);
}

void Module::setCounterMask(uint32_t countermask) {
    LOG(logDEBUG1) << "Setting Counter mask to " << countermask;
    sendToDetector(F_SET_COUNTER_MASK, countermask, nullptr);
    if (shm()->useReceiverFlag) {
        int ncounters = __builtin_popcount(countermask);
        LOG(logDEBUG1) << "Sending Reciver #counters: " << ncounters;
        sendToReceiver(F_RECEIVER_SET_NUM_COUNTERS, ncounters, nullptr);
    }
}

uint32_t Module::getCounterMask() {
    return sendToDetector<uint32_t>(F_GET_COUNTER_MASK);
}

sls_detector_module Module::interpolateTrim(sls_detector_module *a,
                                            sls_detector_module *b,
                                            const int energy, const int e1,
                                            const int e2, int tb) {

    // only implemented for eiger currently (in terms of which dacs)
    if (shm()->myDetectorType != EIGER) {
        throw NotImplementedError(
            "Interpolation of Trim values not implemented for this detector!");
    }

    sls_detector_module myMod{shm()->myDetectorType};
    enum eiger_DacIndex {
        E_SVP,
        E_VTR,
        E_VRF,
        E_VRS,
        E_SVN,
        E_VTGSTV,
        E_VCMP_LL,
        E_VCMP_LR,
        E_CAL,
        E_VCMP_RL,
        E_RXB_RB,
        E_RXB_LB,
        E_VCMP_RR,
        E_VCP,
        E_VCN,
        E_VIS
    };

    // Copy other dacs
    int dacs_to_copy[] = {E_SVP,    E_VTR,    E_SVN, E_VTGSTV,
                          E_RXB_RB, E_RXB_LB, E_VCN, E_VIS};
    int num_dacs_to_copy = sizeof(dacs_to_copy) / sizeof(dacs_to_copy[0]);
    for (int i = 0; i < num_dacs_to_copy; ++i) {
        if (a->dacs[dacs_to_copy[i]] != b->dacs[dacs_to_copy[i]]) {
            throw RuntimeError("Interpolate module: dacs different");
        }
        myMod.dacs[dacs_to_copy[i]] = a->dacs[dacs_to_copy[i]];
    }

    // Copy irrelevant dacs (without failing): CAL
    if (a->dacs[E_CAL] != b->dacs[E_CAL]) {
        LOG(logWARNING) << "DAC CAL differs in both energies ("
                        << a->dacs[E_CAL] << "," << b->dacs[E_CAL]
                        << ")!\nTaking first: " << a->dacs[E_CAL];
    }
    myMod.dacs[E_CAL] = a->dacs[E_CAL];

    // Interpolate vrf, vcmp, vcp
    int dacs_to_interpolate[] = {E_VRF,     E_VCMP_LL, E_VCMP_LR, E_VCMP_RL,
                                 E_VCMP_RR, E_VCP,     E_VRS};
    int num_dacs_to_interpolate =
        sizeof(dacs_to_interpolate) / sizeof(dacs_to_interpolate[0]);
    for (int i = 0; i < num_dacs_to_interpolate; ++i) {
        myMod.dacs[dacs_to_interpolate[i]] =
            linearInterpolation(energy, e1, e2, a->dacs[dacs_to_interpolate[i]],
                                b->dacs[dacs_to_interpolate[i]]);
    }
    // Interpolate all trimbits
    if (tb != 0) {
        for (int i = 0; i < myMod.nchan; ++i) {
            myMod.chanregs[i] = linearInterpolation(
                energy, e1, e2, a->chanregs[i], b->chanregs[i]);
        }
    }
    return myMod;
}

sls_detector_module Module::readSettingsFile(const std::string &fname, int tb) {
    LOG(logDEBUG1) << "Read settings file " << fname;
    sls_detector_module myMod(shm()->myDetectorType);
    // open file
    std::ifstream infile;
    if (shm()->myDetectorType == EIGER || shm()->myDetectorType == MYTHEN3) {
        infile.open(fname.c_str(), std::ifstream::binary);
    } else {
        infile.open(fname.c_str(), std::ios_base::in);
    }
    if (!infile.is_open()) {
        throw RuntimeError("Could not open settings file: " + fname);
    }

    // eiger
    if (shm()->myDetectorType == EIGER) {
        infile.read(reinterpret_cast<char *>(myMod.dacs),
                    sizeof(int) * (myMod.ndac));
        infile.read(reinterpret_cast<char *>(&myMod.iodelay),
                    sizeof(myMod.iodelay));
        infile.read(reinterpret_cast<char *>(&myMod.tau), sizeof(myMod.tau));
        if (tb != 0) {
            infile.read(reinterpret_cast<char *>(myMod.chanregs),
                        sizeof(int) * (myMod.nchan));
        }
        if (!infile) {
            throw RuntimeError("readSettingsFile: Could not load all values "
                               "for settings for " +
                               fname);
        }
        for (int i = 0; i < myMod.ndac; ++i) {
            LOG(logDEBUG1) << "dac " << i << ":" << myMod.dacs[i];
        }
        LOG(logDEBUG1) << "iodelay:" << myMod.iodelay;
        LOG(logDEBUG1) << "tau:" << myMod.tau;
    }

    // mythen3 (dacs, trimbits)
    else if (shm()->myDetectorType == MYTHEN3) {
        infile.read(reinterpret_cast<char *>(myMod.dacs),
                    sizeof(int) * (myMod.ndac));
        infile.read(reinterpret_cast<char *>(myMod.chanregs),
                    sizeof(int) * (myMod.nchan));

        if (!infile) {
            throw RuntimeError("readSettingsFile: Could not load all values "
                               "for settings for " +
                               fname);
        }
        for (int i = 0; i < myMod.ndac; ++i) {
            LOG(logDEBUG1) << "dac " << i << ":" << myMod.dacs[i];
        }
    }

    // gotthard, jungfrau
    else {
        auto names = getSettingsFileDacNames();
        size_t idac = 0;
        std::string str;
        while (infile.good()) {
            getline(infile, str);
            if (str.empty()) {
                break;
            }
            LOG(logDEBUG1) << str;
            std::string sargname;
            int ival = 0;
            std::istringstream ssstr(str);
            ssstr >> sargname >> ival;
            bool found = false;
            for (size_t i = 0; i < names.size(); ++i) {
                if (sargname == names[i]) {
                    myMod.dacs[i] = ival;
                    found = true;
                    LOG(logDEBUG1) << names[i] << "(" << i << "): " << ival;
                    ++idac;
                }
            }
            if (!found) {
                throw RuntimeError("readSettingsFile: Unknown dac: " +
                                   sargname);
            }
        }
        // not all read
        if (idac != names.size()) {
            throw RuntimeError("Could read only " + std::to_string(idac) +
                               " dacs. Expected " +
                               std::to_string(names.size()) + " dacs");
        }
    }
    LOG(logINFO) << "Settings file loaded: " << fname.c_str();
    return myMod;
}

void Module::writeSettingsFile(const std::string &fname,
                               sls_detector_module &mod) {
    LOG(logDEBUG1) << "Write settings file " << fname;

    std::ofstream outfile;
    if (shm()->myDetectorType == EIGER) {
        outfile.open(fname.c_str(), std::ofstream::binary);
    } else {
        outfile.open(fname.c_str(), std::ios_base::out);
    }
    if (!outfile.is_open()) {
        throw RuntimeError("Could not open settings file for writing: " +
                           fname);
    }
    if (shm()->myDetectorType == EIGER) {
        for (int i = 0; i < mod.ndac; ++i) {
            LOG(logINFO) << "dac " << i << ":" << mod.dacs[i];
        }
        LOG(logINFO) << "iodelay: " << mod.iodelay;
        LOG(logINFO) << "tau: " << mod.tau;

        outfile.write(reinterpret_cast<char *>(mod.dacs),
                      sizeof(int) * (mod.ndac));
        outfile.write(reinterpret_cast<char *>(&mod.iodelay),
                      sizeof(mod.iodelay));
        outfile.write(reinterpret_cast<char *>(&mod.tau), sizeof(mod.tau));
        outfile.write(reinterpret_cast<char *>(mod.chanregs),
                      sizeof(int) * (mod.nchan));
    }
    // gotthard, jungfrau
    else {
        auto names = getSettingsFileDacNames();
        for (int i = 0; i < mod.ndac; ++i) {
            LOG(logDEBUG1) << "dac " << i << ": " << mod.dacs[i];
            outfile << names[i] << " " << mod.dacs[i] << std::endl;
        }
    }
}

std::vector<std::string> Module::getSettingsFileDacNames() {
    switch (shm()->myDetectorType) {
    case GOTTHARD:
        return {"Vref",  "VcascN", "VcascP",    "Vout",
                "Vcasc", "Vin",    "Vref_comp", "Vib_test"};
    case JUNGFRAU:
        return {"VDAC0",  "VDAC1",  "VDAC2",  "VDAC3", "VDAC4",  "VDAC5",
                "VDAC6",  "VDAC7",  "VDAC8",  "VDAC9", "VDAC10", "VDAC11",
                "VDAC12", "VDAC13", "VDAC14", "VDAC15"};
    default:
        throw RuntimeError(
            "Unknown detector type - unknown format for settings file");
    }
}

} // namespace sls