#ifndef USEIO_H
#define USEIO_H

//======================== I/O define format ==============================
//  1. ������ function�� �⺻���� �� (����: 16���� ���)
//  2. �ܾ�� �ܾ� ���̿��� '_'�� ��� ��
//  3. Spare�� ��� �ڿ� '_'�� No�� ����.
//  4. �Էº�:
//     . Function �̸� �տ� IN_�� �ٿ� ��°� ���� ��.
//     . Function ���뿡 ������ Detect
//     . Switch �� ��� SW�� ��� ��.
//  5. ��º�
//     . Function�� Sol�� ���� ��
//     . 
//  6. ������ ������ ��� ��,����� ���ڸ� �ο��� ������
//==========================================================================			
const int cInput_Origin		= 1000;
const int cOutput_Origin	= 2000;
const int cOutput_End		= 3000;

typedef enum
{
	//X000~X00F 
	IN_FRONT_EMERGENCY_SW = cInput_Origin,		// X000
	IN_LEFT_START_SW,							// X001
	IN_STOP_SW,									// X002
	IN_RESET_SW,								// X003
	IN_RIGHT_START_SW,							// X004
	IN_SPARE_X005,								// X005
	IN_SPARE_X006,								// X006
	IN_SPARE_X007,								// X007
	IN_JOG_X_FWD,								// X008
	IN_JOG_X_BWD,								// X009
	IN_JOG_Y_FWD,								// X00A
	IN_JOG_Y_BWD,								// X00B
	IN_JOG_Z_UP,								// X00C
	IN_JOG_Z_DOWN,								// X00D
	IN_JOG_W_RIGHT,								// X00E
	IN_JOG_W_LEFT,								// X00F

	//X010~X01F 
	IN_SAFETY_RIGHT_DETECT,						// X010
	IN_SAFETY_LEFT_DETECT,						// X011
	IN_RIGHT_DETECT,							// X012
	IN_LEFT_DETECT,								// X013
	IN_RIGHT_GLASS_DOWN_DETECT,					// X014
	IN_RIGHT_GLASS_UP_DETECT,					// X015
	IN_LEFT_GLASS_DOWN_DETECT,					// X016
	IN_LEFT_GLASS_UP_DETECT,					// X017
	IN_ALIGN_CYLINDER_UP_DETECT,				// X018
	IN_ALIGN_CYLINDER_DOWN_DETECT,				// X019
	IN_SPARE_X01A,								// X01A
	IN_SPARE_X01B,								// X01B
	IN_SPARE_X01C,								// X01C
	IN_SPARE_X01D,								// X01D
	IN_SPARE_X01E,								// X01E
	IN_SPARE_X01F,								// X01F
	In_Max
} Input_Addr;

typedef enum
{
	//Y000~Y00F 
	OUT_SPARE_Y000 = cOutput_Origin,			// Y000
	OUT_STOP_SWITCH,							// Y001
	OUT_RESET_SWITCH,							// Y002
	OUT_LEFT_START_SW,							// Y003
	OUT_RIGHT_START_SW,							// Y004
	OUT_SPARE_Y005,								// Y005
	OUT_LEFT_GLASS_UP_DOWN,						// Y006
	OUT_RIGHT_GLASS_UP_DOWN,					// Y007
	OUT_ALIGN_UP_DOWN,							// Y008
	OUT_TOP_BOTTOM_IONIZER,						// Y009
	OUT_SERVO_ON_AC_1,							// Y00A
	OUT_SERVO_ON_AC_2,							// Y00B
	OUT_BUZZER_ALARM,							// Y00C
	OUT_TOWER_LAMP_YELLOW,						// Y00D
	OUT_TOWER_LAMP_GREEN,						// Y00E
	OUT_TOWER_LAMP_RED,							// Y00F

	//Y010~Y01F 
	OUT_GRAB_1,									// Y010 - ��ιݻ�
	OUT_GRAB_2,									// Y011 - ��ε���
	OUT_GRAB_3,									// Y012	- �Ϻιݻ�
	OUT_GRAB_4,									// Y013 - �Ϻε���
	OUT_SPARE_Y014,								// Y014
	OUT_SPARE_Y015,								// Y015
	OUT_SPARE_Y016,								// Y016
	OUT_SPARE_Y017,								// Y017
	OUT_SPARE_Y018,								// Y018
	OUT_SPARE_Y019,								// Y019
	OUT_SPARE_Y01A,								// Y01A
	OUT_SPARE_Y01B,								// Y01B
	OUT_SPARE_Y01C,								// Y01C
	OUT_SPARE_Y01D,								// Y01D
	OUT_SPARE_Y01E,								// Y01E
	OUT_SPARE_Y01F,								// Y01F
	Out_Max
} Output_Addr;
#endif									
