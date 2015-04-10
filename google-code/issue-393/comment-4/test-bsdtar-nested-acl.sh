#!/bin/bash

set -e

TESTDIR=/tmp/test-bsdtar-acls
TESTGROUP=nobody

sudo rm -rf "$TESTDIR" && mkdir -p "$TESTDIR" && cd "$TESTDIR"

echo "*** Default ACLs should be preserved"
echo "    on top-level directories as well as subdirectories."
echo

for DIRPATH in dir dir1/dir2; do
    sudo rm -rf src dest && sudo mkdir src dest

    echo "==> Testing directory path: $DIRPATH"
    echo
    sudo mkdir -p src/$DIRPATH
    sudo chmod 750 src/$DIRPATH

    # Set ACL on source (sub-)directory
    sudo setfacl -nm g:$TESTGROUP:rx,d:g:$TESTGROUP:rx src/$DIRPATH

    # Backup and restore
    sudo bsdtar -cf - -C src . | sudo bsdtar -xf - -C dest

    echo "--> The following two ACLs should be identical"
    echo "    (in particular the Default ACLs):"
    echo
    sudo getfacl src/$DIRPATH
    sudo getfacl dest/$DIRPATH

    # Create dummy files
    OLDUMASK=$(umask)
    umask 0027
    sudo touch src/$DIRPATH/test
    sudo touch dest/$DIRPATH/test
    umask $OLDUMASK

    echo "--> The following two ACLs should be identical"
    echo "    (in particular the effective rights masks):"
    echo
    sudo getfacl src/$DIRPATH/test
    sudo getfacl dest/$DIRPATH/test
done

echo "==> Done."
