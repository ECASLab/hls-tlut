############################################################
## Copyright 2021-2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################
open_project $::env(PROJECT_NAME)
set_top $::env(TOP_FUNCTION)
add_files $::env(SRC_FILES) -cflags "-std=c++11 $::env(FAL_INCLUDES) $::env(CUSTOM_FLAGS)"
add_files -tb $::env(TB_FILE) -cflags "-std=c++11 -Wno-unknown-pragmas $::env(FAL_INCLUDES) $::env(CUSTOM_FLAGS)"
open_solution "solution"
set_part {xc7z020clg484-1}
create_clock -period $::env(CLOCK_PERIOD) -name default
source "$::env(DIRECTIVE_FILE)"

# Synthesis and cosimulation
if { [info exists ::env(TB_ARGV)] } {
  csim_design -clean -argv "$::env(TB_ARGV)"
  csynth_design
  if { ([info exists ::env(RTL_GEN)]) } {
    cosim_design -argv "$::env(TB_ARGV)"
  }
} else {
  csim_design -clean
  csynth_design
  if { ([info exists ::env(RTL_GEN)]) } {
    cosim_design
  }
}

if { ([info exists ::env(RTL_GEN)]) } {
  # Export IP
  set VENDOR [join $::env(VENDOR) _]
  set LIBRARY [join $::env(LIBRARY) _]
  export_design -rtl verilog -format ip_catalog \
      -vendor "$VENDOR" \
      -description \
      "This IP Core encapsulates a vector PE unit corresponding to the\\
      IP Core: $::env(TEST). This complies with FAL library" \
      -library "$LIBRARY" \
      -version "$::env(VERSION)"

  source "$::env(HELPER_DIR)/post-script.tcl"
}

exit
