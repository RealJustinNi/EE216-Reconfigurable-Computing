#!/bin/bash

VISION_INCLUDE="vitis_library_vision_L1/include"
DEVICE="xczu7ev-ffvc1156-2-e"

rm -f settings.tcl
echo 'set VISION_INCLUDE "'"$VISION_INCLUDE"'"' >> settings.tcl
echo 'set DEVICE "'"$DEVICE"'"' >> settings.tcl

if [ "$#" -eq 0 ]; then
    task="0"
else
    task="$1"
fi

if [ "$task" == "csim" ]; then
    echo "set CSIM 1" >> settings.tcl
    echo "set CSYNTH 0" >> settings.tcl
    echo "set COSIM 0" >> settings.tcl
    vitis_hls -f ./scripts/run_hls.tcl
elif [ "$task" == "csynth" ]; then
    echo "set CSIM 0" >> settings.tcl
    echo "set CSYNTH 1" >> settings.tcl
    echo "set COSIM 0" >> settings.tcl
    vitis_hls -f ./scripts/run_hls.tcl
elif [ "$task" == "cosim" ]; then
    echo "set CSIM 0" >> settings.tcl
    echo "set CSYNTH 0" >> settings.tcl
    echo "set COSIM 1" >> settings.tcl
    vitis_hls -f ./scripts/run_hls.tcl
elif [ "$task" == "bd" ]; then
    echo "set CSIM 0" >> settings.tcl
    echo "set CSYNTH 0" >> settings.tcl
    echo "set COSIM 0" >> settings.tcl
    vivado -mode batch -source ./scripts/block_design.tcl -notrace
elif [ "$task" == "bit" ]; then
    echo "set CSIM 0" >> settings.tcl
    echo "set CSYNTH 0" >> settings.tcl
    echo "set COSIM 0" >> settings.tcl
    vivado -mode batch -source ./scripts/bitstream.tcl -notrace
elif [ "$task" == "all" ]; then
    echo "set CSIM 0" >> settings.tcl
    echo "set CSYNTH 1" >> settings.tcl
    echo "set COSIM 0" >> settings.tcl
    vitis_hls -f ./scripts/run_hls.tcl
    vivado -mode batch -source ./scripts/block_design.tcl -notrace
    vivado -mode batch -source ./scripts/bitstream.tcl -notrace
    vivado -mode batch -source ./scripts/check_timing.tcl -notrace
elif [ "$task" == "clean" ]; then
    rm -rf hls_project vivado_project
    rm -f *.jou *.log
else
    echo "Wrong usage"
fi
