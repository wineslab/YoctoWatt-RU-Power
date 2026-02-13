# YoctoWatt High-Speed Power Measurement Tool

A high-performance C++ application for collecting power measurements from YoctoWatt devices at high sampling rates. YoctoWatt is a versatile USB power sensor that can measure power consumption for any device. This implementation is particularly suited for telecommunications research and radio unit (RU) power analysis due to YoctoWatt's excellent temporal granularity (~60ms measurement intervals) and high accuracy.

## License information

Copyright (C) 2011 and beyond by Yoctopuce Sarl, Switzerland.

Yoctopuce Sarl (hereafter Licensor) grants to you a perpetual
non-exclusive license to use, modify, copy and integrate this
file into your software for the sole purpose of interfacing
with Yoctopuce products.

You may reproduce and distribute copies of this file in
source or object form, as long as the sole purpose of this
code is to interface with Yoctopuce products. You must retain
this notice in the distributed source file.

You should refer to Yoctopuce General Terms and Conditions
for additional information regarding your rights and
obligations.

THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING
WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO
EVENT SHALL LICENSOR BE LIABLE FOR ANY INCIDENTAL, SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR
SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT
LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR
CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON THE
BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF
WARRANTY, OR OTHERWISE.

## Features

- **High-speed measurements**: Continuous power monitoring with minimal latency
- **Real-time statistics**: Live display of measurement timing statistics
- **CSV export**: Optional data logging with timestamps and performance metrics
- **Performance monitoring**: Tracks measurement duration, intervals, and timing statistics
- **USB device support**: Automatic detection and connection to YoctoWatt sensors

## Prerequisites

### Hardware
- YoctoWatt power sensor (or compatible Yoctopuce power measurement device)
- USB connection to the host system

### Software Dependencies
- C++17 compatible compiler (g++ 7.0 or later)
- YoctoLib C++ library
- libusb-1.0-dev

Install dependencies on Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential libusb-1.0-0-dev
```

## Repository Structure

```
YoctoWatt-RU-Power/
├── Examples/
│   └── ru-power-measurement/
│       ├── main.cpp          # Main measurement application
│       ├── Makefile          # Build configuration
│       └── Binary_Linux/
│           └── x86_64/       # Compiled binary output
│               └── experiments/  # CSV data output directory
├── Sources/                  # YoctoLib API source files
└── Binaries/
    └── linux/
        └── x86_64/          # Pre-compiled YoctoLib libraries
            └── libyocto-static.a
```

## Building

1. Navigate to the example directory:
```bash
cd Examples/ru-power-measurement/
```

2. Compile the application:
```bash
make
```

3. The compiled binary will be located at:
```
Binary_Linux/x86_64/yoctowatt_test
```

## Usage

### Basic Usage (Console Output Only)

Run measurements for 60 seconds (default):
```bash
./Binary_Linux/x86_64/yoctowatt_test
```

Run measurements for a custom duration (in seconds):
```bash
./Binary_Linux/x86_64/yoctowatt_test 120
```

### CSV Data Logging

To save measurements to a CSV file:
```bash
./Binary_Linux/x86_64/yoctowatt_test y <filename.csv> <duration_seconds>
```

Example:
```bash
./Binary_Linux/x86_64/yoctowatt_test y ru_power_test.csv 300
```

This will:
- Create `ru_power_test.csv` in the experiments directory
- Run measurements for 300 seconds (5 minutes)
- Save all measurements with timestamps and statistics

## Output

### Console Output

The tool displays real-time information:
```
Using power sensor: YWATTMK1-12345.power
  Yocto-Watt
  Firmware: 12345

Starting high-speed measurements for 60 seconds...
Power: 2.450W  Measurement: 45000ns  Avg Meas.: 45123ns  Interval: 55000ns  Avg Interval: 55234ns  Min: 50000ns  Max: 60000ns
```

### CSV Output Format

The CSV file contains the following columns:

| Column | Description |
|--------|-------------|
| `Timestamp` | Measurement timestamp (YYYY-MM-DD HH:MM:SS.mmm) |
| `Power_W` | Instantaneous power reading in Watts |
| `Measurement_ns` | Time taken for this measurement in nanoseconds |
| `AvgMeasurement_ns` | Running average of measurement durations |
| `Interval_ns` | Time since last measurement in nanoseconds |
| `AvgInterval_ns` | Running average of intervals between measurements |
| `MinInterval_ns` | Minimum observed interval |
| `MaxInterval_ns` | Maximum observed interval |

### Output Directory

CSV files are saved to:
```
Binary_Linux/x86_64/experiments/
```

**Note**: You can modify the output directory by editing the `OUTPUT_DIRECTORY` constant at the top of `main.cpp`.

## Configuration

### Changing Output Directory

Edit the global variable at the top of `main.cpp`:
```cpp
// Global configuration
const string OUTPUT_DIRECTORY = "/your/custom/path/";
```

Then rebuild:
```bash
make clean
make
```

### Adjusting Measurement Rate

The measurement loop includes a 10ms sleep between readings:
```cpp
YAPI::Sleep(10, errmsg);
```

*Important Note**: While you can modify this value in `main.cpp`, the actual measurement interval is limited by the YoctoWatt device's hardware sampling rate (~60ms). Reducing the sleep below 10ms will not improve the sampling rate - you'll simply poll the device more frequently but receive the same reading until the hardware updates. Increasing the sleep will reduce CPU usage but may miss some measurements.

**Recommendation**: Keep the default 10ms sleep to ensure the software is always ready when new data is available from the device without excessive CPU usage.

## Performance Characteristics

- **Measurement granularity**: ~60ms between samples (limited by YoctoWatt hardware)
- **Measurement latency**: ~600-800 nanoseconds (0.6-0.8 µs) to retrieve each reading from the device
- **Sampling rate**: ~16-17 measurements/second (1000ms / 60ms)
- **Accuracy**: High precision suitable for fine-grained power analysis

**Important**: The ~60ms interval is determined by the YoctoWatt device's internal sampling rate, not the software. Even though the code includes a 10ms sleep between polling attempts, the device only updates its power readings approximately every 60ms. The actual API call to retrieve the reading is very fast (~600-800ns), but the device hardware limits how frequently new values are available. This hardware-limited granularity is what makes YoctoWatt particularly suitable for telecommunications research.

## Troubleshooting

### Device Not Found
```
No power sensor found
```
**Solutions**:
- Check USB connection
- Verify device is powered on
- Check USB permissions: `sudo chmod 666 /dev/bus/usb/*/* `
- Run with sudo if necessary: `sudo ./yoctowatt_test`

### Compilation Errors

If you encounter `filesystem` related errors:
- Ensure you're using g++ 8.0 or later
- Verify the Makefile includes `-std=c++17`

### Permission Issues

If you get USB access errors, add udev rules:
```bash
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="24e0", MODE="0666"' | sudo tee /etc/udev/rules.d/99-yoctopuce.rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```


## Contact & Support

For YoctoLib specific issues, refer to:
- [Yoctopuce Documentation](https://www.yoctopuce.com/EN/products/usb-electrical-sensors/yocto-watt)
- [YoctoLib C++ Reference](https://github.com/yoctopuce/yoctolib_cpp)

## Acknowledgments

Built using the YoctoLib C++ library from Yoctopuce.