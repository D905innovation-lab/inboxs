#ifndef  __CAMERA_H__
#define  __CAMERA_H__


/*****************使用摄像头发送彩信宏开关*************************/
#ifndef USING_CAMERA_FOR_MMS
#define	USING_CAMERA_FOR_MMS		1
#endif

/******************使用SD卡存储图片宏开关*************************/
#ifndef USING_SAVE_SD
#define USING_SAVE_SD               0
#endif

#if USING_SAVE_SD
//#include "mmc_sd.h"
//#include "diskio.h"
//#include "ff.h"
# include  "stm32f10x.h"
u8 camera_new_pathname(u8 *pname);
#endif

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#if	USING_CAMERA_FOR_MMS            //条件宏，判断是否需要使用摄像头拍照发彩信

#include <string.h>
#include <stdio.h>


typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;


//硬件引脚定义
#define  camer_pwerH()    GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define  camer_pwerL()    GPIO_ResetBits(GPIOA,GPIO_Pin_1)

//用户自定义宏变量
#define N_BYTE  256        //每次读取N_BYTE字节，N_BYTE必须是8的倍数

#define IMAGE_SIZE_160X120     0x22
#define IMAGE_SIZE_320X240     0x11
#define IMAGE_SIZE_640X480     0x00


#define COMPRESS_RATE_36       0x36   //该压缩率是默认压缩率，160x120和320x240可用此压缩率

#define COMPRESS_RATE_60       0x60   //640X480尺寸，默认压缩率36会占用45K左右的空间
                                      //选择60压缩率可将45K压缩到20K左右



typedef enum
{
    SERIAL_NUM_0 = 0x00,
    SERIAL_NUM_1,
    SERIAL_NUM_2,
    SERIAL_NUM_3,
    SERIAL_NUM_5,
    SERIAL_NUM_6,
    SERIAL_NUM_7,
    SERIAL_NUM_8,
    SERIAL_NUM_9,
    SERIAL_NUM_10
}nSerialNum;
extern const nSerialNum SerialNum_Byte;

extern u8 g_SerialNumber;
extern volatile u8 cameraReady;
extern u32 picLen;//图片长度

//串口驱动接口函数，移植时需要修改接口函数
void cam_write(const u8 *buf,u8 len);
u16 cam_receiver( u8 *buf,u16 send_len);


// 应用实例函数
u8 CameraDemoApp(u8 Serialnumber,u8 nCameraImageSize);

u8 camera_init(u8 Serialnumber,u8 nSetImageSize);
u8 send_cmd(const u8 *cmd,u8 n0,const u8 *rev,u8 n1);
void SetSerailNumber(u8 *DstCmd, const u8 *SrcCmd, u8 SrcCmdLength,
                     u8 *DstRcv, const u8 *SrcRcv, u8 SrcRcvLength,u8 nID);

//摄像头属性设置：复位/图片尺寸大小/图片压缩率
u8 send_reset(u8 Serialnumber);
u8 current_photo_size(u8 Serialnumber,u8 * nImageSize);
u8 send_photo_size(u8 Serialnumber,u8 nImageSize);
u8 send_compress_rate(u8 Serialnumber,u8 nCompressRate);

//移动侦测控制函数
u8 send_motion_sensitivity(u8 Serialnumber);
u8 send_open_motion(u8 Serialnumber);
u8 send_close_motion(u8 Serialnumber);
u8 Motion_Detecte_Idle(u8 *pSerialnumber);


//拍照处理函数
u8 send_photoBuf_cls(u8 Serialnumber);
u8 send_start_photo(u8 Serialnumber);
u32 send_read_len(u8 Serialnumber);
u8 send_get_photo(u16 add,u16 read_len,u8 *buf,u8 Serialnumber);

extern void remenberdata(u8 *data,u32 len);
void beginphoto(void);
void UART1Write(u8 *data,u32 len);

#endif

#endif	


