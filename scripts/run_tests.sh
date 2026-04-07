#!/bin/bash
set -e

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( dirname "$SCRIPT_DIR" )"

echo "========================================"
echo "Running ESP-Skytrace Project Tests"
echo "========================================"

# Firmware Native Tests
echo -e "\n[1/2] Running Firmware Native Tests..."
cd "$PROJECT_ROOT/firmware"
if command -v pio >/dev/null 2>&1; then
    pio test -e native
else
    echo "Error: PlatformIO (pio) not found. Please install it to run firmware tests."
    exit 1
fi

# Processor Python Tests
echo -e "\n[2/2] Running Processor Python Tests..."
cd "$PROJECT_ROOT/processor"
if command -v pytest >/dev/null 2>&1; then
    pytest
else
    echo "Error: pytest not found. Please install it to run python tests."
    exit 1
fi

echo -e "\n========================================"
echo "All tests passed successfully!"
echo "========================================"
