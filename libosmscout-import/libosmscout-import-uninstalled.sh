# generated by configure / remove this line to disable regeneration
prefix="/usr/local"
exec_prefix="${prefix}"
bindir="${exec_prefix}/bin"
libdir="/Users/mac/Desktop/libosmscout/libosmscout-import/src/.libs"
datarootdir="${prefix}/share"
datadir="${datarootdir}"
sysconfdir="${prefix}/etc"
includedir="/Users/mac/Desktop/libosmscout/libosmscout-import/./include"
package="libosmscout-import"
suffix=""

for option; do case "$option" in --list-all|--name) echo  "libosmscout-import"
;; --help) pkg-config --help ; echo Buildscript Of "libosmscout import library"
;; --modversion|--version) echo "0.1"
;; --requires) echo pkg-config libosmscout "libosmscout"
;; --libs) echo -L${libdir} "" "-losmscoutimport -L/usr/local/Cellar/protobuf/2.6.1/lib -lprotobuf -D_THREAD_SAFE -lz -lxml2"
pkg-config libosmscout
;; --cflags) echo -I${includedir} "-D_THREAD_SAFE -I/usr/local/Cellar/protobuf/2.6.1/include -I/usr/include/libxml2"
pkg-config libosmscout
;; --variable=*) eval echo '$'`echo $option | sed -e 's/.*=//'`
;; --uninstalled) exit 0 
;; *) ;; esac done