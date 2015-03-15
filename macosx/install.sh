#!/bin/sh

INSTALLED="/Library/Extensions/k8062.kext"

# Uninstall previous extension
if [ -e $INSTALLED ]; then
    sudo kextunload -verbose $INSTALLED
    sudo rm -Rf $INSTALLED
fi

# Compile new extension
xcodebuild -project k8062.xcodeproj -target k8062 -configuration Deployment clean build

if [ ! -e "build/Deployment/k8062.kext" ]; then
    echo "Failed to build extension"
    exit -1
fi


# Install new extension
sudo cp -R build/Deployment/k8062.kext $INSTALLED
sudo chown -R root:wheel $INSTALLED
sudo kextload -verbose $INSTALLED
