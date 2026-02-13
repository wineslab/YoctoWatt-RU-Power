#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <limits>
#include <filesystem>
#include <sstream>
#include "yocto_api.h"
#include "yocto_power.h"

using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

// Global configuration
const string OUTPUT_DIRECTORY = "YoctoWatt-RU-Power/Examples/ru-power-measurement/Binary_Linux/x86_64/experiments/";

int main(int argc, const char * argv[]) {
    string errmsg;
    YPower *psensor;
    bool save_csv = false;
    ofstream csvFile;
    int duration_seconds = 60; // default duration

    if (argc >= 2 && string(argv[1]) == "y") {
        if (argc < 4) {
            cerr << "Usage: <program> y <csv_filename> <duration_seconds>" << endl;
            return 1;
        }

        fs::create_directories(OUTPUT_DIRECTORY); // ensure directory exists
        
        string filename = argv[2];
        string full_path = OUTPUT_DIRECTORY + filename;
        duration_seconds = atoi(argv[3]);
        
        save_csv = true;
        csvFile.open(full_path);
        
        if (!csvFile.is_open()) {
            cerr << "Error: Could not open file " << full_path << " for writing." << endl;
            return 1;
        }
        
        csvFile << "Timestamp,Power_W,Measurement_ns,AvgMeasurement_ns,Interval_ns,AvgInterval_ns,MinInterval_ns,MaxInterval_ns\n";
    } else if (argc == 2) {
        duration_seconds = atoi(argv[1]);
    }

    if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
        cerr << "RegisterHub error: " << errmsg << endl;
        return 1;
    }

    psensor = YPower::FirstPower();
    if (psensor == NULL) {
        cout << "No power sensor found" << endl;
        return 1;
    }

    cout << "Using power sensor: " << psensor->get_hardwareId() << endl;
    
    YModule *module = YModule::FindModule(psensor->get_module()->get_serialNumber());
    cout << "  " << module->get_productName() << endl;
    cout << "  Firmware: " << module->get_firmwareRelease() << endl;
    cout << endl;
    cout << "Starting high-speed measurements for " << duration_seconds << " seconds..." << endl;
    cout << fixed << setprecision(3);

    // Stats
    double min_interval = numeric_limits<double>::max();
    double max_interval = 0;
    double total_interval = 0;
    double total_measurement_duration = 0;
    long num_measurements = 0;

    auto last_measurement = high_resolution_clock::now();
    auto start_time = high_resolution_clock::now();

    while (true) {
        if (!psensor->isOnline()) {
            cout << "Device disconnected" << endl;
            break;
        }

        auto now_time = high_resolution_clock::now();
        auto elapsed = duration_cast<seconds>(now_time - start_time).count();
        if (elapsed >= duration_seconds) break;

        try {
            auto start = high_resolution_clock::now();
            double power = psensor->get_currentValue();
            auto now = high_resolution_clock::now();

            auto measurement_duration = duration_cast<nanoseconds>(now - start).count();
            auto interval = duration_cast<nanoseconds>(now - last_measurement).count();

            total_measurement_duration += measurement_duration;
            total_interval += interval;
            min_interval = min(min_interval, (double)interval);
            max_interval = max(max_interval, (double)interval);
            num_measurements++;

            // Timestamp with milliseconds
            auto now_sys = system_clock::now();
            auto ms = duration_cast<milliseconds>(now_sys.time_since_epoch()) % 1000;
            time_t now_c = system_clock::to_time_t(now_sys);
            tm *parts = localtime(&now_c);
            
            char timestamp[100];
            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", parts);
            
            ostringstream full_timestamp;
            full_timestamp << timestamp << "." << setfill('0') << setw(3) << ms.count();

            cout << "\rPower: " << power << "W  "
                 << "Measurement: " << measurement_duration << "ns  "
                 << "Avg Meas.: " << (total_measurement_duration / num_measurements) << "ns  "
                 << "Interval: " << interval << "ns  "
                 << "Avg Interval: " << (total_interval / num_measurements) << "ns  "
                 << "Min: " << min_interval << "ns  "
                 << "Max: " << max_interval << "ns  "
                 << flush;

            if (save_csv) {
                csvFile << full_timestamp.str() << ","
                       << power << ","
                       << measurement_duration << ","
                       << (total_measurement_duration / num_measurements) << ","
                       << interval << ","
                       << (total_interval / num_measurements) << ","
                       << min_interval << ","
                       << max_interval << "\n";
            }

            last_measurement = now;
            YAPI::Sleep(10, errmsg);

        } catch (exception &e) {
            cerr << endl << "Measurement error: " << e.what() << endl;
        }
    }

    if (save_csv) {
        csvFile.close();
        cout << endl << "Data saved to: " << OUTPUT_DIRECTORY << endl;
    }

    cout << endl << "Measurement complete. Total measurements: " << num_measurements << endl;

    YAPI::FreeAPI();
    return 0;
}