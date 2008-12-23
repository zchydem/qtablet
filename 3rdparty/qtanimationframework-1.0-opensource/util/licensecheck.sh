#!/bin/sh

# only ask to accept the license text once
if [ -f util/licenseAccepted ]; then
    exit 0
fi

# determine if free or commercial package
if [ -f LICENSE.GPL3 ]; then
    # free edition
    while true; do
	echo 
	echo "You are licensed to use this software under the terms of"
	echo "the GNU General Public License (GPL) versions 2.0 or 3.0,"
	echo "with certain additional extra rights as specified in the"
	echo "Nokia Qt GPL Exception version 1.3."
	echo 
	echo "Type '2' to view the GNU General Plublic License version 2.0."
	echo "Type '3' to view the GNU General Plublic License version 3.0."
	echo "Type 'E' to view the Nokia Qt GPL Exception version 1.3."
	echo "Type 'yes' to accept this license offer."
	echo "Type 'no' to decline this license offer."
	echo 
	echo "Do you accept the terms of this license? "
	read answer

	if [ "x$answer" = "xno" ]; then
	    exit 1
	elif [ "x$answer" = "xyes" ]; then
	    echo license accepted > util/licenseAccepted
	    exit 0
	elif [ "x$answer" = "xe" -o "x$answer" = "xE" ]; then
	    more GPL_EXCEPTION.TXT
	elif [ "x$answer" = "x2" ]; then
	    more LICENSE.GPL2
	elif [ "x$answer" = "x3" ]; then
	    more LICENSE.GPL3
	fi
    done
else
    while true; do
	echo 
	echo "Please choose your region."
	echo 
	echo "Type 1 for North or South America."
	echo "Type 2 for anywhere outside North and South America."
	echo 
	echo "Select: "
	read region
	if [ "x$region" = "x1" ]; then
	    licenseFile=LICENSE.US
	    break;
	elif [ "x$region" = "x2" ]; then
	    licenseFile=LICENSE.NO
	    break;
	fi
    done
    while true; do
	echo 
	echo "License Agreement"
	echo 
	echo "Type '?' to view the Qt Solutions Commercial License."
	echo "Type 'yes' to accept this license offer."
	echo "Type 'no' to decline this license offer."
	echo 
	echo "Do you accept the terms of this license? "
	read answer

	if [ "x$answer" = "xno" ]; then
	    exit 1
	elif [ "x$answer" = "xyes" ]; then
	    echo license accepted > util/licenseAccepted
	    cp "$licenseFile" LICENSE
	    rm LICENSE.US
	    rm LICENSE.NO
	    exit 0
	elif [ "x$answer" = "x?" ]; then
	    more "$licenseFile"
	fi
    done
fi
