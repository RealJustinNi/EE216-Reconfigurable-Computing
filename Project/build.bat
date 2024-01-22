@echo off

set VISION_INCLUDE=vitis_library_vision_L1/include
set DEVICE=xczu7ev-ffvc1156-2-e

del settings.tcl
echo set VISION_INCLUDE %VISION_INCLUDE% >> settings.tcl
echo set DEVICE %DEVICE% >> settings.tcl

if %*=="" (
    set task=0
) else (
    set task=%1
)

if %task%==csim (
    echo set CSIM 1 >> settings.tcl
    echo set CSYNTH 0 >> settings.tcl
    echo set COSIM 0 >> settings.tcl
    CALL vitis_hls -f ./scripts/run_hls.tcl
) else if %task%==csynth (
    echo set CSIM 0 >> settings.tcl
    echo set CSYNTH 1 >> settings.tcl
    echo set COSIM 0 >> settings.tcl
    CALL vitis_hls -f ./scripts/run_hls.tcl
) else if %task%==cosim (
    echo set CSIM 0 >> settings.tcl
    echo set CSYNTH 0 >> settings.tcl
    echo set COSIM 1 >> settings.tcl
    CALL vitis_hls -f ./scripts/run_hls.tcl
) else if %task%==bd (
    echo set CSIM 0 >> settings.tcl
    echo set CSYNTH 0 >> settings.tcl
    echo set COSIM 0 >> settings.tcl
    CALL vivado -mode batch -source ./scripts/block_design.tcl -notrace
) else if %task%==bit (
    echo set CSIM 0 >> settings.tcl
    echo set CSYNTH 0 >> settings.tcl
    echo set COSIM 0 >> settings.tcl
    CALL vivado -mode batch -source ./scripts/bitstream.tcl -notrace
) else if %task%==all (
    echo set CSIM 0 >> settings.tcl
    echo set CSYNTH 1 >> settings.tcl
    echo set COSIM 0 >> settings.tcl
    CALL vitis_hls -f ./scripts/run_hls.tcl
    CALL vivado -mode batch -source ./scripts/block_design.tcl -notrace
    CALL vivado -mode batch -source ./scripts/bitstream.tcl -notrace
    CALL vivado -mode batch -source ./scripts/check_timing.tcl -notrace
) else if %task%==clean (
    rmdir hls_project /s /q
    rmdir vivado_project /s /q
    del *.jou *.log /q
) else (
    echo Wrong usage
)

