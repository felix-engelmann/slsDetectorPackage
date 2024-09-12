// SPDX-License-Identifier: LGPL-3.0-or-other
// Copyright (C) 2021 Contributors to the SLS Detector Package
/* Creates the slsFrameSynchronizer for running multiple receivers in different
 * threads form a single binary that will spit out zmq streams without
 * reconstructing image */
#include "sls/Receiver.h"
#include "sls/ToString.h"
#include "sls/container_utils.h"
#include "sls/logger.h"
#include "sls/sls_detector_defs.h"

#include <csignal> //SIGINT
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ostream>
#include <semaphore.h>
#include <sys/wait.h> //wait
#include <thread>
#include <mutex>
#include <unistd.h>

#include <vector>
#include <set>
#include <zmq.h>

// gettid added in glibc 2.30
#if __GLIBC__ == 2 && __GLIBC_MINOR__ < 30
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif

/** Define Colors to print data call back in different colors for different
 * recievers */
#define PRINT_IN_COLOR(c, f, ...)                                              \
    printf("\033[%dm" f RESET, 30 + c + 1, ##__VA_ARGS__)

std::vector<std::thread> threads;
std::vector<sem_t *> semaphores;

/**
 * Control+C Interrupt Handler
 * to let all the processes know to exit properly
 */
void sigInterruptHandler(int p) {
    for (size_t i = 0; i != semaphores.size(); ++i) {
        sem_post(semaphores[i]);
    }
}

/**
 * prints usage of this example program
 */
std::string getHelpMessage() {
    return std::string(
        "\n\nUsage:\n"
        "./slsFrameSynchronizer(detReceiver) [start_tcp_port (non-zero and 16 "
        "bit)] [num_receivers] [optional: 1 for call back (print frame header "
        "for debugging), 0 for none (default)]\n\n");
}

void zmq_free (void *data, void *hint)
{
    free (data);
}


struct Status{ 
  bool terminate = false;
  unsigned long num_receivers;

  sem_t available;

  std::mutex mtx;
  
  std::vector<zmq_msg_t*> headers;
  std::map<unsigned int, std::map<long unsigned int, std::vector<zmq_msg_t*> > > frames;
  
};

void print_frames(const std::map<unsigned int, std::map<long unsigned int, std::vector<zmq_msg_t*> > > &frames) {
    for (const auto& outer_pair : frames) {
        unsigned int udpPort = outer_pair.first;
        const auto& trigger_map = outer_pair.second;

        std::cout << "UDP port: " << udpPort << std::endl;

        for (const auto& inner_pair : trigger_map) {
            long unsigned int acqIndex = inner_pair.first;
            const auto& msg_vector = inner_pair.second;

            std::cout << "  acq index: " << acqIndex << std::endl;
            std::cout << "    zmq_msg_t* Vector: ";

            // Iterate over the vector of zmq_msg_t* and print each message pointer
            for (const auto& msg : msg_vector) {
                std::cout << " a frame " << msg ;  // Print a space between each pointer
            }

            std::cout << std::endl;
        }
    }
}

std::set<long unsigned int> find_keys(const std::map<unsigned int, std::map<long unsigned int, std::vector<zmq_msg_t*> > >& maps) {
    std::set<long unsigned int> all_keys;  // Set to collect all unique keys across all maps
    std::set<long unsigned int> valid_keys; // Set to store final valid keys
    
    // If no maps are provided, return empty set
    if (maps.empty()) {
        return valid_keys;
    }

    // Collect all unique keys from all maps
    std::cout << "Collecting all unique keys from the maps:\n";
    for (const auto& [port, trigger_map] : maps) {
        std::cout << "Map " << port << ": ";
        for (const auto& [idx, msgs] : trigger_map) {
            all_keys.insert(idx);
            std::cout << idx << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "All unique keys collected: ";
    for (const auto& key : all_keys) {
        std::cout << key << " ";
    }
    std::cout << "\n\n";

    // Now check each key against all maps
    for (const auto& key : all_keys) {
        std::cout << "Checking key: " << key << std::endl;
        bool is_valid = true;
        for (const auto& [port, map] : maps) {
            auto it = map.find(key);
            if (it != map.end()) {
                std::cout << "  Key " << key << " found in map " << port << std::endl;
            } else {
                // Key is missing, check if the map has a larger key
                std::cout << "  Key " << key << " missing in map " << port;
                auto upper_it = map.upper_bound(key);
                if (upper_it != map.end()) {
                    std::cout << ", but found larger key: " << upper_it->first << std::endl;
                } else {
                    std::cout << ", no larger key found. Key " << key << " is invalid.\n";
                    is_valid = false;
                    break;
                }
            }
        }

        if (is_valid) {
            std::cout << "  Key " << key << " is valid.\n\n";
            valid_keys.insert(key);
        } else {
            std::cout << "  Key " << key << " is not valid.\n\n";
        }
    }

    return valid_keys;
}

void Correlate(Status *stat) {
    bool starting = true;
    std::vector<unsigned int> ports;
    while (!stat->terminate) {
        sem_wait(&(stat->available));
        std::cout << "Correlate cache" << std::endl;
        {
            std::lock_guard<std::mutex> lock(stat->mtx);
            if (starting) {
                if (stat->headers.size() == stat->num_receivers) {
                    std::cout << "got all start messages" << std::endl;
                    starting = false;
                    ports.clear();

                }
            }
            else {
                std::cout << "sending data, common keys" << std::endl;
                //print_frames(stat->frames);
                auto common_keys = find_keys(stat->frames);
                for (const auto& key : common_keys) {
                    std::cout << key << " ";
                }
                std::cout << "\n\n";
            }
        }
    }
}

/**
 * Start Acquisition Call back (slsFrameSynchronizer writes data if file write
 * enabled) if registerCallBackRawDataReady or
 * registerCallBackRawDataModifyReady registered, users get data
 */
int StartAcq(const slsDetectorDefs::startCallbackHeader callbackHeader,
             void *objectPointer) {
    LOG(sls::logINFOBLUE) << "#### Start Acquisition:"
                          << "\n\t["
                          << "\n\tUDP Port : "
                          << sls::ToString(callbackHeader.udpPort)
                          << "\n\tDynamic Range : "
                          << callbackHeader.dynamicRange
                          << "\n\tDetector Shape : "
                          << sls::ToString(callbackHeader.detectorShape)
                          << "\n\tImage Size : " << callbackHeader.imageSize
                          << "\n\tFile Path : " << callbackHeader.filePath
                          << "\n\tFile Name : " << callbackHeader.fileName
                          << "\n\tFile Index : " << callbackHeader.fileIndex
                          << "\n\tQuad Enable : " << callbackHeader.quad
                          << "\n\t]";
    Status* stat = static_cast<Status*>(objectPointer);

    std::ostringstream oss;
    oss << "{\"htype\":\"header\""
        << ", \"udpPort\":" << sls::ToString(callbackHeader.udpPort)
        << ", \"filePath\":" << callbackHeader.filePath
        << "\"}\n";
    
    std::string message = oss.str();
    int length = message.length();
    char* hdata = new char[length];

    memcpy(hdata, message.c_str(), length);
    zmq_msg_t *hmsg = new zmq_msg_t;
    zmq_msg_init_data (hmsg, hdata, length, zmq_free, NULL);

    {
        std::lock_guard<std::mutex> lock(stat->mtx);
        stat->headers.push_back(hmsg);
        for(int port: callbackHeader.udpPort) {
            std::cout << "clear cache for stream" << port << std::endl;
            for (auto& pair : stat->frames[port]) {
                std::cout << "clear data" << pair.first << std::endl;
                for (auto msg : pair.second) {
                    zmq_msg_close(msg);
                    free(msg);
                }
            }
            stat->frames[port].clear();
        }
    }
    sem_post(&stat->available);
    return 0;
}

/** Acquisition Finished Call back */
void AcquisitionFinished(
    const slsDetectorDefs::endCallbackHeader callbackHeader,
    void *objectPointer) {
    LOG(sls::logINFOBLUE) << "#### AcquisitionFinished:"
                          << "\n\t["
                          << "\n\tUDP Port : "
                          << sls::ToString(callbackHeader.udpPort)
                          << "\n\tComplete Frames : "
                          << sls::ToString(callbackHeader.completeFrames)
                          << "\n\tLast Frame Index : "
                          << sls::ToString(callbackHeader.lastFrameIndex)
                          << "\n\t]";
}

/**
 * Get Receiver Data Call back
 * Prints in different colors(for each receiver process) the different headers
 * for each image call back.
 */
void GetData(slsDetectorDefs::sls_receiver_header &header,
             slsDetectorDefs::dataCallbackHeader callbackHeader,
             char *dataPointer, size_t &imageSize, void *objectPointer) {

    slsDetectorDefs::sls_detector_header detectorHeader = header.detHeader;

    PRINT_IN_COLOR(
        (callbackHeader.udpPort % 10),
        "#### GetData: "
        "\n\tCallback Header: "
        "\n\t["
        "\n\tUDP Port: %u"
        "\n\tShape: [%u, %u]"
        "\n\tAcq Index : %lu"
        "\n\tFrame Index :%lu"
        "\n\tProgress : %.2f%%"
        "\n\tCompelte Image :%s"
        "\n\tFlip Rows :%s"
        "\n\tAdditional Json Header : %s"
        "\n\t]"
        "\n\ttReceiver Header: "
        "\n\t["
        "\n\tFrame Number : %lu"
        "\n\tExposure Length :%u"
        "\n\tPackets Caught :%u"
        "\n\tDetector Specific 1: %lu"
        "\n\tTimestamp : %lu"
        "\n\tModule Id :%u"
        "\n\tRow : %u"
        "\n\tColumn :%u"
        "\n\tDetector Specific 2 : %u"
        "\n\tDetector Specific 3 : %u"
        "\n\tDetector Specific 4 : %u"
        "\n\tDetector Type : %s"
        "\n\tVersion: %u"
        "\n\t]"
        "\n\tFirst Byte Data: 0x%x"
        "\n\tImage Size: %zu\n\n",
        callbackHeader.udpPort, callbackHeader.shape.x, callbackHeader.shape.y,
        callbackHeader.acqIndex, callbackHeader.frameIndex,
        callbackHeader.progress,
        sls::ToString(callbackHeader.completeImage).c_str(),
        sls::ToString(callbackHeader.flipRows).c_str(),
        sls::ToString(callbackHeader.addJsonHeader).c_str(),
        detectorHeader.frameNumber, detectorHeader.expLength,
        detectorHeader.packetNumber, detectorHeader.detSpec1,
        detectorHeader.timestamp, detectorHeader.modId, detectorHeader.row,
        detectorHeader.column, detectorHeader.detSpec2, detectorHeader.detSpec3,
        detectorHeader.detSpec4, sls::ToString(detectorHeader.detType).c_str(),
        detectorHeader.version,
        // header->packetsMask.to_string().c_str(),
        ((uint8_t)(*((uint8_t *)(dataPointer)))), imageSize);

    Status* stat = static_cast<Status*>(objectPointer);

    std::ostringstream oss;
    oss << "{\"htype\":\"module\""
        << ", \"port\":" << callbackHeader.udpPort
        << "\"}\n";
    
    std::string message = oss.str();
    int length = message.length();

    char* hdata = new char[length];

    memcpy(hdata, message.c_str(), length);
    std::cout << callbackHeader.udpPort << ":creating json part" << std::endl;    
    zmq_msg_t *hmsg = new zmq_msg_t;

    zmq_msg_init_data (hmsg, hdata, length, zmq_free, NULL);

    std::cout << callbackHeader.udpPort << "created header frame" << std::endl;
    //zmq_msg_init_buffer (&hmsg, message, length);

    char* data = new char[imageSize];

    std::cout << callbackHeader.udpPort << "allocated new buffer" << std::endl;

    //printf("data pointer %x, data %x\n", dataPointer, )
    memcpy(data, dataPointer, imageSize);

    std::cout << callbackHeader.udpPort << "copied buffer" << std::endl;
    zmq_msg_t *msg = new zmq_msg_t;
    zmq_msg_init_data (msg, data, imageSize, zmq_free, NULL);

    std::cout << callbackHeader.udpPort << "copied data to data frame" << std::endl;

    //std::tuple<zmq_msg_t *, zmq_msg_t *> msgTuple(&hmsg, &msg);

    {
      std::cout << callbackHeader.udpPort << "getting lock" << std::endl;
      std::lock_guard<std::mutex> lock(stat->mtx);
      //stat->cache[0][(long unsigned int)42] = nullptr;
      std::cout << callbackHeader.udpPort << "put data in cache" << std::endl;
      stat->frames[callbackHeader.udpPort][header.detHeader.frameNumber].push_back(hmsg);
      stat->frames[callbackHeader.udpPort][header.detHeader.frameNumber].push_back(msg);
    }
    std::cout << callbackHeader.udpPort << "call not correlate" << std::endl;
    sem_post(&stat->available);

    // if data is modified, eg ROI and size is reduced
    //imageSize = 26000;
}

/**
 * Example of main program using the Receiver class
 *
 * - Defines in file for:
 *  	- Default Number of receivers is 1
 *  	- Default Start TCP port is 1954
 */
int main(int argc, char *argv[]) {

    /**	- set default values */
    int numReceivers = 1;
    uint16_t startTCPPort = 1954;
    int withCallback = 0;

    /**	- get number of receivers and start tcp port from command line
     * arguments */
    try {
        if (argc == 3 || argc == 4) {
            startTCPPort = sls::StringTo<uint16_t>(argv[1]);
            if (startTCPPort == 0) {
                throw;
            }
            numReceivers = std::stoi(argv[2]);
            if (argc == 4) {
                withCallback = std::stoi(argv[3]);
            }
        } else
            throw;
    } catch (...) {
        throw std::runtime_error(getHelpMessage());
    }

    cprintf(RESET, "Number of Receivers: %d\n", numReceivers);
    cprintf(RESET, "Start TCP Port: %hu\n", startTCPPort);
    cprintf(RESET, "Callback Enable: %d\n", withCallback);

    /** - Catch signal SIGINT to close files and call destructors properly */
    struct sigaction sa;
    sa.sa_flags = 0;                     // no flags
    sa.sa_handler = sigInterruptHandler; // handler function
    sigemptyset(&sa.sa_mask); // dont block additional signals during invocation
                              // of handler
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        cprintf(RED, "Could not set handler function for SIGINT\n");
    }

    /** - Ignore SIG_PIPE, prevents global signal handler, handle locally,
       instead of a server crashing due to client crash when writing, it just
       gives error */
    struct sigaction asa;
    asa.sa_flags = 0;          // no flags
    asa.sa_handler = SIG_IGN;  // handler function
    sigemptyset(&asa.sa_mask); // dont block additional signals during
                               // invocation of handler
    if (sigaction(SIGPIPE, &asa, nullptr) == -1) {
        cprintf(RED, "Could not set handler function for SIGPIPE\n");
    }

    Status stat{false, numReceivers};

    sem_init(&stat.available, 0, 0);

    void* user_data = static_cast<void *>(&stat);
    
    std::thread combinerThread(Correlate, &stat);

    /** - loop over number of receivers */
    for (int i = 0; i != numReceivers; ++i) {

        sem_t *semaphore = new sem_t;
        sem_init(semaphore, 1, 0);
        semaphores.push_back(semaphore);
        threads.emplace_back([semaphore, i, startTCPPort, withCallback,
                              numReceivers, user_data]() {
            sls::Receiver receiver(startTCPPort + i);

            receiver.registerCallBackStartAcquisition(StartAcq, user_data);
            receiver.registerCallBackAcquisitionFinished(
                AcquisitionFinished, user_data);
            receiver.registerCallBackRawDataReady(GetData, user_data);
            /**	- as long as no Ctrl+C */
            sem_wait(semaphore);
            sem_destroy(semaphore);
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    std::cout << "Terminate Combiner" << std::endl;
    stat.terminate = true;
    sem_post(&stat.available);
    combinerThread.join();
    sem_destroy(&stat.available);

    std::cout << "Goodbye!\n";
    return 0;
}
