@echo off

rem
rem "Main"
rem

rem only ask to accept the license text once
if exist util\licenseAccepted goto EOF

rem determine if free or commercial package
set edition=commercial
if exist LICENSE.GPL3. set edition=free

if %edition%==free (
    goto HandleFree
) else (
    goto RegionLoop
    :ReturnRegionLoop
    goto HandleCommercial
)

goto EOF

rem
rem "Loops"
rem

:RegionLoop
    echo .
    echo Please choose your region.
    echo .
    echo Type 1 for North or South America.
    echo Type 2 for anywhere outside North and South America.
    echo .
    set /p region=Select: 
    if %region%==1 (
	set licenseFile=LICENSE.US
	goto ReturnRegionLoop
    )
    if %region%==2 (
	set licenseFile=LICENSE.NO
	goto ReturnRegionLoop
    )
goto RegionLoop

:HandleFree
    echo .
    echo You are licensed to use this software under the terms of
    echo the GNU General Public License (GPL) versions 2.0 or 3.0,
    echo with certain additional extra rights as specified in the
    echo Nokia Qt GPL Exception version 1.3.
    echo .
    echo Type '2' to view the GNU General Plublic License version 2.0.
    echo Type '3' to view the GNU General Plublic License version 3.0.
    echo Type 'E' to view the Nokia Qt GPL Exception version 1.3.
    echo Type 'yes' to accept this license offer.
    echo Type 'no' to decline this license offer.
    echo .
    set /p answer=Do you accept the terms of this license? 

    if %answer%==no exit 1
    if %answer%==yes (
	 echo license accepted > util\licenseAccepted
	 goto EOF
    )
    if %answer%==2 more LICENSE.GPL2
    if %answer%==3 more LICENSE.GPL3
    if %answer%==e more GPL_EXCEPTION.TXT
    if %answer%==E more GPL_EXCEPTION.TXT
goto HandleFree

:HandleCommercial
    echo .
    echo License Agreement
    echo .
    echo Type '?' to view the Qt Solutions Commercial License.
    echo Type 'yes' to accept this license offer.
    echo Type 'no' to decline this license offer.
    echo .
    set /p answer=Do you accept the terms of this license? 

    if %answer%==no exit 1
    if %answer%==yes (
	 echo license accepted > util\licenseAccepted
	 copy %licenseFile% LICENSE
	 del LICENSE.US
	 del LICENSE.NO
	 goto EOF
    )
    if %answer%==? more %licenseFile%
goto HandleCommercial

:EOF
