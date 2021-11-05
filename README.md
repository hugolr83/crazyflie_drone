# inf3995-crazyflie

Crazyflie firmware and app  for drones

## Getting started

- clone this repo recursively (submodules), make and flash drones :

```
git clone --recursive this-repo
```
```
make
```
```
pip3 install cfclient
python3 -m cfloader -w radio://0/your-channel/your-bandwith/your-address flash cf2.bin stm32-fw
```

