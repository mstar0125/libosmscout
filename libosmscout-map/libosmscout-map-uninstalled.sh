# generated by configure / remove this line to disable regeneration
prefix="/usr/local"
exec_prefix="${prefix}"
bindir="${exec_prefix}/bin"
libdir="/Users/mac/Desktop/libosmscout/libosmscout-map/src/.libs"
datarootdir="${prefix}/share"
datadir="${datarootdir}"
sysconfdir="${prefix}/etc"
includedir="/Users/mac/Desktop/libosmscout/libosmscout-map/./include"
package="libosmscout-map"
suffix=""

for option; do case "$option" in --list-all|--name) echo  "libosmscout-map"
;; --help) pkg-config --help ; echo Buildscript Of "libosmscout map rendering base library"
;; --modversion|--version) echo "0.1"
;; --requires) echo pkg-config libosmscout "libosmscout"
;; --libs) echo -L${libdir} "" "-losmscoutmap"
pkg-config libosmscout
;; --cflags) echo -I${includedir} ""
pkg-config libosmscout
;; --variable=*) eval echo '$'`echo $option | sed -e 's/.*=//'`
;; --uninstalled) exit 0 
;; *) ;; esac done