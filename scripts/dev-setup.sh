#! /bin/sh

case $(uname -o) in
FreeBSD)
    # Install the requried packages...
    for pkg in gmake; do
        if ! pkg info -q $pkg ; then
            pkg install -y $pkg
        fi
    done
    ;;
GNU/Linux)
    # To create a debian8 or debian9 KVM for development (including IPv6 support), see https://jira.office.opendns.com/browse/SRE-2142
    # Install the requried packages...
    export DEBIAN_FRONTEND=noninteractive
    apt-get update
    apt-get -y install at git fakeroot dpkg-dev debhelper bash build-essential curl libbsd-dev net-tools ||
         { echo "FAILED to install required packages."; exit 1; }
    ;;
*)
    echo "I don't know what I'm doing" >&2
    ;;
esac
