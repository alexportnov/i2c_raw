# i2c_raw
Low level i2c read/write command line utility.
It provides low level read/write byte-based interface with no restrictions.
It basically allow you to prototype i2c drivers from shell scripts.

Will work on any Linux based system, including Android.
You will obviously need the permissions.

## Why?
Turns out i2cget/set utilities will work only on SMBUS devices following strict protocol.
If you want, say send more than 32 byte burst - you can't.
All other deviations the devise may support with are not standard 1/2 byte addressing is not supported.

i2c_raw will allow you to send/receive the data with no protocol constraints, just as you would see it on a sniffer.

## Usage
i2c_raw r/w BUS DEV READ_SIZE/DATA

Write 4 bytes i2c bus 3, device 0x55
```sh
 i2c_raw w 3 0x55 0x01 0x02 0x03 0x04
```

Read 2 bytes i2c bus 3, device 0x55 register 0x66 (depends on device protocol)
```sh
i2c_raw w 3 0x55 0x66
i2c_raw r 3 0x55 2
```
## Compilation
Linux/crosscompile
```sh
gcc main.c -o i2c_raw
```

Android (mk file provided)
```sh
$ . build/envsetup.sh
$ mmm ./PATH_TO_WHERE_YOU_PUT_IT/i2c_raw
```


Have fun ...
