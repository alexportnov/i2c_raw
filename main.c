#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#ifdef __ANDROID__
    #define I2C_SLAVE_FORCE 	0x0706	// Use this slave address, even if it is already in use by a driver!
    #define I2C_FUNCS			0x0705	// Get the adapter functionality mask
    #define I2C_FUNC_I2C        0x00000001
#else
    #include <linux/i2c.h>
    #include <linux/i2c-dev.h>
#endif

#include <sys/ioctl.h>

/***********************************************************************/
static int i2c_open(int dev, int addr)
{
	unsigned long funcs;
    char device[64] = {0};
    sprintf(device, "/dev/i2c-%d", dev);

	int fd = open(device, O_RDWR);
	if (fd < 0)
    {
		fprintf(stderr, "Unable to open device %s\n", device);
		return -1;
	}

	int ret = ioctl(fd, I2C_FUNCS, &funcs);
	if (ret < 0)
    {
		fprintf(stderr, "I2C ioctl(I2C_FUNCS) error %d\n", errno);
		close(fd);
		return -1;
	}

	if ((funcs & I2C_FUNC_I2C) == 0) 
    {
		fprintf(stderr, "Controller is not I2C, only SMBUS.\n");
		close(fd);
		return -1;
	}

	ret = ioctl(fd, I2C_SLAVE_FORCE, addr);
	if (ret < 0)
    {
		fprintf(stderr, "I2C ioctl(slave) error %d\n", errno);
		close(fd);
		return -1;
	}

	return fd;
}

/***********************************************************************/
int main(int argc, char* argv[])
{
    char *end;
    int i;
    unsigned char block[256];
    if(argc < 4 || argc >= 4 + sizeof(block) ||
     (argv[1][0] != 'r' && argv[1][0] != 'w'))
    {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "   i2c_raw r/w BUS DEV READ_SIZE/DATA\n\n");
        fprintf(stderr, "   Write 4 bytes i2c bus 3, device 0x55\n");
        fprintf(stderr, "       i2c_raw w 3 0x55 0x01 0x02 0x03 0x04\n");
        fprintf(stderr, "   Read 2 bytes i2c bus 3, device 0x55 register 0x66 (depends on device protocol)\n");
        fprintf(stderr, "       i2c_raw w 3 0x55 0x66\n");
        fprintf(stderr, "       i2c_raw r 3 0x55 2\n");
        return -1;
    }

    int dev = strtol(argv[2], &end, 0);
    int addr = strtol(argv[3], &end, 0);
    int fd = i2c_open(dev, addr);
    if(0 > fd)
    {
        fprintf(stderr, "Error: Cannot open bus=%x addr=%x\n", dev, addr);
        return -1;
    }

    if(argv[1][0] == 'r')
    {
        int nbyte = 1;
        if(argc > 4)
        {
            nbyte = strtol(argv[4], &end, 0);
            if (*end || nbyte < 0 || nbyte > 0xff)
            {
                nbyte = 1;
            }
        }

        int ret = read(fd, block, nbyte);
        if (ret != nbyte)
        {
            fprintf(stderr, "read (ret != nbyte) ret=%d, nbytes=%d\n", ret, nbyte);
        }

        for(i = 0; i < ret; ++i)
        {
            printf("0x%02x", block[i]);
        }
        printf("\n");
    }
    else
    {
        int nbyte = 0;
        for (i = 4; i < argc; i++)
        {
            int value = strtol(argv[i], &end, 0);
            if (value < 0 || value > 0xff)
            {
                fprintf(stderr, "Data value invalid! value=%x\n", value);
            }

            block[nbyte++] = (unsigned char)value;
        }

        int ret = write(fd, block, nbyte);
        if (ret != nbyte)
        {
            fprintf(stderr, "write (ret != nbyte) ret=%d, nbytes=%d\n", ret, nbyte);
        }
    }

	close(fd);

	return 0;
}
