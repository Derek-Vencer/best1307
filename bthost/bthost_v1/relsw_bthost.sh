#!/bin/bash

EXPORT_LIB="$EXPORT_LIB
`cat <<-EOF
lib
EOF
`
"

EXPORT_FILE_DIR="$EXPORT_FILE_DIR
`cat <<-EOF
adapter
Makefile
service/bt_source
service/ble_app
service/ble_audio/aob_app
service/ble_audio/gaf_core/inc
service/ble_audio/gaf_core/Makefile
service/bt_app/src/a2dp_codecs/Makefile
service/bt_app/src/audio_policy
service/bt_app/src/*.cpp
service/bt_app/test
service/bt_app/inc
service/bt_app/Makefile
service/Makefile
stack/Makefile
stack/ble_stack/inc
stack/ble_stack/Makefile
stack/ble_stack/ip/profiles/ams
stack/ble_stack/ip/profiles/anc
stack/ble_stack/ip/profiles/anp
stack/ble_stack/ip/profiles/bas
stack/ble_stack/ip/profiles/blp
stack/ble_stack/ip/profiles/bms
stack/ble_stack/ip/profiles/buds
stack/ble_stack/ip/profiles/cpp
stack/ble_stack/ip/profiles/cscp
stack/ble_stack/ip/profiles/datapath
stack/ble_stack/ip/profiles/dis
stack/ble_stack/ip/profiles/find
stack/ble_stack/ip/profiles/gfps
stack/ble_stack/ip/profiles/glp
stack/ble_stack/ip/profiles/hogp
stack/ble_stack/ip/profiles/hrp
stack/ble_stack/ip/profiles/htp
stack/ble_stack/ip/profiles/lan
stack/ble_stack/ip/profiles/ota
stack/ble_stack/ip/profiles/pasp
stack/ble_stack/ip/profiles/prf
stack/ble_stack/ip/profiles/prox
stack/ble_stack/ip/profiles/rscp
stack/ble_stack/ip/profiles/tile
stack/ble_stack/ip/profiles/tip
stack/ble_stack/ip/profiles/tota
stack/ble_stack/inc/profiles
stack/bt_if/inc
stack/bt_if/Makefile
stack/bt_profiles/inc
stack/bt_profiles/Makefile
stack/bt_profiles/lib_proc/Makefile
stack/le_bridge/*.h
stack/le_bridge/Makefile
EOF
`
"

. `dirname $0`/relsw_core $@
