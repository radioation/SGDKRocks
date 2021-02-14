# SGDK Rocks
SGDK Example Code.

I've been teaching myself SGDK for fun.   The example programs here
are based on notes I've taken while playing with SGDK.  This project will
contain some example programs and *(eventually)* a playable version of Asteroids


# SGDK on Ubuntu 20.04
While I do most of my SGDK coding on Windows 10,  I sometimes find it convenient to work
on my GNU/Linux laptop.  While I could use Linux native tools to work with SGDK, but I
went with running SGDK's tools in Wine (which I also use for Atari 8-bit development).  
I haven't had any problems with this setup.  I had previously had SGDK running in CentOS 8,
but recently switched to Ubuntu 20.04.


## Docker
The 
~~~bash
sudo apt update
sudo apt upgrade

~~~

Install docker:  https://docs.docker.com/engine/install/ubuntu/

~~~bash
sudo apt install apt-transport-https ca-certificates curl gnupg-agent software-properties-common

curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -

sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"

sudo apt update
sudo apt install docker-ce docker-ce-cli containerd.io
~~~

Install with Docker
~~~bash
git checkout https://github.com/Stephane-D/SGDK.git
cd SGDK
sudo docker build -t sgdk .

cd sample/basics/hello-world

sudo docker run -rm -v $PWD:/src sgdk
~~~



## Installing SGDK on Ubuntu 20.04 with Wine
I use Wine to run SGDK on Linux. I also use it to run Genesis emulators.  I generaly use
Gens KMod and BlastEm for testing.


### Install Wine
1. Add Wine to Ubuntu.

Getting Wine running on Ubuntu is straightforward.
Instructions can easily be found at https://wiki.winehq.org/Ubuntu.  I've replciated the steps here

~~~bash
sudo dpkg --add-architecture i386
wget -nc https://dl.winehq.org/wine-builds/winehq.key
sudo add-apt-repository 'deb https://dl.winehq.org/wine-builds/ubuntu/ focal main'
sudo apt-key add winehq.key

sudo apt update

sudo apt install --install-recommends winehq-stable
~~~

2. Make a folder for the SGDK Wine environment

~~~bash
sudo mkdir /opt/sgdk
sudo chown ${USERNAME}:${USERNAME} /opt/sgdk
~~~

3. Create a Wine enviornment with `wineboot`
You need to define the following environment variables.  These variables
are used by SGDK and WINE.   If you want them permanently set, add them
to your profile or make a script to set them as needed.
~~~bash
export GDK='c:/sgdk'
export GDK_WIN='c:\sgdk'
export SGDK=/opt/sgdk
export GENS=${SGDK}/gens/gens.exe
~~~

Make a folder for wine to reside in
~~~bash
mkdir ${SGDK}/wine
WINEDEBUG=-all WINEARCH=win32 WINEPREFIX=${SGDK}/wine wineboot
~~~

While you run 'wineboot' it may ask you to download one or more dependencies.
Press 'install' as needed complete the installation.

If everything went well you can run cmd.exe
~~~bash
WINEPREFIX=${SGDK}/wine wine cmd
~~~

Change to the `C:` directory and list its contents
~~~cmd
C:
dir
~~~
If everything went well you should see folders like:


~~~
> c:\>dir
> Volume in drive c has no label.
> Volume Serial Number is 0000-0000
>
> Directory of c:\
>
>  2/13/2021  10:48 PM  <DIR>         Program Files
>  2/13/2021  10:50 PM  <DIR>         Program Files (x86)
>  2/13/2021  10:48 PM  <DIR>         ProgramData
>  2/13/2021  10:48 PM  <DIR>         user
>  2/13/2021  10:50 AM  <DIR>         windows
>        0 files                        0 bytes
>        4 directories     20,158,001,152 bytes free
~~~

### Install java
1. SGDK depends on Java.  You will need to get an installer (https://www.java.com/en/download/) and run it in Wine.  I had to register to get the installer. I won't cover the registration process here.


2. Once you have the installer, copy it to your wine folder.
~~~bash
cp jdk-8u271-windows-i586.exe ${SGDK}/wine/drive_c
~~~

3. Run the java installer.  If needed, start cmd.exe
~~~bash
WINEPREFIX=${SGDK}/wine wine cmd
~~~
Run the java installer from wherever you copied it to
~~~cmd
C:
.\jdk-8u271-windows-i586.exe
~~~

This will install java at
~~~cmd
C:/Program Files (x86)/Java/jdk1.8.0_271/bin/java.exe
~~~
4. Add the Java bin/ folder to your PATH.  You can do this by starting `regedit`
~~~cmd
regedit
~~~
Navigate to the PATH variable in the `regedit` window.   You can find it at
~~~
HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Session Manager\Environment
~~~

Append Java's bin path to the PATH variable
~~~cmd
c:\Program Files (x86)\Java\jdk1.8.0_271\bin
~~~

Exit out of cmd.exe and restart it to update your PATH. 

### Install SGDK
1. Download SGDK.  At the time of this writing The latest version of SGDK is 1.60.
you can download it from https://github.com/Stephane-D/SGDK/releases/tag/v1.60


2. Unpack the SGDK archive.  
Unpack it somewhere and link it to ${SGDK}/sgdk
~~~bash
curl -L -O https://github.com/Stephane-D/SGDK/archive/v1.60.tar.gz
tar -xvf v1.60.tar.gz
ln -sv FULL_PATH/SGDK-1.60 ${SGDK}/wine/drive_c/sgdk
~~~

3. Add SGDK to the PATh in wine
Start regedit
~~~bash
WINEPREFIX=${SGDK}/wine wine regedit
~~~
and add  `%GDK_WIN%\bin to the path.

6. Build SGDK
If needed, start cmd.exe and switch back to drive C:
~~~cmd
WINEPREFIX=${SGDK}/wine wine cmd
C:
~~~
Clean and rebuild SGDK
~~~cmd
%GDK_WIN%\bin\make -f %GDK_WIN%\makelib.gen clean
%GDK_WIN%\bin\make -f %GDK_WIN%\makelib.gen
~~~


If everything worked correctly you should see  `libmd.a` in "C:\sgdk\lib\libmd.a"


### Make a Test project
1. Create a folder
~~~cmd
mkdir c:\work
cd work
~~~
2.  Create a source file named `main.c` in work
3. Add the following text to `main.c`
~~~c
#include<genesis.h>
int main(u16 arg) {
    VDP_drawText("It works!", 0, 0 );
    while(TRUE) {
        SYS_doVBlankProcess();
    }
}
~~~
4. Compile the software
~~~c
%GDK_WIN%\bin\make -f %GDK_WIN%\makefile.gen
~~~

This creates an out folder with a rom.bin file in it.  If you download and 
copy genskmod to ${SGDK}/gens, you can run the bin file in wine  by 
typiing `%GENS%` and loading the rom.bin from the File menu

