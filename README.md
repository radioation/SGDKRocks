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




