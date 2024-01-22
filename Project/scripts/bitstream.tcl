set project_folder "./vivado_project"
set overlay_name "ORB_accel"
set design_name "ORB_accel"

# open block design
open_project ${project_folder}/${overlay_name}.xpr
open_bd_design ${project_folder}/${overlay_name}.srcs/sources_1/bd/${design_name}/${design_name}.bd

# Add top wrapper 
make_wrapper -files [get_files ${project_folder}/${overlay_name}.srcs/sources_1/bd/${design_name}/${design_name}.bd] -top
add_files -norecurse ${project_folder}/${overlay_name}.srcs/sources_1/bd/${design_name}/hdl/${design_name}_wrapper.v
set_property top ${design_name}_wrapper [current_fileset]
# import_files -fileset constrs_1 -norecurse ./vivado/constraints/${overlay_name}.xdc
update_compile_order -fileset sources_1

# call implement
launch_runs impl_1 -to_step write_bitstream -jobs 16
wait_on_run impl_1

# report utilization
open_run impl_1
report_utilization -file utilization_report.txt

# export hardware
write_hw_platform -fixed -include_bit -force -file ORB_accel.xsa

# move and rename bitstream to final location
file copy -force ${project_folder}/${overlay_name}.runs/impl_1/${design_name}_wrapper.bit ${overlay_name}.bit

# copy hwh files
file copy -force ${project_folder}/${overlay_name}.gen/sources_1/bd/${design_name}/hw_handoff/${design_name}.hwh ${overlay_name}.hwh