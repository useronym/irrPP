~/dev/android-ndk-r9d/ndk-build
ant debug
adb -d uninstall com.entity.irrpp.demo
adb -d install bin/irrPP-Demo-debug.apk 
