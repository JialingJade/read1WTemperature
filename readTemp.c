#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

//#define READ_TEMP_DEBUG

static int readValueFromFile(char* fileName, char* buff, int len);
static int readIntValueFromFile(char* fileName);


/*
 * Read from a file. Max read length is len bytes.
 * The data read is stored in buff.
 * Return: -1 - fail
 *         0 -  success.
 */
 
static int readValueFromFile(char* fileName, char* buff, int len) 
{
    int ret = -1;
    FILE *fp = fopen(fileName,"r");
	
    if (fp == NULL)
	{
#ifdef READ_TEMP_DEBUG
        printf("Unable to open file %s",fileName);
#endif
        return -1;
    }
	else
	{
        if (fread(buff, sizeof(char), len, fp)>0)
		{
            ret = 0;
        }
    }
	
    fclose(fp);
    return ret;
}

/*
 * Read an integer value from a file.
 * Return: -1 - fail
 *         Other integer value -  success.
 */

static int readIntValueFromFile(char* fileName) 
{
    char buff[255];
    memset(buff, 0, sizeof(buff));
    int ret = readValueFromFile(fileName, buff, sizeof(buff)-1);
    if (ret == 0)
	{
        return atoi(buff);
    }
	
    return ret;
}

int read1WTemperature(float *in_out_p_temp)
{
	FILE *f;	
	struct dirent *de;  // Pointer for directory entry 
	bool foundTempSensor = false;
	char fullFileName[PATH_MAX+30];
	int temperature;
	
	// opendir() returns a pointer of DIR type.  
    DIR *dr = opendir("/sys/bus/w1/devices/");
	if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
#ifdef READ_TEMP_DEBUG
        printf("Could not open /sys/bus/w1/devices/ directory\n" );
#endif 
        return -1; 
    }
	
	// for readdir() 
    while ((de = readdir(dr)) != NULL)
	{
#ifdef READ_TEMP_DEBUG
		printf("%s\n", de->d_name);
#endif		
		if((de->d_name[0] == '2') && (de->d_name[1] == '8') && (de->d_name[2] == '-'))
		{
			foundTempSensor = true;
			break;
		}	
	}
	
	closedir(dr);

	if(foundTempSensor)
	{
		sprintf(fullFileName, "/sys/bus/w1/devices/%s/temperature", de->d_name);
		temperature = readIntValueFromFile(fullFileName);
		
		if(temperature!=-1)
		{
			*in_out_p_temp = temperature/1000.0;
			return 0;
		}		
	}  
      
    return -1;	
}

/*
int main()
{
	float temp;
	if(read1WTemperature(&temp) == 0)
	{
		printf("Success, %f\n", temp);
	}
	else
	{
		printf("Fail\n");
	}
} */
