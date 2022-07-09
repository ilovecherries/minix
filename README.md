# Minix with Dockerfile for building on AArch64

This is a fork of Minix that incorporates [Santurysim's fixes](https://github.com/Stichting-MINIX-Research-Foundation/minix/pull/322) for compiling on newer versions of GCC, and a Dockerfile for running an Ubuntu container which is suitable for compiling Minix
using the build scripts in `releasetools`. I used this Docker container to compile Minix images for i386 on an Apple silicon Macbook (AArch64).
These images can then be run on QEMU obtained from Homebrew.

# Getting started

1. Clone this repo `git clone https://github.com/lincdog/minix`
2. Change into the repo: `cd minix`
2. Build the docker image `docker build -t minix-build -f Dockerfile.build .`
3. Start the container `docker run -it minix-build --name minixbuild01`
4. In the container, build Minix with `/usr/src/releasetools/x86_cdimage.sh` or another of the build scripts. This takes about 30-45 minutes because it has to compile the entire LLVM/Clang toolchain targeting x86.
5. Copy the built image back to your host machine: `docker cp minixbuild01:/usr/src/minix_x86.iso .`
6. (optional) Commit your docker container as a new image so that you don't need to recompile the toolchain each time while you develop: `docker commit minixbuild01 minix-build:withtools` (make take a bit, the compiled toolchain is a few GB)

# Setting up a new Minix install

I run Minix in a QEMU virtual x86 machine. QEMU may be installed using Homebrew (`brew install qemu`) or another package manager
such as apt (`apt-get install -y qemu-system-x86_64`) or downloaded from [their website](https://www.qemu.org/download/).

When you make a CD image, you can make a fresh install using QEMU as follows:
```sh
qemu-img create -f qcow2 minix.qcow2 10G && \
    qemu-system-x86_64 -cpu max -m 2g \
    -hda minix.qcow2 -cdrom minix_x86.iso \
    -boot d 
```

When you get to the shell prompt in Minix, type **`setup`** and follow the instructions to
install Minix on the blank image, minix.qcow2. Then shutdown with `shutdown -r now`, restart
the QEMU VM without the ISO image mounted 

```sh
qemu-system-x86_64 -cpu max -m 2g \
    -hda minix.qcow2 
```
## Get pkgin and basic packages

For some reason, the 3.4.0. release doesn't have the pkgsrc source repository set correctly.
To get pkgin and then core development packages,
```sh
export PKG_PATH="http://www.minix3.org/pkgsrc/packages/3.4.0/i386/All/"
pkg_add pkgin
pkgin update
pkgin_sets
```

This installs git, vim, ssh, make and other core development utilities.

To activate sshd service, 
```sh
cp /usr/pkg/etc/rc.d/sshd /etc/rc.d/
echo 'sshd=YES' >> /etc/rc.conf
service sshd start
```
And add this line to `/usr/pkg/etc/ssh/sshd_config`:
```
PermitRootLogin yes
```

# Running existing image

The command I use to start Minix in a virtual machine is:
```sh
qemu-system-x86_64 -cpu max -m 2g  \
    -hda minix.qcow2 \
    -netdev user,id=mynet0,hostfwd=tcp::7722-:22 \
    -device e1000,netdev=mynet0
```

This sets up port your host port 7722 to be forwarded to the VM's port 22, so that
you can SSH into the minix instance (`ssh -p 7722 root@localhost`) rather than using the limited functionality QEMU-based terminal.

Once you start the image and it boots, it gives you instructions - type `setup` to set up the package system and other things.

Then make sure the SSH service is activated by running `cp /usr/sbin/sshd /service/`, and set a root password with `passwd`. 
Then you should be able to SSH into your Minix instance from your regular terminal using `ssh -p 7722 root@localhost`.
