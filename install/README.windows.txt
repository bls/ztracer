
Build instructions for Win7 64-bit
----------------------------------

* Install cygwin, including 'wget' and 'make' from: http://cygwin.com/install.html

* Install Win7 SDK from: http://msdn.microsoft.com/en-us/windowsserver/bb980924.aspx

* Open a cygwin shell, go to the xpintools directory and run ./setup_pin.sh

* TODO: additional dependencies

* Then open a Windows SDK Command prompt

* set PATH=%PATH%;c:\cygwin64\bin

* Go to xpintools\ztracer

C:\Users\user\Desktop\xpintools\ztracer>make -f makefile.real PIN_ROOT=..\pin-2.13-61206-msvc10-windows


Misc notes
----------

make "TARGET_COMPILER=ms" "MAKE=cmd /C make"


