#include "lptport.h"
//#include "filelog.hpp"
//extern CFileLog filelog;

int		LPT_data		= 0x378;
int		LPT_status	= 0x379;
int		LPT_ctrl		= 0x37a;
int j=0;

char synch=0;

//* EnableRealTime()
//* Sets the process to real-time priority, hopefully so Windows
//*	won't task us out during a data transfer
//*	seems to work only most of the time
//*

void	EnableRealTime(void) {
	if (!SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS)) {
		printf("Could not set realtime priority!\n");
	}
}

void	DisableRealTime(void) {
	SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
}

//*	The following functions were adapted from the DSK's included
//*	C code.  Please refer to the DSK documentation for more information
//* about it
//*
int		LPT_xmit_byte(Uint8 data)
{

	_outp(LPT_data, data);
	// Set STRB Low ??
	_outp(LPT_ctrl, 0x5);
	for (int t = 50; t > 0; t--)
	{
		if ((_inp(LPT_status) & 0x8) == 0)
			break;
		SDL_Delay(1);
	}
	if (t == 0)
	{
		_outp(LPT_ctrl, 0x4 | 0x00);
		return(-1);
	}
	// Set STRB High ??
	_outp(LPT_ctrl, 0x4 | 0x00);

	return 0;
}

int		LPT_xmit_long(Uint32 data)
{
	//* set a 2us timeout 
	long	t = 2000000;
	
	for	(int b = 0; b < 4; b++)
	{
		_outp(LPT_data, (data & 0xFF));
		_outp(LPT_ctrl, 0x5);
		for (int i = t ; i > 0; i--)
		{
			if ((_inp(LPT_status) & 0x8) == 0)
				break;
			if (i <= 1)
			{
				_outp(LPT_ctrl, 0x4 | 0x00);

				DisableRealTime();

				return(-1);
			}
		}
		t = 0;
		if (b == 3) 
			_outp(LPT_ctrl, 0x4 | 0x00);
		else
			_outp(LPT_ctrl, 0x4);
		data = data >> 8;
	}

	DisableRealTime();

	return 0;
}

int		LPT_recv_long(Uint32 *data)
{
	Uint32	ul	= 0,
			n;
	long	t = 100000L;
	
	for (char shift = 0; shift < 32; shift += 4)
	{
		for ( ; t > 0; t--)
		{
			if ((_inp(LPT_status) & 0x8) == 0)
				break;
			if (t <= 1) {
				DisableRealTime();
				return(-1);
			}
		}
		t = 0;

		_outp(LPT_ctrl, 0x5 | 0x00);
	
		n = ((_inp(LPT_status) >> 4) ^ 0x8) & 0x0F;
		ul = ul | (n << shift);

		_outp(LPT_ctrl, 0x4 | 0x00);
	}
	*data = ul;

	DisableRealTime();

	return 0;
}

int		getmem(Uint32 addr, Uint32 length, Uint32 *data)
{
	Uint32	CMD[4];
	int		error;

	if (( addr         & 0xF00000L) == 0xF00000L) return -1;
	if (((addr+length) & 0xF00000L) == 0xF00000L) return -1;

	CMD[0] = 2; // 2 = XREAD
	CMD[1] = length;
	CMD[2] = addr;
	CMD[3] = 1;

	EnableRealTime();

	for (int i = 0; i < 4; i++)
	{
		if ((error = LPT_xmit_long(CMD[i])) < 0)
			return error;
	}
	for ( ; length > 0; length--) 
	{
		if ((error = LPT_recv_long(data++)) < 0)
			return error;
	}
	return 0;
}

int		putmem(Uint32 addr, Uint32 length, Uint32 *data)
{
	Uint32	CMD[4];
	int		error;

	if (( addr         & 0xF00000L) == 0xF00000L) return -1;
	if (((addr+length) & 0xF00000L) == 0xF00000L) return -1;

	CMD[0] = 1; // 1 = XWRIT
	CMD[1] = length;
	CMD[2] = addr;
	CMD[3] = 1;

	EnableRealTime();
	
	for (int i = 0; i < 4; i++)
	{
		if ((error = LPT_xmit_long(CMD[i])) < 0)
			return error;
	}
	for ( ; length > 0; length--)
	{
		if ((error = LPT_xmit_long(*data++)) < 0)
			return error;
	}
	return 0;
}

void timingLoop(int value)
{
//	char message[28];
/* changes every 5 lines 	
	if (j==0)
	{
//		sprintf(message,"SYNCH_DOWN\n");//%d\n",0);
//		filelog.Log(message);
		synch=0;
	} else if (j==5) {
//		sprintf(message,"SYNCH_UP\n");//%d\n",1);
//		filelog.Log(message);
		synch=value;
	} else if (j==10) {
		j=-1;
	}*/
//	if (j==0)
//		synch=0;
//	else
		synch=value;
	LPT_xmit_byte(synch);
	LPT_xmit_byte(0);
//	j++;
}