#include "main.h"
#include "camera.h"

u16 remb = 170;


#define  USART2_RECEIVE_BUFSIZE     5000
u8 gprs_buf[USART2_RECEIVE_BUFSIZE] = "";


#if		USING_CAMERA_FOR_MMS

#define CLEAR_FRAME            1   	 //ȥ������ͼƬ����Я����Э��ͷ��β76 00 32 00
#define ECHO_CMD_DEBUG_INFO    0     //1������ָ����ԣ�0���ر�

#define ID_SERIAL_NUM       1        //��������������λ��

//��λָ���븴λ�ظ�
const u8 reset_rsp[] = {0x76,0x00,0x26,0x00};
const u8 reset_cmd[] = {0x56,0x00,0x26,0x00};


//���ͼƬ����ָ����ظ�
const u8 photoBufCls_cmd [] = {0x56,0x00,0x36,0x01,0x02};
const u8 photoBufCls_rsp[] = {0x76,0x00,0x36,0x00,0x00};  	

//����ָ����ظ�
const u8 start_photo_cmd[] = {0x56,0x00,0x36,0x01,0x00};    
const u8 start_photo_rsp[] = {0x76,0x00,0x36,0x00,0x00};   

//��ͼƬ����ָ����ظ�
//ͼƬ����ָ��ظ���ǰ7���ֽ��ǹ̶��ģ����2���ֽڱ�ʾͼƬ�ĳ���
//��0xA0,0x00,10���Ʊ�ʾ��40960,��ͼƬ����(��С)Ϊ40K
const u8 read_len_cmd[] = {0x56,0x00,0x34,0x01,0x00};
const u8 read_len_rsp[] = {0x76,0x00,0x34,0x00,0x04,0x00,0x00};

//��ͼƬ����ָ����ظ�
//get_photo_cmdǰ6���ֽ��ǹ̶��ģ�
//��9,10�ֽ���ͼƬ����ʼ��ַ
//��13,14�ֽ���ͼƬ��ĩβ��ַ�������ζ�ȡ�ĳ���

//�����һ���Զ�ȡ����9,10�ֽڵ���ʼ��ַ��0x00,0x00;
//��13,14�ֽ���ͼƬ���ȵĸ��ֽڣ�ͼƬ���ȵĵ��ֽ�(��0xA0,0x00)

//����Ƿִζ�ȡ��ÿ�ζ�N�ֽ�(N������8�ı���)���ȣ�
//����ʼ��ַ���ȴ�0x00,0x00��ȡN����(��N & 0xff00, N & 0x00ff)��
//�󼸴ζ�����ʼ��ַ������һ�ζ�ȡ���ݵ�ĩβ��ַ
const u8 get_photo_cmd [] = {0x56,0x00,0x32,0x0C,0x00,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF }; 
const u8 get_photo_rsp []  = {0x76,0x00,0x32,0x00,0x00};

//����ѹ����ָ����ظ������1���ֽ�Ϊѹ����ѡ��
//��Χ�ǣ�00 - FF
//Ĭ��ѹ������36
const u8 set_compress_cmd [] = {0x56,0x00,0x31,0x05,0x01,0x01,0x12,0x04,0x36};
const u8 compress_rate_rsp [] = {0x76,0x00,0x31,0x00,0x00};

//����ͼƬ�ߴ�ָ����ظ�
//set_photo_size_cmd���1���ֽڵ�����
//0x22 - 160X120
//0x11 - 320X240
//0x00 - 640X480
const u8 set_photo_size_cmd [] = {0x56,0x00,0x31,0x05,0x04,0x01,0x00,0x19,0x11};
const u8 set_photo_size_rsp [] = {0x76,0x00,0x31,0x00,0x00 };

//��ȡͼƬ�ߴ�ָ����ظ�
//read_photo_size_rsp���1���ֽڵ�����
//0x22 - 160X120
//0x11 - 320X240
//0x00 - 640X480
const u8 read_photo_size_cmd [] = {0x56,0x00,0x30,0x04,0x04,0x01,0x00,0x19};
const u8 read_photo_size_rsp [] = {0x76,0x00,0x30,0x00,0x01,0x00};



//�ƶ����ָ��
//motion_enable_cmd ���ƶ����
//motion_disable_cmd �ر��ƶ����
const u8 motion_enable_cmd [] = {0x56,0x00,0x37,0x01,0x01};
const u8 motion_disable_cmd [] = {0x56,0x00,0x37,0x01,0x00};
const u8 motion_rsp [] = {0x76,0x00,0x37,0x00,0x00};

//��ϵͳ��⵽���ƶ�ʱ���Զ��Ӵ������motio_detecte
const u8 motion_detecte [] = {0x76,0x00,0x39,0x00,0x00};

//�ƶ��������������
const u8 motion_sensitivity_cmd [] = {0x56,0x00,0x31,0x05,0x01,0x01,0x1A,0x6E,0x03};
const u8 motion_sensitivity_rsp [] = {0x76,0x00,0x31,0x00,0x00};





volatile u8 cameraReady = 0;
u32 picLen = 0;   //���ݳ���

//CommandPacket��ResponsePacket���ڿ���ֻ������ָ�Ӧ���ڴ�
u8 CommandPacket[16];
u8 ResponsePacket[7];

const nSerialNum SerialNum_Byte;//���к�ö�ٱ���
u8 g_SerialNumber = 0;//�����ϱ��ƶ����ʱ����ĵ�ǰ���к�

u8 photodata[31000];


void beginphoto(void)                                               //��ʼ����
{
	remb=170;
		CameraDemoApp(SERIAL_NUM_0,IMAGE_SIZE_640X480);
//		UART1Write(photodata,remb);//��ӡ������������
 		
}



/****************************************************************
��������SetSerailNumber
��������: �޸�Э���е����
���������Ŀ��ָ����׵�ַ��Դָ���׵�ַ��Դָ��ȣ�
          Ŀ��Ӧ�𻺴��׵�ַ��ԴӦ���׵�ַ��ԴӦ�𳤶ȣ���Ҫ�޸ĵ�
          ���ֵ
����:��
******************************************************************/		
void SetSerailNumber(u8 *DstCmd, const u8 *SrcCmd, u8 SrcCmdLength,
                     u8 *DstRsp, const u8 *SrcRsp, u8 SrcRspLength,u8 nID)
{
    memset(&CommandPacket,0,sizeof(CommandPacket));
    memset(&ResponsePacket,0,sizeof(ResponsePacket));
    
    memcpy(DstCmd,SrcCmd,SrcCmdLength);
    memcpy(DstRsp,SrcRsp,SrcRspLength);
    
    DstCmd[ID_SERIAL_NUM] = nID & 0xFF;
    DstRsp[ID_SERIAL_NUM] = nID & 0xFF;

}

/****************************************************************
��������cam_write
��������: �ӿں�����д���������ͷ�Ĵ���
������������ݵ��׵�ַ������
����:��
******************************************************************/		
void cam_write(const u8 *buf,u8 len)
{ 
    
    //��Ҫ����ʱ�������۲��ӡ������,����ʹ�ô���2����
    #if ECHO_CMD_DEBUG_INFO
    
    UART1Write("\r\n",0);
    UART1Write((u8 *)buf,len);
    UART1Write("\r\n",0);     
    #endif
    
    //д������������
    UART3Write((u8 *)buf,len);/////
    
}

/****************************************************************
��������cam_receiver
�����������ӿں�������ȡ��������ͷ�Ĵ���
����������������ݵĵ�ַ������
����:���յ����ݸ���
******************************************************************/		
u16 cam_receiver(u8 *buf,u16 send_len)/////
{ 
    u16 i = 0;
    i = UART3_Receiver_buf(buf,send_len,L_TIME);
    return i;
}

/****************************************************************
��������camera_init
��������������ͷ��ʼ��
������������кţ���Ҫ���õ�ͼƬ�ߴ�
����:��ʼ���ɹ�����1����ʼ��ʧ�ܷ���0
******************************************************************/		
u8 camera_init(u8 Serialnumber,u8 nSetImageSize)                          //����ͷ��ʼ��
{    
    u8 CurrentImageSize = 0xFF;
    u8 CurrentCompressRate = COMPRESS_RATE_36;
    
    //��ȡ��ǰ��ͼƬ�ߴ絽currentImageSize
    if ( !current_photo_size(Serialnumber,&CurrentImageSize))
    {
       PC_Usart("\r\nread_photo_size error\r\n",0);
        return 0;
    }
    
    //�ж��Ƿ���Ҫ�޸�ͼƬ�ߴ�
    if(nSetImageSize != CurrentImageSize)
    {
        //����ͼƬ�ߴ磬���ú�λ��Ч���������ú�����ñ���
        if ( !send_photo_size(Serialnumber,nSetImageSize))
        {
            PC_Usart("\r\nset_photo_size error\r\n",0);
            return 0;
        }
        else
        {
            //��λ��Ч
            if ( !send_reset(Serialnumber))
            {
                PC_Usart("\r\reset error\r\n",0);
                return 0;
            }
            delay_us(1000000);
            CurrentImageSize = nSetImageSize;
        }
        
    }
    
    //����ͬͼƬ�ߴ������ʵ���ͼƬѹ����
    if(nSetImageSize == CurrentImageSize)
    {
        switch(CurrentImageSize)
        {
            case IMAGE_SIZE_160X120:
            case IMAGE_SIZE_320X240:
                 CurrentCompressRate = COMPRESS_RATE_36;
                 break;
            case IMAGE_SIZE_640X480:
                 CurrentCompressRate = COMPRESS_RATE_60;
                 break;
            default:
                break;
        }
    }
    //����ͼƬѹ���ʣ�������棬ÿ���ϵ������������
    if ( !send_compress_rate(Serialnumber,CurrentCompressRate))
    {
        PC_Usart("\r\nsend_compress_rate error\r\n",0);
        return 0;
    }

    //����Ҫע��,����ѹ���ʺ�Ҫ��ʱ
    delay_us(100000);

    return 1;
    
}

/****************************************************************
 ��������send_cmd
 ��������������ָ�ʶ��ָ���
 ���������ָ����׵�ַ��ָ��ĳ��ȣ�ƥ��ָ����׵�ַ������֤�ĸ���
 ���أ��ɹ�����1,ʧ�ܷ���0
******************************************************************/	
u8 send_cmd( const u8 *cmd,u8 n0,const u8 *rev,u8 n1)
{
    u8  i;
    u8  tmp[5] = {0x00,0x00,0x00,0x00,0x00};
    cam_write(cmd, n0);
    if ( !cam_receiver(tmp,5) ) 
    {
        return 0;
    }
   
    //��������
    for (i = 0; i < n1; i++)
    {  
        if (tmp[i] != rev[i]) 
        {
            return 0;
        }
    }
    
    return 1;

}


/****************************************************************
��������current_photo_size
��������:��ȡ��ǰ���õ�ͼƬ�ߴ�
���������Serialnumber���кţ�nImageSize����ͼƬ�ߴ�����ñ���
����:�ɹ�����1,ʧ�ܷ���0
******************************************************************/	
u8 current_photo_size(u8 Serialnumber,u8 * nImageSize)
{  
    u8  i;
    u8  tmp[6] = {0x00,0x00,0x00,0x00,0x00,0x00};

    SetSerailNumber( CommandPacket,
                     read_photo_size_cmd,
                     sizeof(read_photo_size_cmd),
                     ResponsePacket,
                     read_photo_size_rsp,
                     sizeof(read_photo_size_rsp),
                     Serialnumber );
      
    cam_write(CommandPacket, sizeof(read_photo_size_cmd));

    if ( !cam_receiver(tmp,6) ) 
    {
        return 0;
    }
   
    //��������,�Ա�ǰ5���ֽ�
    for (i = 0; i < 5; i++)
    {  
        if (tmp[i] != ResponsePacket[i]) 
        {
            return 0;
        }
    }
    
    //���һ���ֽڱ�ʾ��ǰ��ͼƬ��С
    *nImageSize = tmp[5];
    return 1;
}


/****************************************************************
��������send_photo_size
�����������������յ�ͼƬ�ߴ磨��ѡ��160X120,320X240,640X480��
������������кţ���Ҫ���õ�ͼƬ�ߴ�
����:�ɹ�����1,ʧ�ܷ���0
******************************************************************/	
u8 send_photo_size(u8 Serialnumber,u8 nImageSize)
{  
    u8  i;
    
    SetSerailNumber( CommandPacket,
                     set_photo_size_cmd,
                     sizeof(set_photo_size_cmd),
                     ResponsePacket,
                     set_photo_size_rsp,
                     sizeof(set_photo_size_rsp),
                     Serialnumber );
    
    CommandPacket [sizeof(set_photo_size_cmd) - 1] = nImageSize;
    
    i = send_cmd( CommandPacket,
                  sizeof(set_photo_size_cmd),
                  ResponsePacket,
                  sizeof(set_photo_size_rsp) );
    return i;
}


/****************************************************************
��������send_reset
�������������͸�λָ�λ��Ҫ��ʱ1-2��
������������к�
����:�ɹ�����1 ʧ�ܷ���0
******************************************************************/		
u8 send_reset(u8 Serialnumber)
{  
    u8 i;
    //����������Ӧ���޸����
    SetSerailNumber( CommandPacket,
                     reset_cmd,
                     sizeof(reset_cmd),
                     ResponsePacket,
                     reset_rsp,
                     sizeof(reset_rsp),
                     Serialnumber );
    
    i = send_cmd( CommandPacket,
                  sizeof(reset_cmd),
                  ResponsePacket,
                  sizeof(reset_rsp) );
    
    return i;

}

/****************************************************************
��������send_stop_photo
�������������ͼƬ����
������������к�
����:�ɹ�����1,ʧ�ܷ���0
******************************************************************/		 
u8 send_photoBuf_cls(u8 Serialnumber)
{ 
    u8 i;
    
    SetSerailNumber( CommandPacket,
                     photoBufCls_cmd,
                     sizeof(photoBufCls_cmd),
                     ResponsePacket,
                     photoBufCls_rsp,
                     sizeof(photoBufCls_rsp),
                     Serialnumber );
    
    i = send_cmd( CommandPacket,
                  sizeof(photoBufCls_cmd),
                  ResponsePacket,
                  sizeof(photoBufCls_rsp) );
    return i;
}  


/****************************************************************
��������send_compress_rate
������������������ͼƬѹ����
������������к�
����:�ɹ�����1,ʧ�ܷ���0
******************************************************************/		 
u8 send_compress_rate(u8 Serialnumber,u8 nCompressRate)
{
    u8 i;
    
    SetSerailNumber( CommandPacket,
                     set_compress_cmd,
                     sizeof(set_compress_cmd),
                     ResponsePacket,
                     compress_rate_rsp,
                     sizeof(compress_rate_rsp),
                     Serialnumber );
    
    if(nCompressRate > 0x36)
    {
        //���һ���ֽڱ�ʾѹ����
        CommandPacket [sizeof(set_compress_cmd) - 1] = nCompressRate;
    }
    
    i = send_cmd( CommandPacket,
                  sizeof(set_compress_cmd),
                  ResponsePacket,
                  sizeof(compress_rate_rsp) );
    return i;
}


/****************************************************************
��������send_start_photo
�������������Ϳ�ʼ���յ�ָ��
������������к�
����:ʶ��ɹ�����1 ʧ�ܷ���0
******************************************************************/		
u8 send_start_photo(u8 Serialnumber)
{
    u8 i;
    
    SetSerailNumber( CommandPacket,
                     start_photo_cmd,
                     sizeof(start_photo_cmd),
                     ResponsePacket,
                     start_photo_rsp,
                     sizeof(start_photo_rsp),
                     Serialnumber );
    
    i = send_cmd( CommandPacket,
                  sizeof(start_photo_cmd),
                  ResponsePacket,
                  sizeof(start_photo_rsp) );
    return i;
}	  


/****************************************************************
��������send_read_len
������������ȡ���պ��ͼƬ���ȣ���ͼƬռ�ÿռ��С
������������к�
����:ͼƬ�ĳ���
******************************************************************/	
u32 send_read_len(u8 Serialnumber)
{
    u8 i;
    u32 len;
    u8 tmp[9];	
    
    SetSerailNumber( CommandPacket,
                     read_len_cmd,
                     sizeof(read_len_cmd),
                     ResponsePacket,
                     read_len_rsp,
                     sizeof(read_len_rsp),
                     Serialnumber );
    
    //���Ͷ�ͼƬ����ָ��
    cam_write(CommandPacket, 5);

    if ( !cam_receiver(tmp,9)) 
    {
        return 0;
    }

    //��������
    for (i = 0; i < 7; i++)
    {
        if ( tmp[i] != ResponsePacket[i]) 
        {
            return 0;
        }
    }
    
    len = (u32)tmp[7] << 8;//���ֽ�
    len |= tmp[8];//���ֽ�
    
    return len;
}


/****************************************************************
��������send_get_photo
������������ȡͼƬ����
�����������ͼƬ��ʼ��ַStaAdd, 
          ��ȡ�ĳ���readLen ��
          �������ݵĻ�����buf
          ���к�
����:�ɹ�����1��ʧ�ܷ���0
FF D8 ... FF D9 ��JPG��ͼƬ��ʽ

1.һ���Զ�ȡ�Ļظ���ʽ��76 00 32 00 00 FF D8 ... FF D9 76 00 32 00 00

2.�ִζ�ȡ��ÿ�ζ�N�ֽ�,ѭ��ʹ�ö�ȡͼƬ����ָ���ȡM�λ���(M + 1)�ζ�ȡ��ϣ�
���һ��ִ�к�ظ���ʽ
76 00 32 00 <FF D8 ... N> 76 00 32 00
�´�ִ�ж�ȡָ��ʱ����ʼ��ַ��Ҫƫ��N�ֽڣ�����һ�ε�ĩβ��ַ���ظ���ʽ
76 00 32 00 <... N> 76 00 32 00
......
76 00 32 00 <... FF D9> 76 00 32 00 //lastBytes <= N

Length = N * M �� Length = N * M + lastBytes

******************************************************************/	
u8 send_get_photo(u16 staAdd,u16 readLen,u8 *buf,u8 Serialnumber)
{
    u8 i = 0;
    u8 *ptr = NULL;
    
    
    SetSerailNumber( CommandPacket,
                     get_photo_cmd,
                     sizeof(get_photo_cmd),
                     ResponsePacket,
                     get_photo_rsp,
                     sizeof(get_photo_rsp),
                     Serialnumber );
    
    //װ����ʼ��ַ�ߵ��ֽ�
    CommandPacket[8] = (staAdd >> 8) & 0xff;
    CommandPacket[9] = staAdd & 0xff;
    //װ��ĩβ��ַ�ߵ��ֽ�
    CommandPacket[12] = (readLen >> 8) & 0xff;
    CommandPacket[13] = readLen & 0xff;
    
    //ִ��ָ��
    cam_write(CommandPacket,16);
																								   
    //�ȴ�ͼƬ���ݴ洢��buf����ʱ�������ݻظ��򷵻�0
    if ( !cam_receiver(buf,readLen + 10))
    {
        return 0;
    }
    
    //����֡ͷ76 00 32 00 00
    for (i = 0; i < 5; i++)
    {
        if ( buf[i] != ResponsePacket[i] )
        {
            return 0;
        }
    }

    //����֡β76 00 32 00 00
    for (i = 0; i < 5; i++)
    {
        if ( buf[i + 5 + readLen] != ResponsePacket[i] )
        {
            return 0;
        }
    }
    
    
    //�꿪��ѡ����/���� ֡ͷ֡β76 00 32 00 00
    #if CLEAR_FRAME
//    memcpy(buf,buf + 5,read_len);
    ptr = buf;
    
    for (; readLen > 0; ++ptr)
    {
        *(ptr) = *(ptr + 5);
        readLen--;
    }
    #endif
    
    return 1;
}

/****************************************************************
��������send_open_motion
�������������ʹ��ƶ����ָ��
������������к�
����:ʶ��ɹ�����1 ʧ�ܷ���0
******************************************************************/		
u8 send_motion_sensitivity(u8 Serialnumber)
{
    u8 i;
    
    SetSerailNumber( CommandPacket,
                     motion_sensitivity_cmd,
                     sizeof(motion_sensitivity_cmd),
                     ResponsePacket,
                     motion_sensitivity_rsp,
                     sizeof(motion_sensitivity_rsp),
                     Serialnumber );
    
    i = send_cmd( CommandPacket,
                  sizeof(motion_sensitivity_cmd),
                  ResponsePacket,
                  sizeof(motion_sensitivity_rsp) );
    return i;
}

/****************************************************************
��������send_open_motion
�������������ʹ��ƶ����ָ��
������������к�
����:ʶ��ɹ�����1 ʧ�ܷ���0
******************************************************************/		
u8 send_open_motion(u8 Serialnumber)
{
    u8 i;
    
    SetSerailNumber( CommandPacket,
                     motion_enable_cmd,
                     sizeof(motion_enable_cmd),
                     ResponsePacket,
                     motion_rsp,
                     sizeof(motion_rsp),
                     Serialnumber );
    
    i = send_cmd( CommandPacket,
                  sizeof(motion_enable_cmd),
                  ResponsePacket,
                  sizeof(motion_rsp) );
    return i;
}

/****************************************************************
��������send_close_motion
�������������͹ر��ƶ����ָ��
������������к�
����:ʶ��ɹ�����1 ʧ�ܷ���0
******************************************************************/		
u8 send_close_motion(u8 Serialnumber)
{
    u8 i;
    
    SetSerailNumber( CommandPacket,
                     motion_disable_cmd,
                     sizeof(motion_disable_cmd),
                     ResponsePacket,
                     motion_rsp,
                     sizeof(motion_rsp),
                     Serialnumber );
    
    i = send_cmd( CommandPacket,
                  sizeof(motion_disable_cmd),
                  ResponsePacket,
                  sizeof(motion_rsp) );
    return i;
}

/****************************************************************
��������Motion_Detecte_Idle
��������: �ȴ��ƶ�����¼�,�ú�������RS485ͬʱ�Ӷ������ͷʱ������
          ��ǰ�ǵڼ������к��ϱ��ƶ����
�������������һ��ָ�����
����:�ɹ�����1 ʧ�ܷ���0
******************************************************************/		
u8 Motion_Detecte_Idle(u8 *pSerialnumber)
{
    u8  tmp[5] = {0x00,0x00,0x00,0x00,0x00};
    
    if ( !cam_receiver(tmp,5) ) 
    {
        return 0;
    }
    
    //��������
    //ȫ����5�����ݣ�ֻУ��4�������������±�Ϊ1�����к�
    if(!(tmp[0] == motion_detecte[0] && 
         tmp[2] == motion_detecte[2] &&
         tmp[3] == motion_detecte[3] &&
         tmp[4] == motion_detecte[4] ))
    {
        return 0;
    }
    
    //ȡ�����к�
    *pSerialnumber = tmp[1];
    return 1;

}

void remenberdata(u8 *data,u32 len)
{
	u16 j;	
        for(j = 0;j < len;j++)
        {
            //uart1_putchar(data[i]);
					photodata[remb]=data[j];
					remb++;
				//	printf("%x",photodata[remb]);
					
        }
}


/****************************************************************
��������CameraDemoApp
��������������ͷӦ��ʵ��
������������к�,ͼƬ�ߴ�
����:�ɹ�����1��ʧ�ܷ���0
******************************************************************/		
u8 CameraDemoApp(u8 Serialnumber,u8 nCameraImageSize)
{
    u16 cntM = 0,lastBytes = 0,i = 0;
    
    #if USING_SAVE_SD
    FRESULT res;
    u8 pname[20];
    u8 Issuccess = 0;
    u32 defaultbw = 1;
    #endif
    
    
    //��ʼ������ͷ
    cameraReady = camera_init(Serialnumber,nCameraImageSize);
    if(!cameraReady)
    {
        return 0;
    }
    //���ͼƬ����
    if( !send_photoBuf_cls(Serialnumber) )
    {
        return 0;
    }
    //��ʼ����
    if( !send_start_photo(Serialnumber) )
    {
        return 0;
    }
    else
    {
        //��ȡ���պ��ͼƬ����
        picLen = send_read_len(Serialnumber);
    }
    
    if( !picLen )
    {
        return 0;
    }
    else
    {
        cntM = picLen / N_BYTE;
        lastBytes = picLen % N_BYTE;
			
    //��M�Σ�ÿ�ζ�N_BYTE�ֽ�
    if( cntM )
    {
        for( i = 0; i < cntM; i++)
        {	 
            memset(gprs_buf, 0, sizeof(gprs_buf));
            
            //��ͼƬ���ȶ�ȡ����
            if( !send_get_photo( i * N_BYTE,
                               N_BYTE,
                               gprs_buf,
                               Serialnumber) )
            {
                return 0;
            }
            else
            {
                //�˷�֧�ɽ�ͼƬ���������ָ���Ĵ���
                //��ӿں�������ͼƬ����д�뵽����2
                //UART1Write(gprs_buf,N_BYTE);
							remenberdata(gprs_buf,N_BYTE);		
							//UART1Write(gprs_buf,N_BYTE);
            }
            
            delay_ms(100);
						
        }
    }
    //ʣ��ͼƬ����
    if(lastBytes)
    {

        memset(gprs_buf, 0, sizeof(gprs_buf));	
        
        //��ȡʣ�೤��
        if( !send_get_photo( i * N_BYTE,
                            lastBytes,
                            gprs_buf,
                            Serialnumber) )
        {
        
            return 0;
        }
        else
        {
            //UART1Write(gprs_buf,lastBytes);
          remenberdata(gprs_buf,N_BYTE);
					//UART1Write(gprs_buf,lastBytes);
        }
        delay_ms(100);
    }
    return 1;
}
		


#endif
}