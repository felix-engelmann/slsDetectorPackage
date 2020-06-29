#include "CmdProxy.h"
#include "Detector.h"
#include "catch.hpp"
#include "sls_detector_defs.h"
#include <array>
#include <sstream>
#include <thread>

#include "test-CmdProxy-global.h"
#include "tests/globals.h"
#include "versionAPI.h"

using sls::CmdProxy;
using sls::Detector;
using test::GET;
using test::PUT;

/** temperature */

TEST_CASE("temp_fpgaext", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        REQUIRE_NOTHROW(proxy.Call("temp_fpgaext", {}, -1, GET));
        std::ostringstream oss;
        REQUIRE_NOTHROW(proxy.Call("temp_fpgaext", {}, 0, GET, oss));
        std::string s = (oss.str()).erase(0, strlen("temp_fpgaext "));
        REQUIRE(std::stoi(s) != -1);
    } else {
        REQUIRE_THROWS(proxy.Call("temp_fpgaext", {}, -1, GET));
    }
}

TEST_CASE("temp_10ge", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        REQUIRE_NOTHROW(proxy.Call("temp_10ge", {}, -1, GET));
        std::ostringstream oss;
        REQUIRE_NOTHROW(proxy.Call("temp_10ge", {}, 0, GET, oss));
        std::string s = (oss.str()).erase(0, strlen("temp_10ge "));
        REQUIRE(std::stoi(s) != -1);
    } else {
        REQUIRE_THROWS(proxy.Call("temp_10ge", {}, -1, GET));
    }
}

TEST_CASE("temp_dcdc", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        REQUIRE_NOTHROW(proxy.Call("temp_dcdc", {}, -1, GET));
        std::ostringstream oss;
        REQUIRE_NOTHROW(proxy.Call("temp_dcdc", {}, 0, GET, oss));
        std::string s = (oss.str()).erase(0, strlen("temp_dcdc "));
        REQUIRE(std::stoi(s) != -1);
    } else {
        REQUIRE_THROWS(proxy.Call("temp_dcdc", {}, -1, GET));
    }
}

TEST_CASE("temp_sodl", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        REQUIRE_NOTHROW(proxy.Call("temp_sodl", {}, -1, GET));
        std::ostringstream oss;
        REQUIRE_NOTHROW(proxy.Call("temp_sodl", {}, 0, GET, oss));
        std::string s = (oss.str()).erase(0, strlen("temp_sodl "));
        REQUIRE(std::stoi(s) != -1);
    } else {
        REQUIRE_THROWS(proxy.Call("temp_sodl", {}, -1, GET));
    }
}

TEST_CASE("temp_sodr", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        REQUIRE_NOTHROW(proxy.Call("temp_sodr", {}, -1, GET));
        std::ostringstream oss;
        REQUIRE_NOTHROW(proxy.Call("temp_sodr", {}, 0, GET, oss));
        std::string s = (oss.str()).erase(0, strlen("temp_sodr "));
        REQUIRE(std::stoi(s) != -1);
    } else {
        REQUIRE_THROWS(proxy.Call("temp_sodr", {}, -1, GET));
    }
}

TEST_CASE("temp_fpgafl", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        REQUIRE_NOTHROW(proxy.Call("temp_fpgafl", {}, -1, GET));
        std::ostringstream oss;
        REQUIRE_NOTHROW(proxy.Call("temp_fpgafl", {}, 0, GET, oss));
        std::string s = (oss.str()).erase(0, strlen("temp_fpgafl "));
        REQUIRE(std::stoi(s) != -1);
    } else {
        REQUIRE_THROWS(proxy.Call("temp_fpgafl", {}, -1, GET));
    }
}

TEST_CASE("temp_fpgafr", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        REQUIRE_NOTHROW(proxy.Call("temp_fpgafr", {}, -1, GET));
        std::ostringstream oss;
        REQUIRE_NOTHROW(proxy.Call("temp_fpgafr", {}, 0, GET, oss));
        std::string s = (oss.str()).erase(0, strlen("temp_fpgafr "));
        REQUIRE(std::stoi(s) != -1);
    } else {
        REQUIRE_THROWS(proxy.Call("temp_fpgafr", {}, -1, GET));
    }
}

/* dacs */

TEST_CASE("Setting and reading back EIGER dacs", "[.cmd][.dacs][.new]") {
    // vsvp, vtr, vrf, vrs, vsvn, vtgstv, vcmp_ll, vcmp_lr, vcal, vcmp_rl,
    // rxb_rb, rxb_lb, vcmp_rr, vcp, vcn, vis, vthreshold
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        SECTION("vsvp") { test_dac(defs::VSVP, "vsvp", 5); }
        SECTION("vtrim") { test_dac(defs::VTRIM, "vtrim", 1200); }
        SECTION("vrpreamp") { test_dac(defs::VRPREAMP, "vrpreamp", 1500); }
        SECTION("vrshaper") { test_dac(defs::VRSHAPER, "vrshaper", 1510); }
        SECTION("vsvn") { test_dac(defs::VSVN, "vsvn", 3800); }
        SECTION("vtgstv") { test_dac(defs::VTGSTV, "vtgstv", 2550); }
        SECTION("vcmp_ll") { test_dac(defs::VCMP_LL, "vcmp_ll", 1400); }
        SECTION("vcmp_lr") { test_dac(defs::VCMP_LR, "vcmp_lr", 1400); }
        SECTION("vcal") { test_dac(defs::VCAL, "vcal", 1400); }
        SECTION("vcmp_rl") { test_dac(defs::VCMP_RL, "vcmp_rl", 1400); }
        SECTION("rxb_rb") { test_dac(defs::RXB_RB, "rxb_rb", 1400); }
        SECTION("rxb_lb") { test_dac(defs::RXB_LB, "rxb_lb", 1400); }
        SECTION("vcmp_rr") { test_dac(defs::VCMP_RR, "vcmp_rr", 1400); }
        SECTION("vcp") { test_dac(defs::VCP, "vcp", 1400); }
        SECTION("vcn") { test_dac(defs::VCN, "vcn", 1400); }
        SECTION("vishaper") { test_dac(defs::VISHAPER, "vishaper", 1400); }
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
                det.setDAC(defs::VCMP_LR, vcmp_lr[i], false, {i});
                det.setDAC(defs::VCMP_RL, vcmp_rl[i], false, {i});
                det.setDAC(defs::VCMP_RR, vcmp_rr[i], false, {i});
                det.setDAC(defs::VCP, vcp[i], false, {i});
            }
        }
        // gotthard
        REQUIRE_THROWS(proxy.Call("vref_ds", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vcascn_pb", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vcascp_pb", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vout_cm", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vcasc_out", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vin_cm", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vref_comp", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("ib_test_c", {}, -1, GET));
        // mythen3
        // REQUIRE_THROWS(proxy.Call("vrpreamp", {}, -1, GET));
        // REQUIRE_THROWS(proxy.Call("vrshaper", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vrshaper_n", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vipre", {}, -1, GET));
        // REQUIRE_THROWS(proxy.Call("vishaper", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vdcsh", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vth1", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vth2", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vth3", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vcal_n", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vcal_p", {}, -1, GET));
        // REQUIRE_THROWS(proxy.Call("vtrim", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vcassh", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vcas", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vicin", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vipre_out", {}, -1, GET));
        // gotthard2
        REQUIRE_THROWS(proxy.Call("vref_h_adc", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vb_comp_fe", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vb_comp_adc", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vcom_cds", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vref_rstore", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vb_opa_1st", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vref_comp_fe", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vcom_adc1", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vref_l_adc", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vref_cds", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vb_cs", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vb_opa_fd", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vcom_adc2", {}, -1, GET));
        // jungfrau
        REQUIRE_THROWS(proxy.Call("vb_comp", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vdd_prot", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vin_com", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vref_prech", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vb_pixbuf", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vb_ds", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vref_ds", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("vref_comp", {}, -1, GET));
    }
}

/* acquisition */

TEST_CASE("trigger", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    REQUIRE_THROWS(proxy.Call("trigger", {}, -1, GET));
    auto det_type = det.getDetectorType().squash();
    if (det_type != defs::EIGER) {
        REQUIRE_THROWS(proxy.Call("trigger", {}, -1, PUT));
    } else {
        auto prev_timing =
            det.getTimingMode().tsquash("inconsistent timing mode in test");
        auto prev_frames =
            det.getNumberOfFrames().tsquash("inconsistent #frames in test");
        auto prev_exptime =
            det.getExptime().tsquash("inconsistent exptime in test");
        auto prev_period =
            det.getPeriod().tsquash("inconsistent period in test");
        det.setTimingMode(defs::TRIGGER_EXPOSURE);
        det.setNumberOfFrames(1);
        det.setExptime(std::chrono::milliseconds(1));
        det.setPeriod(std::chrono::milliseconds(1));
        auto startingfnum = det.getStartingFrameNumber().tsquash(
            "inconsistent frame nr in test");
        det.startDetector();
        {
            std::ostringstream oss;
            proxy.Call("trigger", {}, -1, PUT, oss);
            REQUIRE(oss.str() == "trigger successful\n");
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
        auto currentfnum = det.getStartingFrameNumber().tsquash(
            "inconsistent frame nr in test");
        REQUIRE(startingfnum + 1 == currentfnum);
        det.stopDetector();
        det.setTimingMode(prev_timing);
        det.setNumberOfFrames(prev_frames);
    }
}

/* Network Configuration (Detector<->Receiver) */

TEST_CASE("txndelay_left", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto prev_val = det.getTransmissionDelayLeft();
        {
            std::ostringstream oss1, oss2;
            proxy.Call("txndelay_left", {"5000"}, -1, PUT, oss1);
            REQUIRE(oss1.str() == "txndelay_left 5000\n");
            proxy.Call("txndelay_left", {}, -1, GET, oss2);
            REQUIRE(oss2.str() == "txndelay_left 5000\n");
        }
        for (int i = 0; i != det.size(); ++i) {
            det.setTransmissionDelayLeft(prev_val[i]);
        }
    } else {
        REQUIRE_THROWS(proxy.Call("txndelay_left", {}, -1, GET));
    }
}

TEST_CASE("txndelay_right", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto prev_val = det.getTransmissionDelayRight();
        {
            std::ostringstream oss1, oss2;
            proxy.Call("txndelay_right", {"5000"}, -1, PUT, oss1);
            REQUIRE(oss1.str() == "txndelay_right 5000\n");
            proxy.Call("txndelay_right", {}, -1, GET, oss2);
            REQUIRE(oss2.str() == "txndelay_right 5000\n");
        }
        for (int i = 0; i != det.size(); ++i) {
            det.setTransmissionDelayRight(prev_val[i]);
        }
    } else {
        REQUIRE_THROWS(proxy.Call("txndelay_right", {}, -1, GET));
    }
}

/* Eiger Specific */

TEST_CASE("dr", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto dr = det.getDynamicRange().squash();
        std::array<int, 4> vals{4, 8, 16, 32};
        for (const auto val : vals) {
            std::ostringstream oss1, oss2;
            proxy.Call("dr", {std::to_string(val)}, -1, PUT, oss1);
            REQUIRE(oss1.str() == "dr " + std::to_string(val) + '\n');
            proxy.Call("dr", {}, -1, GET, oss2);
            REQUIRE(oss2.str() == "dr " + std::to_string(val) + '\n');
        }
        det.setDynamicRange(dr);
    } else if (det_type == defs::MYTHEN3) {
        // not updated in firmware to support anything other than 32 at the
        // moment
        std::ostringstream oss1, oss2;
        proxy.Call("dr", {"32"}, -1, PUT, oss1);
        REQUIRE(oss1.str() == "dr 32\n");
        proxy.Call("dr", {"32"}, -1, PUT, oss2);
        REQUIRE(oss2.str() == "dr 32\n");
        REQUIRE_THROWS(proxy.Call("dr", {"4"}, -1, PUT));
        REQUIRE_THROWS(proxy.Call("dr", {"8"}, -1, PUT));
        REQUIRE_THROWS(proxy.Call("dr", {"16"}, -1, PUT));
    } else {
        // For the other detectors we should get an error message
        // except for dr 16
        REQUIRE_THROWS(proxy.Call("dr", {"4"}, -1, PUT));
        REQUIRE_THROWS(proxy.Call("dr", {"8"}, -1, PUT));
        REQUIRE_THROWS(proxy.Call("dr", {"32"}, -1, PUT));

        std::ostringstream oss1, oss2;
        proxy.Call("dr", {"16"}, -1, PUT, oss1);
        REQUIRE(oss1.str() == "dr 16\n");
        proxy.Call("dr", {"16"}, -1, PUT, oss2);
        REQUIRE(oss2.str() == "dr 16\n");
    }
}

TEST_CASE("subexptime", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);

    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto time = det.getSubExptime();
        std::ostringstream oss1, oss2;
        proxy.Call("subexptime", {"2.5us"}, -1, PUT, oss1);
        REQUIRE(oss1.str() == "subexptime 2.5us\n");
        proxy.Call("subexptime", {}, -1, GET, oss2);
        REQUIRE(oss2.str() == "subexptime 2.5us\n");
        for (int i = 0; i != det.size(); ++i) {
            det.setSubExptime(time[i], {i});
        }
    } else {
        REQUIRE_THROWS(proxy.Call("subexptime", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("subexptime", {"2.13"}, -1, PUT));
    }
}

TEST_CASE("subdeadtime", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);

    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto time = det.getSubDeadTime();
        std::ostringstream oss1, oss2;
        proxy.Call("subdeadtime", {"500us"}, -1, PUT, oss1);
        REQUIRE(oss1.str() == "subdeadtime 500us\n");
        proxy.Call("subdeadtime", {}, -1, GET, oss2);
        REQUIRE(oss2.str() == "subdeadtime 500us\n");
        for (int i = 0; i != det.size(); ++i) {
            det.setSubDeadTime(time[i], {i});
        }
    } else {
        REQUIRE_THROWS(proxy.Call("subdeadtime", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("subdeadtime", {"2.13"}, -1, PUT));
    }
}

TEST_CASE("threshold", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);

    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto prev_threshold = det.getThresholdEnergy();
        auto prev_energies =
            det.getTrimEnergies().tsquash("inconsistent trim energies to test");
        if (!prev_energies.empty()) {
            std::ostringstream oss1, oss2;
            proxy.Call("threshold", {"4500", "standard"}, -1, PUT, oss1);
            REQUIRE(oss1.str() == "threshold [4500, standard]\n");
            proxy.Call("threshold", {}, -1, GET, oss2);
            REQUIRE(oss2.str() == "threshold 4500\n");
            det.setTrimEnergies(prev_energies);
            for (int i = 0; i != det.size(); ++i) {
                if (prev_threshold[i] >= 0) {
                    det.setThresholdEnergy(prev_threshold[i], defs::STANDARD,
                                           true, {i});
                }
            }
        }
        REQUIRE_NOTHROW(proxy.Call("threshold", {}, -1, GET));
    } else {
        REQUIRE_THROWS(proxy.Call("threshold", {}, -1, GET));
    }
}

TEST_CASE("thresholdnotb", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);

    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto prev_threshold = det.getThresholdEnergy();
        auto prev_energies =
            det.getTrimEnergies().tsquash("inconsistent trim energies to test");
        if (!prev_energies.empty()) {
            std::ostringstream oss1, oss2;
            proxy.Call("thresholdnotb", {"4500 standard"}, -1, PUT, oss1);
            REQUIRE(oss1.str() == "thresholdnotb [4500 standard]\n");
            proxy.Call("threshold", {}, -1, GET, oss2);
            REQUIRE(oss2.str() == "threshold 4500\n");
            det.setTrimEnergies(prev_energies);
            for (int i = 0; i != det.size(); ++i) {
                if (prev_threshold[i] >= 0) {
                    det.setThresholdEnergy(prev_threshold[i], defs::STANDARD,
                                           false, {i});
                }
            }
        }
        REQUIRE_NOTHROW(proxy.Call("threshold", {}, -1, GET));
    } else {
        REQUIRE_THROWS(proxy.Call("thresholdnotb", {}, -1, GET));
    }
}

TEST_CASE("settingspath", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto prev_val = det.getSettingsPath();
    {
        std::ostringstream oss1, oss2;
        proxy.Call("settingspath", {"/tmp"}, -1, PUT, oss1);
        REQUIRE(oss1.str() == "settingspath /tmp\n");
        proxy.Call("settingspath", {}, -1, GET, oss2);
        REQUIRE(oss2.str() == "settingspath /tmp\n");
    }
    for (int i = 0; i != det.size(); ++i) {
        det.setSettingsPath(prev_val[i], {i});
    }
}

TEST_CASE("parallel", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();

    if (det_type == defs::EIGER) {
        auto prev_val = det.getParallelMode();
        {
            std::ostringstream oss;
            proxy.Call("parallel", {"1"}, -1, PUT, oss);
            REQUIRE(oss.str() == "parallel 1\n");
        }
        {
            std::ostringstream oss;
            proxy.Call("parallel", {"0"}, -1, PUT, oss);
            REQUIRE(oss.str() == "parallel 0\n");
        }
        {
            std::ostringstream oss;
            proxy.Call("parallel", {}, -1, GET, oss);
            REQUIRE(oss.str() == "parallel 0\n");
        }
        for (int i = 0; i != det.size(); ++i) {
            det.setParallelMode(prev_val[i], {i});
        }
    } else {
        REQUIRE_THROWS(proxy.Call("parallel", {}, -1, GET));
    }
}

TEST_CASE("overflow", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto previous = det.getOverFlowMode();
        std::ostringstream oss1, oss2, oss3;
        proxy.Call("overflow", {"1"}, -1, PUT, oss1);
        REQUIRE(oss1.str() == "overflow 1\n");
        proxy.Call("overflow", {}, -1, GET, oss2);
        REQUIRE(oss2.str() == "overflow 1\n");
        proxy.Call("overflow", {"0"}, -1, PUT, oss3);
        REQUIRE(oss3.str() == "overflow 0\n");
        for (int i = 0; i != det.size(); ++i) {
            det.setOverFlowMode(previous[i], {i});
        }
    } else {
        REQUIRE_THROWS(proxy.Call("overflow", {}, -1, GET));
    }
}

TEST_CASE("flippeddatax", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto previous = det.getBottom();
        std::ostringstream oss1, oss2, oss3;
        proxy.Call("flippeddatax", {"1"}, -1, PUT, oss1);
        REQUIRE(oss1.str() == "flippeddatax 1\n");
        proxy.Call("flippeddatax", {}, -1, GET, oss2);
        REQUIRE(oss2.str() == "flippeddatax 1\n");
        proxy.Call("flippeddatax", {"0"}, -1, PUT, oss3);
        REQUIRE(oss3.str() == "flippeddatax 0\n");
        for (int i = 0; i != det.size(); ++i) {
            det.setBottom(previous[i], {i});
        }
    } else {
        REQUIRE_THROWS(proxy.Call("flippeddatax", {}, -1, GET));
    }
}

TEST_CASE("trimen", "[.cmd][.this][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto previous = det.getTrimEnergies();
        std::ostringstream oss1, oss2;
        proxy.Call("trimen", {"4500", "5400", "6400"}, -1, PUT, oss1);
        REQUIRE(oss1.str() == "trimen [4500, 5400, 6400]\n");
        proxy.Call("trimen", {}, -1, GET, oss2);
        REQUIRE(oss2.str() == "trimen [4500, 5400, 6400]\n");

        for (int i = 0; i != det.size(); ++i) {
            det.setTrimEnergies(previous[i], {i});
        }
    } else {
        REQUIRE_THROWS(proxy.Call("trimen", {"4500", "5400", "6400"}, -1, PUT));
        REQUIRE_THROWS(proxy.Call("trimen", {}, -1, GET));
    }
}

TEST_CASE("ratecorr", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto prev_dr = det.getDynamicRange().tsquash("inconsistent dr to test");
        auto prev_tau = det.getRateCorrection();
        det.setDynamicRange(16);
        {
            std::ostringstream oss;
            proxy.Call("ratecorr", {"120"}, -1, PUT, oss);
            REQUIRE(oss.str() == "ratecorr 120ns\n");
        }
        {
            std::ostringstream oss;
            proxy.Call("ratecorr", {}, -1, GET, oss);
            REQUIRE(oss.str() == "ratecorr 120ns\n");
        }
        // may fail if default settings not loaded
        // REQUIRE_NOTHROW(proxy.Call("ratecorr", {"-1"}, -1, PUT));
        {
            std::ostringstream oss;
            proxy.Call("ratecorr", {"0"}, -1, PUT, oss);
            REQUIRE(oss.str() == "ratecorr 0ns\n");
        }
        for (int i = 0; i != det.size(); ++i) {
            det.setRateCorrection(prev_tau[i], {i});
        }
        det.setDynamicRange(prev_dr);
    } else {
        REQUIRE_THROWS(proxy.Call("ratecorr", {}, -1, GET));
    }
}

TEST_CASE("readnlines", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto prev_val = det.getPartialReadout();
        {
            std::ostringstream oss;
            proxy.Call("readnlines", {"256"}, -1, PUT, oss);
            REQUIRE(oss.str() == "readnlines 256\n");
        }
        {
            std::ostringstream oss;
            proxy.Call("readnlines", {}, -1, GET, oss);
            REQUIRE(oss.str() == "readnlines 256\n");
        }
        {
            std::ostringstream oss;
            proxy.Call("readnlines", {"16"}, -1, PUT, oss);
            REQUIRE(oss.str() == "readnlines 16\n");
        }
        REQUIRE_THROWS(proxy.Call("readnlines", {"0"}, -1, PUT));
        for (int i = 0; i != det.size(); ++i) {
            det.setPartialReadout(prev_val[i], {i});
        }
    } else {
        REQUIRE_THROWS(proxy.Call("readnlines", {}, -1, GET));
    }
}

TEST_CASE("interruptsubframe", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto prev_val = det.getInterruptSubframe();

        std::ostringstream oss1, oss2, oss3;
        proxy.Call("interruptsubframe", {"1"}, -1, PUT, oss1);
        REQUIRE(oss1.str() == "interruptsubframe 1\n");
        proxy.Call("interruptsubframe", {}, -1, GET, oss2);
        REQUIRE(oss2.str() == "interruptsubframe 1\n");
        proxy.Call("interruptsubframe", {"0"}, -1, PUT, oss3);
        REQUIRE(oss3.str() == "interruptsubframe 0\n");
        for (int i = 0; i != det.size(); ++i) {
            det.setInterruptSubframe(prev_val[i], {i});
        }

    } else {
        REQUIRE_THROWS(proxy.Call("interruptsubframe", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("interruptsubframe", {"1"}, -1, PUT));
    }
}

TEST_CASE("measuredperiod", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto prev_frames = det.getNumberOfFrames().tsquash(
            "inconsistent number of frames to test");
        auto prev_timing =
            det.getTimingMode().tsquash("inconsistent timing mode to test");
        auto prev_period = det.getPeriod();
        det.setNumberOfFrames(2);
        det.setPeriod(std::chrono::seconds(1));
        det.setTimingMode(defs::AUTO_TIMING);
        det.startDetector();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::ostringstream oss;
        proxy.Call("measuredperiod", {}, -1, GET, oss);
        std::string st = oss.str();
        std::string s = st.erase(0, strlen("measuredperiod "));
        double val = std::stod(s);
        // REQUIRE(val >= 1.0);
        REQUIRE(val < 2.0);
        for (int i = 0; i != det.size(); ++i) {
            det.setPeriod(prev_period[i], {i});
        }
        det.setNumberOfFrames(prev_frames);
        det.setTimingMode(prev_timing);
    } else {
        REQUIRE_THROWS(proxy.Call("measuredperiod", {}, -1, GET));
    }
}

TEST_CASE("measuredsubperiod", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto prev_frames = det.getNumberOfFrames().tsquash(
            "inconsistent number of frames to test");
        auto prev_timing =
            det.getTimingMode().tsquash("inconsistent timing mode to test");
        auto prev_period = det.getPeriod();
        auto prev_dr = det.getDynamicRange().tsquash("inconsistent dr to test");
        det.setNumberOfFrames(1);
        det.setPeriod(std::chrono::seconds(1));
        det.setTimingMode(defs::AUTO_TIMING);
        det.setDynamicRange(32);
        det.startDetector();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::ostringstream oss;
        proxy.Call("measuredsubperiod", {}, -1, GET, oss);
        std::string st = oss.str();
        std::string s = st.erase(0, strlen("measuredsubperiod "));
        double val = std::stod(s);
        REQUIRE(val >= 0);
        REQUIRE(val < 1000);
        for (int i = 0; i != det.size(); ++i) {
            det.setPeriod(prev_period[i], {i});
        }
        det.setNumberOfFrames(prev_frames);
        det.setTimingMode(prev_timing);
        det.setDynamicRange(prev_dr);
    } else {
        REQUIRE_THROWS(proxy.Call("measuredsubperiod", {}, -1, GET));
    }
}

TEST_CASE("activate", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto prev_val = det.getActive();
        {
            std::ostringstream oss;
            proxy.Call("activate", {"1"}, -1, PUT, oss);
            REQUIRE(oss.str() == "activate 1\n");
        }
        {
            std::ostringstream oss;
            proxy.Call("activate", {"1", "nopadding"}, -1, PUT, oss);
            REQUIRE(oss.str() == "activate 1 nopadding\n");
        }
        {
            std::ostringstream oss;
            proxy.Call("activate", {"0", "padding"}, -1, PUT, oss);
            REQUIRE(oss.str() == "activate 0 padding\n");
        }
        {
            std::ostringstream oss;
            proxy.Call("activate", {"0", "nopadding"}, -1, PUT, oss);
            REQUIRE(oss.str() == "activate 0 nopadding\n");
        }
        {
            std::ostringstream oss;
            proxy.Call("activate", {"1", "padding"}, -1, PUT, oss);
            REQUIRE(oss.str() == "activate 1 padding\n");
        }
        for (int i = 0; i != det.size(); ++i) {
            det.setActive(prev_val[i], {i});
        }

    } else {
        REQUIRE_THROWS(proxy.Call("activate", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("activate", {"1"}, -1, PUT));
    }
}

TEST_CASE("partialreset", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto prev_val = det.getPartialReset();
        std::ostringstream oss1, oss2, oss3;
        proxy.Call("partialreset", {"1"}, -1, PUT, oss1);
        REQUIRE(oss1.str() == "partialreset 1\n");
        proxy.Call("partialreset", {}, -1, GET, oss2);
        REQUIRE(oss2.str() == "partialreset 1\n");
        proxy.Call("partialreset", {"0"}, -1, PUT, oss3);
        REQUIRE(oss3.str() == "partialreset 0\n");
        for (int i = 0; i != det.size(); ++i) {
            det.setPartialReset(prev_val[i], {i});
        }
    } else {
        REQUIRE_THROWS(proxy.Call("partialreset", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("partialreset", {"1"}, -1, PUT));
    }
}

TEST_CASE("pulse", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        REQUIRE_THROWS(proxy.Call("pulse", {}, -1, GET));
        std::ostringstream oss;
        proxy.Call("pulse", {"1", "1", "5"}, -1, PUT, oss);
        REQUIRE(oss.str() == "pulse [1, 1, 5]\n");
    } else {
        REQUIRE_THROWS(proxy.Call("pulse", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("pulse", {"1", "1", "5"}, -1, PUT));
    }
}

TEST_CASE("pulsenmove", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        REQUIRE_THROWS(proxy.Call("pulsenmove", {}, -1, GET));
        std::ostringstream oss;
        proxy.Call("pulsenmove", {"1", "1", "5"}, -1, PUT, oss);
        REQUIRE(oss.str() == "pulsenmove [1, 1, 5]\n");
    } else {
        REQUIRE_THROWS(proxy.Call("pulsenmove", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("pulsenmove", {"1", "1", "5"}, -1, PUT));
    }
}

TEST_CASE("pulsechip", "[.cmd][.new]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        REQUIRE_THROWS(proxy.Call("pulsechip", {}, -1, GET));
        std::ostringstream oss;
        proxy.Call("pulsechip", {"1"}, -1, PUT, oss);
        REQUIRE(oss.str() == "pulsechip 1\n");
    } else {
        REQUIRE_THROWS(proxy.Call("pulsechip", {}, -1, GET));
        REQUIRE_THROWS(proxy.Call("pulsechip", {"1"}, -1, PUT));
    }
}

TEST_CASE("quad", "[.cmd]") {
    Detector det;
    CmdProxy proxy(&det);
    auto det_type = det.getDetectorType().squash();
    if (det_type == defs::EIGER) {
        auto prev_val = det.getQuad().tsquash("inconsistent quad to test");
        // Quad only works with a single half module EIGER
        std::ostringstream oss;
        proxy.Call("quad", {}, -1, GET, oss);
        REQUIRE(oss.str() == "quad 0\n");
        det.setQuad(prev_val);
    } else {
        REQUIRE_THROWS(proxy.Call("quad", {}, -1, GET));
    }
}
