source settings.tcl

set CFLAGS "-I${VISION_INCLUDE}"

open_project hls_project
set_top ORB_accel
add_files "src/ORB.cpp src/fast.hpp src/ORB.h src/pattern.h" -cflags "${CFLAGS}"
add_files -tb "src/testbench.cpp src/stb_image.h tests/test.png" -cflags "${CFLAGS}"

open_solution "solution1" -flow_target vivado
set_part ${DEVICE}
create_clock -period 300MHz
config_export -format ip_catalog -rtl verilog

if {$CSIM == 1} {
    csim_design
}

if {$CSYNTH == 1} {
  csynth_design
  export_design
}

if {$COSIM == 1} {
  cosim_design
}

exit