#include "CmdProxy.h"
#include "Detector.h"
#include "catch.hpp"
#include "sls_detector_defs.h"
#include <sstream>

#include "tests/globals.h"
#include "versionAPI.h"

using sls::CmdProxy;
using sls::Detector;
using test::GET;
using test::PUT;

// TEST_CASE("tengiga", "[.cmd][.eiger][.ctb]") {
//     if (test::type == slsDetectorDefs::EIGER || test::type ==
//     slsDetectorDefs::CHIPTESTBOARD) {
//         {
//             REQUIRE_NOTHROW(multiSlsDetectorClient("tengiga 1", PUT));
//             std::ostringstream oss;
//             REQUIRE_NOTHROW(multiSlsDetectorClient("0:tengiga", GET, nullptr,
//             oss)); REQUIRE(oss.str() == "tengiga 1\n");
//         }
//         {
//             REQUIRE_NOTHROW(multiSlsDetectorClient("tengiga 0", PUT));
//             std::ostringstream oss;
//             REQUIRE_NOTHROW(multiSlsDetectorClient("0:tengiga", GET, nullptr,
//             oss)); REQUIRE(oss.str() == "tengiga 0\n");
//         }
//     } else {
//         REQUIRE_THROWS(multiSlsDetectorClient("tengiga", GET));
//     }
// }

TEST_CASE("quad", "[.cmd]") {
    // TODO! set and get once available in virtual detector
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER && det.size() == 1) {
        // Quad only works with a single half module EIGER
        std::ostringstream oss;
        proxy.Call("quad", {}, -1, GET, oss);
        REQUIRE(oss.str() == "quad 0\n");
    } else {
        REQUIRE_THROWS(proxy.Call("quad", {}, -1, GET));
    }
}

void test_dac(defs::dacIndex index, const std::string &dacname, int dacvalue) {
    Detector det;
    CmdProxy proxy(&det);
    std::ostringstream oss_set, oss_get;
    auto dacstr = std::to_string(dacvalue);
    auto previous = det.getDAC(index, false);
    proxy.Call(dacname, {dacstr}, -1, PUT, oss_set);
    REQUIRE(oss_set.str() == dacname + " " + dacstr + "\n");
    proxy.Call(dacname, {}, -1, GET, oss_get);
    REQUIRE(oss_set.str() == dacname + " " + dacstr + "\n");
    // Reset all dacs to previous value
    for (int i = 0; i != det.size(); ++i) {
        det.setDAC(index, previous[i], false, {i});
    }
}

TEST_CASE("Setting and reading back EIGER dacs", "[.cmd]") {
    // vsvp, vtr, vrf, vrs, vsvn, vtgstv, vcmp_ll, vcmp_lr, vcal, vcmp_rl,
    // rxb_rb, rxb_lb, vcmp_rr, vcp, vcn, vis, vthreshold
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        SECTION("vsvp") { test_dac(defs::SVP, "vsvp", 5); }
        SECTION("vtr") { test_dac(defs::VRF, "vtr", 1200); }
        SECTION("vrf") { test_dac(defs::VRF, "vrf", 1500); }
        SECTION("vrs") { test_dac(defs::VRF, "vrs", 1510); }
        SECTION("vsvn") { test_dac(defs::SVN, "vsvn", 3800); }
        SECTION("vtgstv") { test_dac(defs::VTGSTV, "vtgstv", 2550); }
        SECTION("vcmp_ll") { test_dac(defs::VCMP_LL, "vcmp_ll", 1400); }
        SECTION("vcmp_lr") { test_dac(defs::VCMP_LR, "vcmp_lr", 1400); }
        SECTION("vcal") { test_dac(defs::CAL, "vcal", 1400); }
        SECTION("vcmp_rl") { test_dac(defs::VCMP_RL, "vcmp_rl", 1400); }
        SECTION("rxb_rb") { test_dac(defs::RXB_RB, "rxb_rb", 1400); }
        SECTION("rxb_lb") { test_dac(defs::RXB_LB, "rxb_lb", 1400); }
        SECTION("vcmp_rr") { test_dac(defs::VCMP_RR, "vcmp_rr", 1400); }
        SECTION("vcp") { test_dac(defs::VCP, "vcp", 1400); }
        SECTION("vcn") { test_dac(defs::VCN, "vcn", 1400); }
        SECTION("vis") { test_dac(defs::VIS, "vis", 1400); }
        SECTION("iodelay") { test_dac(defs::IO_DELAY, "iodelay", 1400); }
        SECTION("vthreshold") {
            // Read out individual vcmp to be able to reset after
            // the test is done
            auto vcmp_ll = det.getDAC(defs::VCMP_LL, false);
            auto vcmp_lr = det.getDAC(defs::VCMP_LR, false);
            auto vcmp_rl = det.getDAC(defs::VCMP_RL, false);
            auto vcmp_rr = det.getDAC(defs::VCMP_RR, false);
            auto vcp = det.getDAC(defs::VCP, false);

            {
                std::ostringstream oss;
                proxy.Call("vthreshold", {"1234"}, -1, PUT, oss);
                REQUIRE(oss.str() == "vthreshold 1234\n");
            }
            {
                std::ostringstream oss;
                proxy.Call("vthreshold", {}, -1, GET, oss);
                REQUIRE(oss.str() == "vthreshold 1234\n");
            }

            // Reset dacs after test
            for (int i = 0; i != det.size(); ++i) {
                det.setDAC(defs::VCMP_LL, vcmp_ll[i], false, {i});
                det.setDAC(defs::VCMP_LR, vcmp_ll[i], false, {i});
                det.setDAC(defs::VCMP_RL, vcmp_ll[i], false, {i});
                det.setDAC(defs::VCMP_RR, vcmp_ll[i], false, {i});
                det.setDAC(defs::VCP, vcp[i], false, {i});
            }
        }
    }
}

// TEST_CASE("trigger", "[.cmd]") {
//     Detector det;
//     CmdProxy proxy(&det);
//     auto det_type = det.getDetectorType().squash();
//     if (det_type != defs::EIGER) {
//         proxy.Call("trigger", {}, -1, PUT);
//     } else {

//         {
//             std::ostringstream oss;
//             proxy.Call("timing", {"trigger"}, -1, PUT, oss);
//             REQUIRE(oss.str() == "timing trigger\n");
//         }
//         auto startingfnum = det.getStartingFrameNumber().tsquash(
//             "inconsistent frame nr in test");
//         det.startDetector();

//         {
//             std::ostringstream oss;
//             proxy.Call("trigger", {}, -1, PUT, oss);
//             REQUIRE(oss.str() == "trigger successful\n");
//         }


//         auto currentfnum = det.getStartingFrameNumber().tsquash(
//             "inconsistent frame nr in test");

//         REQUIRE(startingfnum +1 == currentfnum);
//         det.stopDetector();
//         {
//             std::ostringstream oss;
//             proxy.Call("timing", {"auto"}, -1, PUT, oss);
//             REQUIRE(oss.str() == "timing auto\n");
//         }
//     }
    //     if(test::type != slsDetectorDefs::EIGER) {
    //         REQUIRE_THROWS(multiSlsDetectorClient("trigger", PUT));
    //     } else {
    //         // trigger
    //         {
    //             std::ostringstream oss;
    //             REQUIRE_NOTHROW(multiSlsDetectorClient("timing trigger", PUT,
    //             nullptr, oss)); REQUIRE(oss.str() == "timing trigger\n");
    //         }
    //         int startingfnum = 0;
    //         {
    //             std::ostringstream oss;
    //             REQUIRE_NOTHROW(multiSlsDetectorClient("startingfnum", GET,
    //             nullptr, oss)); std::string s = (oss.str()).erase (0,
    //             strlen("startingfnum ")); startingfnum = std::stoi(s);
    //         }
    //         {
    //             std::ostringstream oss;
    //             REQUIRE_NOTHROW(multiSlsDetectorClient("start", PUT, nullptr,
    //             oss)); REQUIRE(oss.str() == "start successful\n");
    //         }
    //         {
    //             std::ostringstream oss;
    //             REQUIRE_NOTHROW(multiSlsDetectorClient("status", GET,
    //             nullptr, oss)); REQUIRE(oss.str() != "status idle\n");
    //             REQUIRE(oss.str()
    //             != "status stopped\n");
    //         }
    //         {
    //             std::ostringstream oss;
    //             REQUIRE_NOTHROW(multiSlsDetectorClient("trigger", PUT,
    //             nullptr, oss)); REQUIRE(oss.str() == "trigger successful\n");
    //         }
    //         REQUIRE_NOTHROW(multiSlsDetectorClient("stop", PUT));
    //         int currentfnum = 0;
    //         {
    //             std::ostringstream oss;
    //             REQUIRE_NOTHROW(multiSlsDetectorClient("startingfnum", GET,
    //             nullptr, oss)); std::string s = (oss.str()).erase (0,
    //             strlen("startingfnum ")); currentfnum = std::stoi(s);
    //         }
    //         REQUIRE((startingfnum + 1) == currentfnum);

    //         REQUIRE_NOTHROW(multiSlsDetectorClient("timing auto", PUT));
    //     }
// }
