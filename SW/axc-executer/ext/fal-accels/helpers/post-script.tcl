############################################################
## Copyright 2021-2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

set VERSION [split $:::env(VERSION) .]
set MAJOR [lindex $VERSION 0]
set MINOR [lindex $VERSION 1]
set PATCH [lindex $VERSION 2]
set VENDOR [join $::env(VENDOR) _]
set LIBRARY [join $::env(LIBRARY) _]

# Personalise package
set IP_PATH $::env(PROJECT_NAME)/solution/impl/ip
# file copy -force $::env(CORE_PATH)/logo.png $IP_PATH/misc/
set PROJECT_PATH [pwd]
cd $IP_PATH
exec sed -i \
    "s/displayName>$::env(TOP_FUNCTION)/displayName>$::env(COMPONENT_NAME)/gi" \
    component.xml
exec sed -i "s/VIVADO_HLS_IP/$::env(LIBRARY)/g" component.xml
puts $IP_PATH

# Define as production
# exec sed -i "s/Pre-Production/Production/g" component.xml

# Integrate into the package
set PACKAGE_ZIP \
    ${VENDOR}_${LIBRARY}_$::env(TOP_FUNCTION)_${MAJOR}_${MINOR}.zip
exec zip $PACKAGE_ZIP component.xml
# exec zip $PACKAGE_ZIP misc/logo.png

cd $PROJECT_PATH
file copy -force $IP_PATH/$PACKAGE_ZIP \
    ./FAL_$::env(ACCELERATOR)_$::env(Q_KS)_$::env(Q_BW)_$::env(Q_INT)_$::env(Q_O)_$::env(Q_CORE)_$::env(Q_PES)-${MAJOR}.${MINOR}.${PATCH}.zip
