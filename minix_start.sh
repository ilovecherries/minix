qemu-system-x86_64 -cpu max -m 2g  \
    -drive if=virtio,file=${1},format=raw \
    -netdev user,id=mynet0,hostfwd=tcp::7722-:22 \
    -device e1000,netdev=mynet0
