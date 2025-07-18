/*===========================================================================
*
=============================================================================
EDIT HISTORY

when           who     what, where, why
--------       ---     -----------------------------------------------------------
03/25/2020           Inital Release
=============================================================================*/

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#define pr_fmt(fmt)	"[ds28e16] %s: " fmt, __func__

#include <linux/slab.h>		/* kfree() */
#include <linux/module.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/string.h>

#include <linux/param.h>
#include <linux/jiffies.h>
#include <linux/workqueue.h>
#include <linux/power_supply.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/gpio/consumer.h>
#include <linux/regmap.h>
#include <linux/random.h>
#include <linux/pinctrl/consumer.h>

#include "ds28e30.h"
#include "./ds28e30lib/sha256_hmac.h"
#include "./ds28e30lib/deep_cover_coproc.h"
#include "battery_auth_class.h"

//common define
#define ds_info	pr_err
#define ds_dbg	pr_err
#define ds_err	pr_err
#define ds_log	pr_err

struct regulator *vreg;

// int OWSkipROM(void);
int pagenumber;

//maxim define
unsigned short CRC16;
const short oddparity[16] =
    { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
unsigned char last_result_byte = RESULT_SUCCESS;

#ifdef SPIN_LOCK_ENABLE
struct mutex ds_cmd_lock;
#endif

//define system-level publick key, authority public key  and certificate constant variables
unsigned char SystemPublicKeyX[32];
unsigned char SystemPublicKeyY[32];
unsigned char AuthorityPublicKey_X[32];
unsigned char AuthorityPublicKey_Y[32];
unsigned char Page_Certificate_R[32];
unsigned char Page_Certificate_S[32];
unsigned char Certificate_Constant[16];
unsigned char Expected_CID[2];
unsigned char Expected_MAN_ID[2];
unsigned char challenge[32] =
    { 55, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 66
};

// int pagenumber = 0;

unsigned char session_seed[32] = {
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA
};

unsigned char S_secret[32] = {
	0x0C, 0x99, 0x2B, 0xD3, 0x95, 0xDB, 0xA0, 0xB4,
	0xEF, 0x07, 0xB3, 0xD8, 0x75, 0xF3, 0xC7, 0xAE,
	0xDA, 0xC4, 0x41, 0x2F, 0x48, 0x93, 0xB5, 0xD9,
	0xE1, 0xE5, 0x4B, 0x20, 0x9B, 0xF3, 0x77, 0x39
};

int auth_ANON = 1;
int auth_BDCONST = 1;
#if 1
//define constant for generating certificate
unsigned char Samsung_Certificate_Constant[16]={0x6A,0x31,0x94,0x0C,0x06,0x98,0x41,0x9C,0x4E,0xE1,0x6D,0xEA,0xE9,0x52,0x9C,0x5C};    //for Samsung

unsigned char Samsung_SystemPublicKeyX[32]={0xF9,0x3A,0x49,0x7A,0xDF,0xA2,0xD9,0x93,		//32-byte system-level public key X
                                         0x6F,0xFE,0xD1,0x57,0x66,0x6D,0x94,0x66,
                                         0xC4,0xEE,0xF9,0x6E,0x9D,0x3B,0x6E,0xFD,
                                         0xA1,0x9F,0x0F,0x07,0x55,0x26,0x4E,0x4F };
unsigned char Samsung_SystemPublicKeyY[32]={0x1A,0x10,0x03,0x4F,0xE0,0x51,0x21,0x30,		//32-byte system-level public key Y
                                         0x57,0x7F,0x34,0x53,0x33,0x6C,0xA1,0x76,
                                         0x2F,0x82,0xC5,0x35,0xCE,0x4E,0xC0,0xC8,
                                         0x64,0x40,0x43,0xC8,0x6C,0x99,0xF7,0xED };

unsigned char  Samsung_AuthorityPrivateKey[32]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned char  Samsung_AuthorityPublicKey_X[32]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned char  Samsung_AuthorityPublicKey_Y[32]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

unsigned char Samsung_PageProtectionStatus[11]={0x02,0,0,0,0x02,0x02,0x00,0x00,0x02,0x02,0x03};  //{0,0,0,0, PROT_WP, PROT_WP, 0, 0, PROT_WP, PROT_WP, PROT_RP|PROT_WP };

//end definition for Samsung
#else
//define constant for generating certificate
unsigned char MI_Certificate_Constant[16] = { 0x25, 0x1C, 0x51, 0x79, 0x91, 0x92, 0x67, 0x58, 0xAF, 0xCA, 0xD5, 0x44, 0x22, 0xA2, 0x98, 0xB9 };	//for MI

unsigned char MI_SystemPublicKeyX[32] = { 0xE4, 0xDD, 0xAB, 0x00, 0x57, 0xA2, 0x11, 0xBC,	//32-byte system-level public key X
	0x4E, 0x7F, 0xE1, 0x70, 0xF9, 0xB0, 0x98, 0xB3,
	0x76, 0xE9, 0x84, 0x7B, 0xEE, 0x7F, 0x69, 0xCF,
	0x98, 0xC8, 0xBB, 0xD7, 0xC1, 0x22, 0xB9, 0xA3
};

unsigned char MI_SystemPublicKeyY[32] = { 0x3A, 0xD6, 0xEE, 0x23, 0x40, 0x12, 0xB9, 0x9E,	//32-byte system-level public key Y
	0x61, 0x1C, 0x73, 0xC6, 0xED, 0x94, 0x47, 0x71,
	0x2A, 0x66, 0x7A, 0x9D, 0x1A, 0xAC, 0x07, 0xC4,
	0xF9, 0x27, 0xCB, 0xF4, 0xFB, 0x22, 0x41, 0x3C
};

unsigned char MI_AuthorityPublicKey_X[32] =
    { 0x7B, 0xD7, 0x7A, 0xC6, 0xCF, 0xEF, 0xD3, 0xC1,
	0xAC, 0x60, 0x44, 0x23, 0x1C, 0xCE, 0xE2, 0x66,
	0x05, 0xD3, 0xDE, 0x85, 0x06, 0xBD, 0x17, 0xF9,
	0xA7, 0x14, 0x0B, 0x0D, 0x32, 0x7F, 0x1F, 0x16
};

unsigned char MI_AuthorityPublicKey_Y[32] =
    { 0x7D, 0xA2, 0x2B, 0xED, 0xBD, 0x13, 0xFB, 0x94,
	0x4B, 0x1F, 0x0A, 0x1B, 0x0D, 0x33, 0xE8, 0x91,
	0xAF, 0x37, 0x75, 0xD8, 0x4A, 0x5C, 0x54, 0x90,
	0x5A, 0x64, 0xC7, 0x36, 0x37, 0x33, 0xC5, 0xFC
};

// unsigned char MI_PageProtectionStatus[11]={0,0,0,0,0x02,0x02,0x22,0x22,0x02,0x02,0x03};  //{0,0,0,0, PROT_WP, PROT_WP, PROT_WP|PROT_AUTH, PROT_WP|PROT_AUTH, PROT_WP, PROT_WP, PROT_RP|PROT_WP };
#endif
// keys in byte array format, used by software compute functions
// char private_key[32];
char public_key_x[32];
char public_key_y[32];

//define testing number
#define Testing_Item_Number  19

//useful define
unsigned char MANID[2] = { 0x00 };
unsigned char HardwareVersion[2] = { 0x00 };

unsigned char TestingItemResult[Testing_Item_Number];	//maximal testing items

//define testing item result
#define Family_Code_Result    0
#define Custom_ID_Result 1	//custom ID is special for each mobile maker
#define Unique_ID_Result  2
#define MAN_ID_Result  3
#define Status_Result     4
#define Page0_Result  5
#define Page1_Result  6
#define Page2_Result  7
#define Page3_Result  8
#define CounterValue_Result 9
#define Verification_Signature_Result  10
#define Verification_Certificate_Result  11
#define Program_Page0_Result  12
#define Program_Page1_Result  13
#define Program_Page2_Result  14
#define Program_Page3_Result  15
#define DecreasingCounterValue_Result 16
#define Device_publickey_Result  17
#define Device_certificate_Result  18

//mi add
// unsigned char flag_mi_romid = 0;
unsigned char flag_mi_status = 0;
unsigned char flag_mi_page0_data = 0;
unsigned char flag_mi_page1_data = 0;
unsigned char flag_mi_counter = 0;
unsigned char flag_mi_auth_result = 0;
unsigned char mi_romid[8] = { 0x00 };
unsigned char mi_status[12] = { 0x00 };	//0,1,2,3,4,5,6,7,28,29,36
unsigned char mi_page0_data[32] = { 0x00 };
unsigned char mi_page1_data[32] = { 0x00 };
unsigned char mi_counter[32] = { 0x00 };

int mi_auth_result = 0x00;
unsigned int attr_trytimes = 1;

struct ds_data {
	struct platform_device *pdev;
	struct device *dev;
	const char *auth_name;
	struct auth_device *auth_dev;
	struct pinctrl *pinctrl;
	struct pinctrl_state *pin_active;
	struct pinctrl_state *pin_suspend;
};

static struct ds_data *g_info;

/*#define ONE_WIRE_CONFIG_IN    gpio_direction_input(g_info->auth_dev->gpio)
#define ONE_WIRE_OUT_HIGH     gpio_set_value(g_info->auth_dev->gpio, 1)
#define ONE_WIRE_OUT_LOW      gpio_set_value(g_info->auth_dev->gpio, 0)
#define ONE_WIRE_READ_GPIO_IN    gpio_get_value(g_info->auth_dev->gpio)
#define ONE_WIRE_CONFIG_OUT   gpio_direction_output(g_info->auth_dev->gpio, 1)  // ONE_WIRE_CONFIG_IN */

//#define data_reg_addr          0x00566000
//#define control_reg_addr       0x00566004
static void __iomem *data_reg_addr = NULL; /* GPIO */
static void __iomem *control_reg_addr = NULL; /* GPIO */
#define ONE_WIRE_OUT_LOW       writel_relaxed(0x1, data_reg_addr)
#define ONE_WIRE_OUT_HIGH      writel_relaxed(0x3, data_reg_addr)
#define ONE_WIRE_READ_GPIO_IN  (readl_relaxed(data_reg_addr) >> 0) & 0x01
#define ONE_WIRE_CONFIG_IN     writel_relaxed(0x1C3, control_reg_addr)
#define ONE_WIRE_CONFIG_OUT    writel_relaxed(0x3C3, control_reg_addr)

static char const hex2ascii_data[] = "0123456789abcdef";
#define hex2ascii(hex) (hex2ascii_data[hex])
static unsigned char ascii2hex(unsigned char a)
{
	unsigned char value = 0;

	if (a >= '0' && a <= '9')
		value = a - '0';
	else if (a >= 'A' && a <= 'F')
		value = a - 'A' + 0x0A;
	else if (a >= 'a' && a <= 'f')
		value = a - 'a' + 0x0A;
	else
		value = 0xff;

	return value;
}

/* write/read ops */
static void Delay_us(unsigned int T)
{
	udelay(T);
}

static unsigned char ow_reset(void)
{
	unsigned int presence = 0;
	unsigned long flags;

	raw_spin_lock_irqsave(&g_info->auth_dev->io_lock, flags);

	ONE_WIRE_CONFIG_OUT;
	ONE_WIRE_OUT_LOW;
	udelay(54);
	ONE_WIRE_CONFIG_IN;
	udelay(9);
	presence = ONE_WIRE_READ_GPIO_IN;
	udelay(50);
	ONE_WIRE_OUT_HIGH;
	ONE_WIRE_CONFIG_OUT;

	raw_spin_unlock_irqrestore(&g_info->auth_dev->io_lock, flags);

  	pr_err("presence = %x\n",presence);
	return presence;
}

unsigned char read_bit(void)
{
    unsigned char vamm;

	ONE_WIRE_CONFIG_OUT;        //set 1-wire as output
	ONE_WIRE_OUT_LOW;				// output low '0'
	//ndelay(300);                //keeping at least 300ns to generate read bit clock
	ONE_WIRE_CONFIG_IN;       // set 1-wire as input
	//ndelay(500);
	vamm = ONE_WIRE_READ_GPIO_IN;
	udelay(5);                //Keep GPIO at the input state
	ONE_WIRE_OUT_HIGH;         // prepare to output logic '1'
	ONE_WIRE_CONFIG_OUT;        //set 1-wire as output
	udelay(6);                //Keep GPIO at the output state
	pr_err("%s: vamm=%d\n", __func__, vamm); 
	return vamm;               // return value of 1-wire dat pin
}

void write_bit(unsigned char bitval)
{
	ONE_WIRE_OUT_LOW;			// Output Low '0'
	udelay(1);                                //keeping logic low for 1 us
	if (bitval != 0)
		ONE_WIRE_OUT_HIGH;         // set 1-wire to logic high if bitval='1'
	udelay(8);                              // waiting for 10us
	ONE_WIRE_OUT_HIGH;                        // restore 1-wire dat pin to high
	udelay(6); 
}

static unsigned char read_byte(void)
{
	unsigned char i;
	unsigned char value = 0;
	unsigned long flags;

	raw_spin_lock_irqsave(&g_info->auth_dev->io_lock, flags);
	for (i = 0; i < 8; i++) {
		if (read_bit())
			value |= 0x01 << i;	// reads byte in, one byte at a time and then shifts it left
	}

	raw_spin_unlock_irqrestore(&g_info->auth_dev->io_lock, flags);
	return value;
}

static void write_byte(char val)
{
	unsigned char i;
	unsigned char temp;
	unsigned long flags;

	raw_spin_lock_irqsave(&g_info->auth_dev->io_lock, flags);
	ONE_WIRE_CONFIG_OUT;
	// writes byte, one bit at a time
	for (i = 0; i < 8; i++) {
		temp = val >> i;	// shifts val right ‘i’ spaces
		temp &= 0x01;	// copy that bit to temp
		write_bit(temp);	// write bit in temp into
	}
	raw_spin_unlock_irqrestore(&g_info->auth_dev->io_lock, flags);
}

unsigned char crc_low_first(unsigned char *ptr, unsigned char len)
{
	unsigned char i;
	unsigned char crc = 0x00;

	while (len--) {
		crc ^= *ptr++;
		for (i = 0; i < 8; ++i) {
			if (crc & 0x01)
				crc = (crc >> 1) ^ 0x8c;
			else
				crc = (crc >> 1);
		}
	}

	return (crc);
}

short Read_RomID(unsigned char *RomID)
{
	unsigned char i;
	unsigned char crc = 0x00;

	// if (flag_mi_romid == 2) {
	//      memcpy(RomID, mi_romid, 8);
	//      return DS_TRUE;
	// }

#ifdef SPIN_LOCK_ENABLE
	mutex_lock(&ds_cmd_lock);
	ds_info("DS28E30_standard_cmd_flow start\n");
#endif

	if ((ow_reset()) != 0) {
		ds_err("Failed to reset ds28e30!\n");
		ow_reset();
#ifdef SPIN_LOCK_ENABLE
		mutex_unlock(&ds_cmd_lock);
#endif
		return ERROR_NO_DEVICE;
	}

	// ds_dbg("Ready to write 0x33 to maxim IC!\n");
	write_byte(CMD_READ_ROM);
	Delay_us(10);
	for (i = 0; i < 8; i++)
		RomID[i] = read_byte();

	ds_dbg("RomID = %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
	       RomID[0], RomID[1], RomID[2], RomID[3],
	       RomID[4], RomID[5], RomID[6], RomID[7]);

	crc = crc_low_first(RomID, 7);
	 ds_dbg("crc_low_first = %02x\n", crc);

	if (crc == RomID[7]) {
		// if (flag_mi_status == 0)
		//      flag_mi_romid = 1;
		// else
		//      flag_mi_romid = 2;
		memcpy(mi_romid, RomID, 8);
#ifdef SPIN_LOCK_ENABLE
		mutex_unlock(&ds_cmd_lock);
#endif
		ds_info("DS28E30_standard_cmd_flow: read ROMID successfully!\n");
		return DS_TRUE;
	} else {
		ow_reset();
#ifdef SPIN_LOCK_ENABLE
		mutex_unlock(&ds_cmd_lock);
#endif
		ds_dbg("DS28E30_standard_cmd_flow: error in reading ROMID!\n");
		return DS_FALSE;
	}
}

unsigned short docrc16(unsigned short data)
{
	pr_err("%s start: data = %x,CRC16 = %x\n",__func__,data,CRC16);
	data = (data ^ (CRC16 & 0xff)) & 0xff;
	CRC16 >>= 8;

	if (oddparity[data & 0xf] ^ oddparity[data >> 4])
		CRC16 ^= 0xc001;

	data <<= 6;
	CRC16 ^= data;
	data <<= 1;
	CRC16 ^= data;
	pr_err("%s end: data = %x,CRC16 = %x\n",__func__,data,CRC16);
	return CRC16;
}

//---------------------------------------------------------------------------
/// @internal
///
/// Sent/receive standard flow command 
///
/// @param[in] write_buf
/// Buffer with write contents (preable payload)
/// @param[in] write_len
/// Total length of data to write in 'write_buf'
/// @param[in] delay_ms
/// Delay in milliseconds after command/preable.  If == 0 then can use 
/// repeated-start to re-access the device for read of result byte. 
/// @param[in] expect_read_len
/// Expected result read length 
/// @param[out] read_buf
/// Buffer to hold data read from device. It must be at least 255 bytes long. 
/// @param[out] read_len
/// Pointer to an integer to contain the length of data read and placed in read_buf
/// Preloaded with expected read length for 1-Wire mode. If (0) but expected_read=TRUE
/// then the first byte read is the length of data to read. 
///
///  @return
///  TRUE - command successful @n
///  FALSE - command failed
///
/// @endinternal
///
int standard_cmd_flow(unsigned char *write_buf, int write_len,
		      int delay_ms, int expect_read_len,
		      unsigned char *read_buf, int *read_len)
{
	unsigned char pkt[256] = { 0 };
	int pkt_len = 0;
	int i;
	// Reset/presence
	// Rom COMMAND (set from select options)
	// if((OWSkipROM() == 0))     return DS_FALSE;

#ifdef SPIN_LOCK_ENABLE
	mutex_lock(&ds_cmd_lock);
	pr_err("DS28E30_standard_cmd_flow start\n");
#endif

	if ((ow_reset()) != 0) {
		ds_err("Failed to reset ds28e30!\n");
		ow_reset();
#ifdef SPIN_LOCK_ENABLE
		mutex_unlock(&ds_cmd_lock);
#endif
		return ERROR_NO_DEVICE;
	}

	write_byte(CMD_SKIP_ROM);

	// set result byte to no response
	last_result_byte = RESULT_FAIL_NONE;

	// Construct write block, start with XPC command
	pkt[pkt_len++] = CMD_START;

	// Add length
	pkt[pkt_len++] = write_len;

	// write (first byte will be sub-command)
	memcpy(&pkt[pkt_len], write_buf, write_len);
	pkt_len += write_len;

	//send packet to DS28E30
	for (i = 0; i < pkt_len; i++) {
		pr_err("%s pkt[%d] = %d\n", __func__, i, pkt[i]);
		write_byte(pkt[i]);
	}

	// read two CRC bytes
	pkt[pkt_len++] = read_byte();
	pkt[pkt_len++] = read_byte();
	pr_err("%s pkt[%d] = %02x,%02x\n", __func__, i, pkt[pkt_len-2],pkt[pkt_len-1]);
	// check CRC16
	CRC16 = 0;
	for (i = 0; i < pkt_len; i++)
		docrc16(pkt[i]);

	if( mi_romid[0] !=0 ) {		//?????
		if (CRC16 != 0xB001) {
			ow_reset();
			ds_info("standard_cmd_flow: 1 crc error!\n");
#ifdef SPIN_LOCK_ENABLE
			mutex_unlock(&ds_cmd_lock);
#endif
			return DS_FALSE;
		}
	}		//?????

	if (delay_ms > 0) {
		// Send release byte, start strong pull-up
		write_byte(0xAA);
		// optional delay
		Delay_us(1000 * delay_ms);
	}
	// read FF and the length byte
	pkt[0] = read_byte();
	pkt[1] = read_byte();
	*read_len = pkt[1];
	pr_err("%s write_byte(0xAA) pkt[%d] = %x,%x\n", __func__, i, pkt[0],pkt[1]);
	// make sure there is a valid length
	if (*read_len != RESULT_FAIL_NONE) {
		// read packet
		for (i = 0; i < *read_len + 2; i++) {
			read_buf[i] = read_byte();
            pr_err("%s read_buf[%d] = %02x\n", __func__, i, read_buf[i]);
		}

		// check CRC16
		CRC16 = 0;
		docrc16(*read_len);
		for (i = 0; i < (*read_len + 2); i++)
			docrc16(read_buf[i]);

		if (CRC16 != 0xB001) {
			ds_info("standard_cmd_flow: 2 crc error!\n");
#ifdef SPIN_LOCK_ENABLE
			mutex_unlock(&ds_cmd_lock);
#endif
			return DS_FALSE;
		}


		if (expect_read_len != *read_len) {
			ds_info("standard_cmd_flow: 2 len error!:%d,%d\n",expect_read_len,*read_len);
#ifdef SPIN_LOCK_ENABLE
			mutex_unlock(&ds_cmd_lock);
#endif
			return DS_FALSE;
		}

	} else {
#ifdef SPIN_LOCK_ENABLE
		pr_err("DS28E30_standard_cmd_flow: read len error!\n");
		mutex_unlock(&ds_cmd_lock);
#endif
		return DS_FALSE;
	}

#ifdef SPIN_LOCK_ENABLE
	ds_info("DS28E30_standard_cmd_flow: success!\n");
	mutex_unlock(&ds_cmd_lock);
#endif
	return DS_TRUE;
}

int ds28e30_read_ROMNO_MANID_HardwareVersion(void)
{
	unsigned char i, temp = 0, buf[10], pg = 0, flag;

	pr_err("%s mi_romid[0]:0x%x,MANID:0x%x,0x%x\n", __func__, mi_romid[0], MANID[0], MANID[1]);
	if ((mi_romid[0]&0x7F) != FAMILY_CODE) {
		mi_romid[0] = 0x00;
		return Read_RomID(mi_romid);	//search DS28E30
	} else {
		for (i = 0; i < 6; i++)
			temp |= mi_romid[1 + i];	//check if the device is power up at the first time

		if (temp == 0)	//power up the device, then read ROMID again
		{
			mi_romid[0] = 0x00;   //?????
			ds28e30_cmd_readStatus(pg, buf, MANID, HardwareVersion);	//page number=0
//			mi_romid[0] = 0x00;   //?????
			Read_RomID(mi_romid);	//read ROMID from DS28E30
			flag = ds28e30_cmd_readStatus(0x80 | pg, buf, MANID, HardwareVersion);	//page number=0
			return flag;
		} else {
			flag = ds28e30_cmd_readStatus(0x80 | pg, buf, MANID, HardwareVersion);	//page number=0
			return flag;
		}
	}
	return DS_FALSE;
}

//---------------------------------------------------------------------------
//setting expected MAN_ID, protection status, counter value, system-level public key, authority public key and certificate constants
void ConfigureDS28E30Parameters(void)
{
	//MI device
	Expected_CID[0] = Samsung_CID_LSB;
	Expected_CID[1] = Samsung_CID_MSB;
	Expected_MAN_ID[0] = Samsung_MAN_ID_LSB;
	Expected_MAN_ID[1] = Samsung_MAN_ID_MSB;
	//memcpy( Expected_PageProtectionStatus, MI_PageProtectionStatus ,11);

	memcpy(Certificate_Constant, Samsung_Certificate_Constant, sizeof(Samsung_Certificate_Constant));
	memcpy(SystemPublicKeyX, Samsung_SystemPublicKeyX, sizeof(Samsung_SystemPublicKeyX));
	memcpy(SystemPublicKeyY, Samsung_SystemPublicKeyY, sizeof(Samsung_SystemPublicKeyY));
	memcpy(AuthorityPublicKey_X, Samsung_AuthorityPublicKey_X, sizeof(Samsung_AuthorityPublicKey_X));
	memcpy(AuthorityPublicKey_Y, Samsung_AuthorityPublicKey_Y, sizeof(Samsung_AuthorityPublicKey_Y));
}

//---------------------------------------------------------------------------
/// Verify certificate of devices like DS28C36/DS28C36/DS28E38/DS28E30.
///
/// @param[in] sig_r
/// Buffer for R portion of certificate signature (MSByte first)
/// @param[in] sig_s
/// Buffer for S portion of certificate signature (MSByte first)
/// @param[in] pub_x
/// Public Key x to verify
/// @param[in] pub_y
/// Public Key y to verify
/// @param[in] SLAVE_ROMID
/// device's 64-bit ROMID (LSByte first)
/// @param[in] SLAVE_MANID
/// Maxim defined as manufacturing ID
/// @param[in] system_level_pub_key_x
/// 32-byte buffer container the system level public key x
/// @param[in] system_level_pub_key_y
/// 32-byte buffer container the system level public key y
///
///  @return
///  DS_TRUE - certificate valid @n
///  DS_FALSE - certificate not valid
///
int verifyECDSACertificateOfDevice(unsigned char *sig_r,
				   unsigned char *sig_s,
				   unsigned char *pub_key_x,
				   unsigned char *pub_key_y,
				   unsigned char *SLAVE_ROMID,
				   unsigned char *SLAVE_MANID,
				   unsigned char *system_level_pub_key_x,
				   unsigned char *system_level_pub_key_y)
{
	unsigned char buf[32];

	// setup software ECDSA computation
	deep_cover_coproc_setup(0, 0, 0, 0);

	// create customization field
	// 16 zeros (can be set to other customer specific value)
	memcpy(buf, Certificate_Constant, 16);
	// ROMID
	memcpy(&buf[16], SLAVE_ROMID, 8);
	// MANID
	memcpy(&buf[24], SLAVE_MANID, 2);
	return deep_cover_verifyECDSACertificate(sig_r, sig_s, pub_key_x,
						 pub_key_y, buf, 26,
						 system_level_pub_key_x,
						 system_level_pub_key_y);
}

//--------------------------------------------------------------------------
/// 'Compute and Read Page Authentication' command
///
/// @param[in] pg - page number to compute auth on
/// @param[in] anon - anonymous flag (1) for anymous
/// @param[in] challenge
/// buffer length must be at least 32 bytes containing the challenge
/// @param[out] data
/// buffer length must be at least 64 bytes to hold ECDSA signature
///
/// @return
/// DS_TRUE - command successful @n
/// DS_FALSE - command failed
///
int ds28e30_cmd_computeReadPageAuthentication(int pg, int anon,
					      unsigned char *challenge,
					      unsigned char *sig)
{
	unsigned char write_buf[200];
	int write_len;
	unsigned char read_buf[255];
	int read_len;
	/*
	   Reset
	   Presence Pulse
	   <ROM Select>
	   TX: XPC Command (66h)
	   TX: Length byte 34d 
	   TX: XPC sub-command A5h (Compute and Read Page Authentication)
	   TX: Parameter (page)
	   TX: Challenge (32d bytes)
	   RX: CRC16 (inverted of XPC command, length, sub-command, parameter, and challenge)
	   TX: Release Byte
	   <Delay TBD>
	   RX: Dummy Byte
	   RX: Length byte (65d)
	   RX: Result Byte
	   RX: Read ECDSA Signature (64 bytes, ‘s’ and then ‘r’, MSByte first, [same as ES10]), 
	   signature 00h's if result byte is not AA success
	   RX: CRC16 (inverted, length byte, result byte, and signature)
	   Reset or send XPC command (66h) for a new sequence
	 */

	// construct the write buffer
	write_len = 0;
	write_buf[write_len++] = CMD_COMP_READ_AUTH;
	write_buf[write_len] = pg & 0x7f;
	if (anon)
		write_buf[write_len] |= 0xE0;
	write_len++;
	write_buf[write_len++] = 0x03;	//authentication parameter
	memcpy(&write_buf[write_len], challenge, 32);
	write_len += 32;

	// preload read_len with expected length
	read_len = 65;

	// default failure mode 
	last_result_byte = RESULT_FAIL_NONE;

	// if(ds28e30_standard_cmd_flow(write_buf, DELAY_DS28E30_ECDSA_GEN_TGES, read_buf, &read_len, write_len))
	if (standard_cmd_flow
	    (write_buf, write_len, DELAY_DS28E30_ECDSA_GEN_TGES, read_len,
	     read_buf, &read_len)) {
		// get result byte
		last_result_byte = read_buf[0];
		// check result
		if (read_len == 65) {
			if (read_buf[0] == RESULT_SUCCESS) {
				memcpy(sig, &read_buf[1], 64);
				return DS_TRUE;
			}
		}
	}

	ow_reset();
	// no payload in read buffer or failed command
	return DS_FALSE;
}

//---------------------------------------------------------------------------
/// High level function to do a full challenge/response ECDSA operation
/// on specified page
///
/// @param[in] pg
/// page to do operation on
/// @param[in] anon
/// flag to indicate in anonymous mode (1) or not anonymous (0)
/// @param[in] mempage
/// buffer with memory page contents, required for verification of ECDSA signature
/// @param[in] challenge
/// buffer containing challenge, must be 32 bytes
/// @param[out] sig_r
/// buffer for r portion of signature, must be 32 bytes
/// @param[out] sig_s
/// buffer for s portion of signature, must be 32 bytes
///
/// @return
/// DS_TRUE - command successful @n
/// DS_FALSE - command failed
///
int ds28e30_computeAndVerifyECDSA_NoRead(int pg, int anon,
					 unsigned char *mempage,
					 unsigned char *challenge,
					 unsigned char *sig_r,
					 unsigned char *sig_s)
{
	unsigned char signature[64], message[256];
	int msg_len;
	unsigned char *pubkey_x, *pubkey_y;

	// compute and read auth command
	if (!ds28e30_cmd_computeReadPageAuthentication
	    (pg, anon, challenge, signature)) {
		return DS_FALSE;
	}
	// put the signature in the return buffers, signature is 's' and then 'r', MSByte first
	memcpy(sig_s, signature, 32);
	memcpy(sig_r, &signature[32], 32);

	// construct the message to hash for signature verification
	// ROM NO | Page Data | Challenge (Buffer) | Page# | MANID

	// ROM NO
	msg_len = 0;
	if (anon)
		memset(&message[msg_len], 0xFF, sizeof(message[msg_len]));
	else
		memcpy(&message[msg_len], mi_romid, 8);
	msg_len += 8;
	// Page Data
	memcpy(&message[msg_len], mempage, 32);
	msg_len += 32;
	// Challenge (Buffer)
	memcpy(&message[msg_len], challenge, 32);
	msg_len += 32;
	// Page#
	message[msg_len++] = pg;
	// MANID
	memcpy(&message[msg_len], MANID, 2);
	msg_len += 2;

	pubkey_x = public_key_x;
	pubkey_y = public_key_y;

	// verify Signature and return result
	return deep_cover_verifyECDSASignature(message, msg_len, pubkey_x,
					       pubkey_y, sig_r, sig_s);
}

//---------------------------------------------------------------------------
//-------- ds28e30 High level functions 
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// High level function to do a full challenge/response ECDSA operation 
/// on specified page 
///
/// @param[in] pg
/// page to do operation on
/// @param[in] anon
/// flag to indicate in anonymous mode (1) or not anonymous (0)
/// @param[out] mempage
/// buffer to return the memory page contents
/// @param[in] challenge
/// buffer containing challenge, must be 32 bytes
/// @param[out] sig_r
/// buffer for r portion of signature, must be 32 bytes 
/// @param[out] sig_s
/// buffer for s portion of signature, must be 32 bytes 
///
/// @return
/// DS_TRUE - command successful @n
/// DS_FALSE - command failed
///
int ds28e30_computeAndVerifyECDSA(int pg, int anon, unsigned char *mempage,
				  unsigned char *challenge,
				  unsigned char *sig_r,
				  unsigned char *sig_s)
{
	// read destination page
	// if (!ds28e30_cmd_readMemory(pg, mempage))
	if (TestingItemResult[Page0_Result] == DS_FALSE)
		return DS_FALSE;

	return ds28e30_computeAndVerifyECDSA_NoRead(pg, anon, mempage,
						    challenge, sig_r,
						    sig_s);
}

uint8_t batt_id;
int batt_full_status_usage;
int batt_asoc;
int fai_expired;
int sync_buf_mem_sts;
int sync_buf_mem;

int AuthenticateDS28E30(void)
{
	int i = 0, j = 0;
	unsigned char flag = DS_TRUE;
	unsigned char buf[128], Page0Data[32], DecrementCounter[3];
	unsigned char PageNumberInOrderForProtection[] =
	    { 0, 1, 2, 3, 4, 5, 6, 7, 28, 29, 36 };
	unsigned char DevicePublicKey_X[32], DevicePublicKey_Y[32];
	// unsigned char Page_Certificate_R[32],Page_Certificate_S[32];
	unsigned char sig_r[32], sig_s[32];
	// unsigned long long start_ticks_us = 0;
	// unsigned long long end_ticks_us = 0;
	// start_ticks_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);

#if 0
	if (mi_auth_result == DS_TRUE)
		return mi_auth_result;
#endif
    /**1、reading family code(romid), CID(page0 data), ROMID and MAN_ID(page0 status)**/
	TestingItemResult[Family_Code_Result] = DS_FALSE;
	TestingItemResult[Custom_ID_Result] = DS_FALSE;
	TestingItemResult[Unique_ID_Result] = DS_FALSE;
	TestingItemResult[MAN_ID_Result] = DS_FALSE;

	for (j = 0; j < READ_ROMNO_RETRY; j++) {
		flag = ds28e30_read_ROMNO_MANID_HardwareVersion();
		if (flag == DS_TRUE) {
			if (mi_romid[0] == 0xDB)	//0xdb: famliy code, 代表电池型号；
			{
				TestingItemResult[Family_Code_Result] = DS_TRUE;	//testing family code
				ConfigureDS28E30Parameters();
			}
			if (mi_romid[6] == Expected_CID[1]
			    && (mi_romid[5] & 0xF0) == Expected_CID[0]) {
				TestingItemResult[Custom_ID_Result] = DS_TRUE;	//testing Samsung CID=0x050----------0x05代表三星公司；
			}
			TestingItemResult[Unique_ID_Result] = DS_TRUE;	//unique ROMID CRC8 is correct in the current condition, 即romid

			if (MANID[0] == Expected_MAN_ID[0]
			    && MANID[1] == Expected_MAN_ID[1]) {
				TestingItemResult[MAN_ID_Result] = DS_TRUE;	//manufacturer id, 生产商；
			}
		}

		if ((TestingItemResult[Family_Code_Result] == DS_TRUE) &&
		    (TestingItemResult[Custom_ID_Result] == DS_TRUE) &&
		    (TestingItemResult[Unique_ID_Result] == DS_TRUE) &&
		    (TestingItemResult[MAN_ID_Result] == DS_TRUE)) {
			break;
		} else {
			ds_info("ds28e30_read_ROMNO_MANID_HardwareVersion 1 failed\n");
			continue;
		}
	}

	if (j >= READ_ROMNO_RETRY)
		return DS_FALSE;

	/**2、reading page status byte (page0~3, certificate page 4&5, authority pub page 6&7, device pub key page 28&29, private key page 36)**/
	TestingItemResult[Status_Result] = DS_FALSE;

	//依次读取各个page的protection status
	for (j = 0; j < READ_STATUS_RETRY; j++) {
		for (i = 0; i < 11; i++) {
			flag =
			    ds28e30_cmd_readStatus
			    (PageNumberInOrderForProtection[i], buf, MANID,
			     HardwareVersion);
			if (flag == DS_FALSE)
				break;
		}

		if (i == 11) {
			TestingItemResult[Status_Result] = DS_TRUE;
			ds_dbg("mi_status data:\n");
			ds_dbg("%02x %02x %02x\n", mi_status[0],
				mi_status[1], mi_status[2]);
			break;
		} else {
			ds_info("ds28e30_cmd_readStatus failed\n");
			continue;
		}
	}

	for (j = 0; j < READ_ROMNO_RETRY; j++) {
		flag = ds28e30_read_ROMNO_MANID_HardwareVersion();
		if (flag == DS_TRUE) {
			if (mi_romid[0] == 0xDB)	//0xdb: famliy code, 代表电池型号；
			{
				TestingItemResult[Family_Code_Result] = DS_TRUE;	//testing family code
				ConfigureDS28E30Parameters();
			}
			if (mi_romid[6] == Expected_CID[1]
			    && (mi_romid[5] & 0xF0) == Expected_CID[0]) {
				TestingItemResult[Custom_ID_Result] = DS_TRUE;	//testing Samsung CID=0x050----------0x05代表三星公司；
			}
			TestingItemResult[Unique_ID_Result] = DS_TRUE;	//unique ROMID CRC8 is correct in the current condition, 即romid

			if (MANID[0] == Expected_MAN_ID[0]
			    && MANID[1] == Expected_MAN_ID[1]) {
				TestingItemResult[MAN_ID_Result] = DS_TRUE;	//manufacturer id, 生产商；
			}
		}

		if ((TestingItemResult[Family_Code_Result] == DS_TRUE) &&
		    (TestingItemResult[Custom_ID_Result] == DS_TRUE) &&
		    (TestingItemResult[Unique_ID_Result] == DS_TRUE) &&
		    (TestingItemResult[MAN_ID_Result] == DS_TRUE)) {
			break;
		} else {
			ds_info("ds28e30_read_ROMNO_MANID_HardwareVersion 2 failed\n");
			continue;
		}
	}

	//reading page 0 data, page0放的是IC SN，是由battery SN转换而来的；
	for (j = 0; j < READ_PAGEDATA0_RETRY; j++) {
		TestingItemResult[Page0_Result] = DS_FALSE;
		flag = ds28e30_cmd_readMemory(PG_USER_EEPROM_0, buf);
		if (flag == DS_TRUE) {
			memcpy(Page0Data, buf, 32);
			TestingItemResult[Page0_Result] = DS_TRUE;
			ds_dbg("Page0Data:\n");
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				Page0Data[0], Page0Data[1], Page0Data[2],
				Page0Data[3], Page0Data[4], Page0Data[5],
				Page0Data[6], Page0Data[7]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				Page0Data[8], Page0Data[9], Page0Data[10],
				Page0Data[11], Page0Data[12],
				Page0Data[13], Page0Data[14],
				Page0Data[15]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				Page0Data[16], Page0Data[17],
				Page0Data[18], Page0Data[19],
				Page0Data[20], Page0Data[21],
				Page0Data[22], Page0Data[23]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				Page0Data[24], Page0Data[25],
				Page0Data[26], Page0Data[27],
				Page0Data[28], Page0Data[29],
				Page0Data[30], Page0Data[31]);
			if (Page0Data[13] == SCUD && Page0Data[14] == BYD)		//NVT(NV)--first_supplier
				batt_id = BATTERY_VENDOR_FIRST;
			else if (Page0Data[13] == SCUD && Page0Data[14] == QIANFENG)  //swd--second_supplier
				batt_id = BATTERY_VENDOR_THIRD;
			else if (Page0Data[13] == GY_SN1 || Page0Data[0] == GY_SN2)		//冠宇(G)--third_supplier
				batt_id = BATTERY_VENDOR_SECOND;
			else
				batt_id = BATTERY_VENDOR_UNKNOW;
			memcpy(mi_page0_data, Page0Data, 32);
			break;
		} else {
			ds_info("readMemory page0 failed\n");
			continue;
		}
	}

	//reading Counter/page 106 data if counter=expected counter value，page106是递减计数器，0x1FFFF for Samsung
	TestingItemResult[CounterValue_Result] = DS_FALSE;
	for (j = 0; j < READ_DEC_COUNTER_RETRY; j++) {
		flag = ds28e30_cmd_readMemory(PG_DEC_COUNTER, buf);
		if (flag == DS_TRUE) {
			memcpy(DecrementCounter, buf, 3);
			TestingItemResult[CounterValue_Result] = DS_TRUE;
			ds_dbg("DecrementCounter:\n");
			ds_dbg("%02x %02x %02x\n",
				DecrementCounter[0], DecrementCounter[1],
				DecrementCounter[2]);
			break;
		} else {
			ds_info("readMemory page106 failed\n");
			continue;
		}
	}

    /**3、verifying Device's digital signatre 验证数字签名**/
	for (j = 0; j < VERIFY_SIGNATURE_RETRY; j++) {
		TestingItemResult[Device_publickey_Result] = DS_FALSE;
		flag = ds28e30_cmd_readDevicePublicKey(buf);	//read device public key X&Y, 第28、29 page存放公钥；
		if (flag == DS_TRUE) {
			TestingItemResult[Device_publickey_Result] =
			    DS_TRUE;
			memcpy(DevicePublicKey_X, buf, 32);	//reserve device public key X
			memcpy(DevicePublicKey_Y, &buf[32], 32);	//reserve device public key Y
			//prepare to verify the signature
			memcpy(public_key_x, DevicePublicKey_X, 32);	//copy device public key X to public key x buffer
			memcpy(public_key_y, DevicePublicKey_Y, 32);	//copy device public key Y to public key x buffer
			ds_dbg("page28Data:\n");
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				buf[0], buf[1], buf[2], buf[3],
				buf[4], buf[5], buf[6], buf[7]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				buf[8], buf[9], buf[10], buf[11],
				buf[12], buf[13], buf[14], buf[15]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				buf[16], buf[17], buf[18], buf[19],
				buf[20], buf[21], buf[22], buf[23]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				buf[24], buf[25], buf[26], buf[27],
				buf[28], buf[29], buf[30], buf[31]);
			ds_dbg("page29Data:\n");
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				buf[0 + 32], buf[1 + 32], buf[2 + 32],
				buf[3 + 32], buf[4 + 32], buf[5 + 32],
				buf[6 + 32], buf[7 + 32]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				buf[8 + 32], buf[9 + 32], buf[10 + 32],
				buf[11 + 32], buf[12 + 32], buf[13 + 32],
				buf[14 + 32], buf[15 + 32]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				buf[16 + 32], buf[17 + 32], buf[18 + 32],
				buf[19 + 32], buf[20 + 32], buf[21 + 32],
				buf[22 + 32], buf[23 + 32]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				buf[24 + 32], buf[25 + 32], buf[26 + 32],
				buf[27 + 32], buf[28 + 32], buf[29 + 32],
				buf[30 + 32], buf[31 + 32]);
			break;
		} else {
			ds_info("readMemory page28/29 failed\n");
			continue;
		}
	}
	for (j = 0; j < VERIFY_SIGNATURE_RETRY; j++) {
		TestingItemResult[Verification_Signature_Result] =
		    DS_FALSE;

		// for(i = 0; i < 32; i++) 
		//    challenge[i] = 0xaa;    //use a real challenge to display this line!!!!!

		if (TestingItemResult[Device_publickey_Result] == DS_TRUE) {
			//setup software ECDSA computation, ECDSA初始化
			deep_cover_coproc_setup(0, 0, 0, 0);
			//Verify the digital signature for the device
			TestingItemResult[Verification_Signature_Result] = ds28e30_computeAndVerifyECDSA(0, 0, Page0Data, challenge, sig_r, sig_s);	//page number=0
			// TestingItemResult[Verification_Signature_Result] = DS_TRUE;
			if (TestingItemResult
			    [Verification_Signature_Result] == DS_TRUE) {
				ds_info("ds28e30_computeAndVerifyECDSA successful\n");
				break;
			} else {
				ds_info("ds28e30_computeAndVerifyECDSA failed\n");
				continue;
			}
		}
	}
    /**4、verifying Device's certificate 验证ECDSA证书**/
	for (j = 0; j < VERIFY_CERTIFICATE_RETRY; j++) {
		TestingItemResult[Device_certificate_Result] = DS_FALSE;
		if ((ds28e30_cmd_readMemory(PG_CERTIFICATE_R, Page_Certificate_R)) == DS_TRUE && (ds28e30_cmd_readMemory(PG_CERTIFICATE_S, Page_Certificate_S)) == DS_TRUE)	//read device Certificate R&S in page 0/1
		{
			TestingItemResult[Device_certificate_Result] =
			    DS_TRUE;
			ds_dbg("page4Data:\n");
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				Page_Certificate_R[0],
				Page_Certificate_R[1],
				Page_Certificate_R[2],
				Page_Certificate_R[3],
				Page_Certificate_R[4],
				Page_Certificate_R[5],
				Page_Certificate_R[6],
				Page_Certificate_R[7]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				Page_Certificate_R[8],
				Page_Certificate_R[9],
				Page_Certificate_R[10],
				Page_Certificate_R[11],
				Page_Certificate_R[12],
				Page_Certificate_R[13],
				Page_Certificate_R[14],
				Page_Certificate_R[15]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				Page_Certificate_R[16],
				Page_Certificate_R[17],
				Page_Certificate_R[18],
				Page_Certificate_R[19],
				Page_Certificate_R[20],
				Page_Certificate_R[21],
				Page_Certificate_R[22],
				Page_Certificate_R[23]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				Page_Certificate_R[24],
				Page_Certificate_R[25],
				Page_Certificate_R[26],
				Page_Certificate_R[27],
				Page_Certificate_R[28],
				Page_Certificate_R[29],
				Page_Certificate_R[30],
				Page_Certificate_R[31]);
			ds_dbg("page5Data:\n");
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				Page_Certificate_S[0],
				Page_Certificate_S[1],
				Page_Certificate_S[2],
				Page_Certificate_S[3],
				Page_Certificate_S[4],
				Page_Certificate_S[5],
				Page_Certificate_S[6],
				Page_Certificate_S[7]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				Page_Certificate_S[8],
				Page_Certificate_S[9],
				Page_Certificate_S[10],
				Page_Certificate_S[11],
				Page_Certificate_S[12],
				Page_Certificate_S[13],
				Page_Certificate_S[14],
				Page_Certificate_S[15]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				Page_Certificate_S[16],
				Page_Certificate_S[17],
				Page_Certificate_S[18],
				Page_Certificate_S[19],
				Page_Certificate_S[20],
				Page_Certificate_S[21],
				Page_Certificate_S[22],
				Page_Certificate_S[23]);
			ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
				Page_Certificate_S[24],
				Page_Certificate_S[25],
				Page_Certificate_S[26],
				Page_Certificate_S[27],
				Page_Certificate_S[28],
				Page_Certificate_S[29],
				Page_Certificate_S[30],
				Page_Certificate_S[31]);
			break;
		} else {
			ds_info("readMemory page4/5 failed\n");
			continue;
		}
	}
#if 1
	for (j = 0; j < VERIFY_CERTIFICATE_RETRY; j++) {
		TestingItemResult[Verification_Certificate_Result] =
		    DS_FALSE;
		if (TestingItemResult[Device_certificate_Result] ==
		    DS_TRUE) {
			TestingItemResult[Verification_Certificate_Result]
			    =
			    verifyECDSACertificateOfDevice
			    (Page_Certificate_R, Page_Certificate_S,
			     DevicePublicKey_X, DevicePublicKey_Y,
			     mi_romid, MANID, SystemPublicKeyX,
			     SystemPublicKeyY);
			if (TestingItemResult
			    [Verification_Certificate_Result] == DS_TRUE) {
				break;
			} else {
				ds_info("verifyECDSACertificateOfDevice failed\n");
				continue;
			}
		}
	}
#endif

	// end_ticks_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
	// ds_info("start_ticks_us=(%lu), end_ticks_us =(%lu), diff_us=(%lu)",
	//      start_ticks_us, end_ticks_us, (end_ticks_us - start_ticks_us) );
	if ((TestingItemResult[Family_Code_Result] == DS_TRUE) &&
	    (TestingItemResult[Custom_ID_Result] == DS_TRUE) &&
	    (TestingItemResult[Unique_ID_Result] == DS_TRUE) &&
	    (TestingItemResult[MAN_ID_Result] == DS_TRUE) &&
	    (TestingItemResult[Status_Result] == DS_TRUE) &&
	    (TestingItemResult[Page0_Result] == DS_TRUE) &&
	    (TestingItemResult[CounterValue_Result] == DS_TRUE) &&
	    (TestingItemResult[Verification_Signature_Result] == DS_TRUE)
	    && (TestingItemResult[Verification_Certificate_Result] ==
		DS_TRUE)) {
		mi_auth_result = 1;
		ds_info("mi_auth_result = %d\n", mi_auth_result);

		return DS_TRUE;
	} else {
		mi_auth_result = 0;
		ds_info("mi_auth_result = %d\n", mi_auth_result);
		return DS_FALSE;
	}
}

/*
* 'Read Status' command
*
*  @param[in] pg
*  page to read protection
*  @param[out] pr_data
*  pointer to unsigned char buffer of length 6 for page protection data
*  @param[out] manid
*  pointer to unsigned char buffer of length 2 for manid (manufactorur ID)
*
*  @return
*  DS_TRUE - command successful @n
*  DS_FALSE - command failed
*/
int ds28e30_cmd_readStatus(int pg, unsigned char *pr_data, unsigned char *manid, unsigned char *hardware_version)	//-----------------------
{
	unsigned char write_buf[10];
	unsigned char read_buf[255];
	int read_len = 2, write_len;
	int PageNumbers[] = { 0, 1, 2, 3, 4, 5, 6, 7, 28, 29, 36, 106 };
	int i;

	/* 
	   Reset
	   Presence Pulse
	   <ROM Select>
	   TX: XPC Command (66h)
	   TX: Length byte 1d 
	   TX: XPC sub-command AAh (Read Status)
	   RX: CRC16 (inverted of XPC command, length, and sub-command)
	   TX: Release Byte
	   <Delay TBD>
	   RX: Dummy Byte
	   RX: Length Byte (11d)
	   RX: Result Byte
	   RX: Read protection values (6 Bytes), MANID (2 Bytes), ROM VERSION (2 bytes)
	   RX: CRC16 (inverted, length byte, protection values, MANID, ROM_VERSION)
	   Reset or send XPC command (66h) for a new sequence
	 */

	// construct the write buffer
	write_len = 0;
	write_buf[write_len++] = CMD_READ_STATUS;
	write_buf[write_len++] = pg;

	// preload read_len with expected length
	if (pg & 0x80)
		read_len = 5;

	// default failure mode 
	last_result_byte = RESULT_FAIL_NONE;


	// if(ds28e30_standard_cmd_flow(write_buf, DELAY_DS28E30_EE_READ_TRM, read_buf, &read_len, write_len))
	if (standard_cmd_flow
	    (write_buf, write_len, DELAY_DS28E30_EE_READ_TRM, read_len,
	     read_buf, &read_len)) {
		// get result byte
		last_result_byte = read_buf[0];
		// should always be 2 or 5 length for status data
		if (read_len == 2 || read_len == 5) {
			if (read_buf[0] == RESULT_SUCCESS
			    || read_buf[0] == RESULT_FAIL_DEVICEDISABLED) {
				if (read_len == 2) {
					memcpy(pr_data, &read_buf[1], 8);
					*pr_data = read_buf[1];
					pr_data[0] = read_buf[1];
					for (i = 0;
					     i < ARRAY_SIZE(PageNumbers);
					     i++) {
						if (pg == PageNumbers[i]) {
							if (i <
							    ARRAY_SIZE
							    (mi_status)) {
								flag_mi_status
								    = 1;
								mi_status
								    [i] =
								    read_buf
								    [1];
							}
						}
					}
				} else {
					memcpy(manid, &read_buf[1], 2);
					memcpy(hardware_version,
					       &read_buf[3], 2);
					memcpy(MANID, &read_buf[1], 2);
					memcpy(HardwareVersion,
					       &read_buf[3], 2);
					ds_dbg("page%d manid:\n",
						pg & 0x7f);
					ds_dbg("%02x %02x\n",
						manid[0], manid[1]);
					ds_dbg
					    ("page%d hardware_version:\n",
					     pg & 0x7f);
					ds_dbg("%02x %02x\n",
						hardware_version[0],
						hardware_version[1]);
				}
				// ds_info("[llt--------------true----------------] ds28e30_cmd_readStatus--\n");
				return DS_TRUE;
			}
		}
	}

	ow_reset();
	// no payload in read buffer or failed command
	return DS_FALSE;
}

////////////////////////////////////////////////////////////////////////////////
/*!
* @brief maxim_authentic_check()
* @detailed
* This function check maxim authentic result
* @return int DS_FALSE/ DS_TRUE/ ERROR_UNMATCH_MAC
*/
////////////////////////////////////////////////////////////////////////////////
int maxim_authentic_check(void)
{
	if (flag_mi_auth_result)
		return mi_auth_result;

	return DS_FALSE;
}

////////////////////////////////////////////////////////////////////////////////
/*!
* @brief maxim_authentic_start(void)
* @detailed
* This function start maxim authentic
* @return bool true if success
*/
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//-------- ds28e30 Memory functions 
//---------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// 'Write Memory' command
///
/// @param[in] pg
/// page number to write
/// @param[in] data
/// buffer must be at least 32 bytes
///
///  @return
///  DS_TRUE - command successful @n
///  DS_FALSE - command failed
///
int ds28e30_cmd_writeMemory(int pg, unsigned char *data)
{
	unsigned char write_buf[50];
	int write_len;
	unsigned char read_buf[255];
	int read_len;

	/*
	   Reset
	   Presence Pulse
	   <ROM Select>
	   TX: XPC Command (66h)
	   TX: Length byte 34d
	   TX: XPC sub-command 96h (Write Memory)
	   TX: Parameter
	   TX: New page data (32d bytes)
	   RX: CRC16 (inverted of XPC command, length, sub-command, parameter)
	   TX: Release Byte
	   <Delay TBD>
	   RX: Dummy Byte
	   RX: Length Byte (1d)
	   RX: Result Byte
	   RX: CRC16 (inverted of length and result byte)
	   Reset or send XPC command (66h) for a new sequence
	 */

	// construct the write buffer
	write_len = 0;
	write_buf[write_len++] = CMD_WRITE_MEM;
	write_buf[write_len++] = pg;
	memcpy(&write_buf[write_len], data, 32);
	write_len += 32;

	// preload read_len with expected length
	read_len = 1;

	// default failure mode
	last_result_byte = RESULT_FAIL_NONE;

	// if(ds28e30_standard_cmd_flow(write_buf, DELAY_DS28E30_EE_WRITE_TWM, read_buf, &read_len, write_len))
	if (standard_cmd_flow
	    (write_buf, write_len, DELAY_DS28E30_EE_WRITE_TWM, read_len,
	     read_buf, &read_len)) {
		// get result byte
		last_result_byte = read_buf[0];
		// check result
		if (read_len == 1)
			return (read_buf[0] == RESULT_SUCCESS);
	}
	//ow_reset();
	// no payload in read buffer or failed command
	return DS_FALSE;
}

//--------------------------------------------------------------------------
/// 'Read Memory' command
///
/// @param[in] pg
/// page number to read
/// @param[out] data
/// buffer length must be at least 32 bytes to hold memory read
///
///  @return
///  DS_TRUE - command successful @n
///  DS_FALSE - command failed
///
int ds28e30_cmd_readMemory(int pg, unsigned char *data)
{
	unsigned char write_buf[10];
	int write_len;
	unsigned char read_buf[255];
	int read_len;

	/*
	   Reset
	   Presence Pulse
	   <ROM Select>
	   TX: XPC Command (66h)
	   TX: Length byte 2d
	   TX: XPC sub-command 69h (Read Memory)
	   TX: Parameter (page)
	   RX: CRC16 (inverted of XPC command, length, sub-command, and parameter)
	   TX: Release Byte
	   <Delay TBD>
	   RX: Dummy Byte
	   RX: Length (33d)
	   RX: Result Byte
	   RX: Read page data (32d bytes)
	   RX: CRC16 (inverted, length byte, result byte, and page data)
	   Reset or send XPC command (66h) for a new sequence
	 */

	// construct the write buffer
	write_len = 0;
	write_buf[write_len++] = CMD_READ_MEM;
	write_buf[write_len++] = pg;

	// preload read_len with expected length
	read_len = 33;

	// default failure mode
	last_result_byte = RESULT_FAIL_NONE;

	// if(ds28e30_standard_cmd_flow(write_buf, DELAY_DS28E30_EE_READ_TRM, read_buf, &read_len, write_len))
	if (standard_cmd_flow
	    (write_buf, write_len, DELAY_DS28E30_EE_READ_TRM, read_len,
	     read_buf, &read_len)) {
		// get result byte
		last_result_byte = read_buf[0];
		// check result
		if (read_len == 33) {
			if (read_buf[0] == RESULT_SUCCESS) {
				memcpy(data, &read_buf[1], 32);

				if (pg == 0) {
					flag_mi_page0_data = 1;
					memcpy(mi_page0_data, data, 32);
				}
				if (pg == 1) {
					flag_mi_page1_data = 1;
					memcpy(mi_page1_data, data, 32);
				}
				if (pg == 106) {
					flag_mi_counter = 1;
					memcpy(mi_counter, data, 32);
				}
				// ds_info("[llt---------------true---------------] DS28E30_cmd_readMemory--\n");
				return DS_TRUE;
			}
		}
	}
	//ow_reset();
	// ds_info("[llt--------------false----------------] DS28E30_cmd_readMemory--\n");
	// no payload in read buffer or failed command
	return DS_FALSE;
}

//--------------------------------------------------------------------------
/// 'Set Page Protection' command
///
/// @param[in] pg
/// page to set protection
/// @param[in] prot
/// protection value to set
///
///  @return
///  DS_TRUE - command successful @n
///  DS_FALSE - command failed
///
int ds28e30_cmd_setPageProtection(int pg, unsigned char prot)
{
	unsigned char write_buf[10];
	int write_len;
	unsigned char read_buf[255];
	int read_len;

	/*
	   Reset
	   Presence Pulse
	   <ROM Select>
	   TX: XPC Command (66h)
	   TX: Length byte 3d 
	   TX: XPC sub-command C3h (Set Protection)
	   TX: Parameter (page)
	   TX: Parameter (protection)
	   RX: CRC16 (inverted of XPC command, length, sub-command, parameters)
	   TX: Release Byte
	   <Delay TBD>
	   RX: Dummy Byte
	   RX: Length Byte (1d)
	   RX: Result Byte
	   RX: CRC16 (inverted, length byte and result byte)
	   Reset or send XPC command (66h) for a new sequence
	 */

	// construct the write buffer
	write_len = 0;
	write_buf[write_len++] = CMD_SET_PAGE_PROT;
	write_buf[write_len++] = pg;
	write_buf[write_len++] = prot;

	// preload read_len with expected length
	read_len = 1;

	// default failure mode 
	last_result_byte = RESULT_FAIL_NONE;

	// if(ds28e30_standard_cmd_flow(write_buf, DELAY_DS28E30_EE_WRITE_TWM, read_buf, &read_len, write_len))
	if (standard_cmd_flow
	    (write_buf, write_len, DELAY_DS28E30_EE_WRITE_TWM, read_len,
	     read_buf, &read_len)) {
		// get result byte
		last_result_byte = read_buf[0];
		// check result
		if (read_len == 1)
			return (read_buf[0] == RESULT_SUCCESS);
	}

	ow_reset();
	// no payload in read buffer or failed command
	return DS_FALSE;
}

//--------------------------------------------------------------------------
/// 'Decrement Counter' command
///
///  @return
///  DS_TRUE - command successful @n
///  DS_FALSE - command failed
///
int ds28e30_cmd_decrementCounter(void)
{
	int write_len;
	unsigned char write_buf[10];
	unsigned char read_buf[255];
	int read_len;

	/*
	   Presence Pulse
	   <ROM Select>
	   TX: XPC Command (66h)
	   TX: Length byte 1d 
	   TX: XPC sub-command C9h (Decrement Counter)
	   RX: CRC16 (inverted of XPC command, length, sub-command)
	   TX: Release Byte
	   <Delay TBD>
	   RX: Dummy Byte
	   RX: Length Byte (1d)
	   RX: Result Byte
	   RX: CRC16 (inverted, length byte and result byte)
	   Reset or send XPC command (66h) for a new sequence
	 */

	// construct the write buffer
	write_len = 0;
	write_buf[write_len++] = CMD_DECREMENT_CNT;

	// preload read_len with expected length
	read_len = 1;

	// default failure mode 
	last_result_byte = RESULT_FAIL_NONE;

	// if(ds28e30_standard_cmd_flow(write_buf, DELAY_DS28E30_EE_WRITE_TWM+50, read_buf, &read_len, write_len))
	if (standard_cmd_flow
	    (write_buf, write_len, DELAY_DS28E30_EE_WRITE_TWM + 50,
	     read_len, read_buf, &read_len)) {
		// get result byte
		last_result_byte = read_buf[0];
		// check result byte
		if (read_len == 1)
			return (read_buf[0] == RESULT_SUCCESS);
	}

	ow_reset();
	// no payload in read buffer or failed command
	return DS_FALSE;
}

//--------------------------------------------------------------------------
/// 'Device Disable' command
///
/// @param[in] release_sequence
/// 8 byte release sequence to disable device
///
///  @return
///  DS_TRUE - command successful @n
///  DS_FALSE - command failed
///
int ds28e30_cmd_DeviceDisable(unsigned char *release_sequence)
{
	unsigned char write_buf[10];
	int write_len;
	unsigned char read_buf[255];
	int read_len;

	/*
	   Reset
	   Presence Pulse
	   <ROM Select>
	   TX: XPC Command (66h)
	   TX: Length byte 9d 
	   TX: XPC sub-command 33h (Disable command)
	   TX: Release Sequence (8 bytes)
	   RX: CRC16 (inverted of XPC command, length, sub-command, and release sequence)
	   TX: Release Byte
	   <Delay TBD>
	   RX: Dummy Byte
	   RX: Length Byte (1d)
	   RX: Result Byte
	   RX: CRC16 (inverted, length byte and result byte)
	   Reset or send XPC command (66h) for a new sequence
	 */

	// construct the write buffer
	write_len = 0;
	write_buf[write_len++] = CMD_DISABLE_DEVICE;
	memcpy(&write_buf[write_len], release_sequence, 8);
	write_len += 8;

	// preload read_len with expected length
	read_len = 1;

	// default failure mode 
	last_result_byte = RESULT_FAIL_NONE;

	// if(ds28e30_standard_cmd_flow(write_buf, DELAY_DS28E30_EE_WRITE_TWM, read_buf, &read_len, write_len))
	if (standard_cmd_flow
	    (write_buf, write_len, DELAY_DS28E30_EE_WRITE_TWM, read_len,
	     read_buf, &read_len)) {
		// get result byte
		last_result_byte = read_buf[0];
		// check result
		if (read_len == 1)
			return (read_buf[0] == RESULT_SUCCESS);
	}

	ow_reset();
	// no payload in read buffer or failed command
	return DS_FALSE;
}

//--------------------------------------------------------------------------
/// 'authenticated Write memory' command
///
/// @param[in] pg
/// page number to write
/// @param[in] data
/// buffer must be at least 32 bytes
/// @param[in] certificate sig_r
/// @param[in] certificate sig_s

///
///  @return
///  DS_TRUE - command successful @n
///  DS_FALSE - command failed
///
int ds28e30_cmd_authendicatedECDSAWriteMemory(int pg, unsigned char *data,
					      unsigned char *sig_r,
					      unsigned char *sig_s)
{
	unsigned char write_buf[128];
	int write_len;
	unsigned char read_buf[16];
	int read_len;

	/*
	   Reset
	   Presence Pulse
	   <ROM Select>
	   TX: XPC Command (66h)
	   TX: Length byte 98d
	   TX: XPC sub-command 89h (authenticated Write Memory)
	   TX: Parameter
	   TX: New page data (32d bytes)
	   TX: Certificate R&S (64 bytes)
	   RX: CRC16 (inverted of XPC command, length, sub-command, parameter, page data, certificate R&S)
	   TX: Release Byte
	   <Delay TBD>
	   RX: Dummy Byte
	   RX: Length Byte (1d)
	   RX: Result Byte
	   RX: CRC16 (inverted of length and result byte)
	   Reset or send XPC command (66h) for a new sequence
	 */

	// construct the write buffer
	write_len = 0;
	write_buf[write_len++] = CMD_AUTHENTICATE_WRITE;
	write_buf[write_len++] = pg & 0x03;
	memcpy(&write_buf[write_len], data, 32);
	write_len += 32;
	memcpy(&write_buf[write_len], sig_r, 32);
	write_len += 32;
	memcpy(&write_buf[write_len], sig_s, 32);
	write_len += 32;


	// preload read_len with expected length
	read_len = 1;

	// default failure mode
	last_result_byte = RESULT_FAIL_NONE;

	// if(ds28e30_standard_cmd_flow(write_buf, DELAY_DS28E30_EE_WRITE_TWM + DELAY_DS28E30_Verify_ECDSA_Signature_tEVS, read_buf, &read_len, write_len))
	if (standard_cmd_flow
	    (write_buf, write_len,
	     DELAY_DS28E30_EE_WRITE_TWM +
	     DELAY_DS28E30_Verify_ECDSA_Signature_tEVS, read_len, read_buf,
	     &read_len)) {
		// get result byte
		last_result_byte = read_buf[0];
		// check result
		if (read_len == 1)
			return (read_buf[0] == RESULT_SUCCESS);
	}

	ow_reset();
	// no payload in read buffer or failed command
	return DS_FALSE;
}

//--------------------------------------------------------------------------
/// 'Read device public key' command
///
/// @param[in]
/// no param required
/// @param[out] data
/// buffer length must be at least 64 bytes to hold device public key read
///
///  @return
///  DS_TRUE - command successful @n
///  DS_FALSE - command failed
///
int ds28e30_cmd_readDevicePublicKey(unsigned char *data)
{
	if ((ds28e30_cmd_readMemory(PG_DS28E30_PUB_KEY_X, data)) ==
	    DS_FALSE)
		return DS_FALSE;
	if ((ds28e30_cmd_readMemory(PG_DS28E30_PUB_KEY_Y, data + 32)) ==
	    DS_FALSE)
		return DS_FALSE;
	return DS_TRUE;
}


//--------------------------------------------------------------------------
/// write EEPROM page 1 as an example
///
int ds28e30_func_write_pg1(int pg, unsigned char *data,
			   unsigned char *sig_r, unsigned char *sig_s)
{
	short i;
	unsigned char buf[128];
	unsigned char flag = DS_TRUE;

	TestingItemResult[Program_Page1_Result] = DS_FALSE;
	memset(buf, 0x55, sizeof(buf));	//set page 1 to all 0x55, just for testing, displace it with your own page 1 data
	for (i = 0; i < WRITE_PG1_RETRY; i++) {
		flag = ds28e30_cmd_writeMemory(PG_USER_EEPROM_1, buf);	//User page 1
		TestingItemResult[Program_Page1_Result] = flag;
		if (flag == DS_FALSE)
			continue;
		flag = ds28e30_cmd_readMemory(PG_USER_EEPROM_1, &buf[32]);	//User page 1
		if ((memcmp(buf, &buf[32], 32)) != 0) {
			TestingItemResult[Program_Page1_Result] = DS_FALSE;
			continue;
		} else {
			TestingItemResult[Program_Page1_Result] = DS_TRUE;
			break;
		}
	}

	return DS_TRUE;
}

//---------------------------------------------------------------------------
//-------- ds28e30 Helper functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// Return last result byte. Useful if a function fails.
///
/// @return
/// Result byte
///
unsigned char ds28e30_getLastResultByte(void)
{
	return last_result_byte;
}

////////////////////////////////////////////////////////////////////////////////
/*
* retry interface for read romid 
*/
//////////////////////////////////////////////////////////////////////////////// 
int ds28e30_Read_RomID_retry(unsigned char *RomID)
{
	int i, pg;
	unsigned char data[50];

	pg = 0;
	for (i = 0; i < GET_ROM_ID_RETRY; i++) {
		if (ds28e30_cmd_readStatus
		    (pg, data, MANID, HardwareVersion) == DS_TRUE) {
			if (Read_RomID(RomID) == DS_TRUE) {
				return DS_TRUE;
			}
		}
	}

	return DS_FALSE;
}

EXPORT_SYMBOL(ds28e30_Read_RomID_retry);

////////////////////////////////////////////////////////////////////////////////
/*
* retry interface for read page data
*/
////////////////////////////////////////////////////////////////////////////////
int ds28e30_get_page_data_retry(int page, unsigned char *data)
{
	int i;

	if (page >= MAX_PAGENUM)
	return DS_FALSE;

	for (i = 0; i < GET_USER_MEMORY_RETRY; i++) {
		if (ds28e30_cmd_readMemory(page, data) == DS_TRUE) {
			if (page == 0) {
				ds_dbg("mi_page0_data data:\n");
				ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
					mi_page0_data[0], mi_page0_data[1],
					mi_page0_data[2], mi_page0_data[3],
					mi_page0_data[4], mi_page0_data[5],
					mi_page0_data[6], mi_page0_data[7]);
				ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
					mi_page0_data[8], mi_page0_data[9],
					mi_page0_data[10], mi_page0_data[11],
					mi_page0_data[12], mi_page0_data[13],
					mi_page0_data[14], mi_page0_data[15]);
				ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
					mi_page0_data[16], mi_page0_data[17],
					mi_page0_data[18], mi_page0_data[19],
					mi_page0_data[20], mi_page0_data[21],
					mi_page0_data[22], mi_page0_data[23]);
				ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
					mi_page0_data[24], mi_page0_data[25],
					mi_page0_data[26], mi_page0_data[27],
					mi_page0_data[28], mi_page0_data[29],
					mi_page0_data[30], mi_page0_data[31]);
			} else if (page == 1) {
				ds_dbg("mi_page1_data data:\n");
				ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
					mi_page1_data[0], mi_page1_data[1],
					mi_page1_data[2], mi_page1_data[3],
					mi_page1_data[4], mi_page1_data[5],
					mi_page1_data[6], mi_page1_data[7]);
				ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
					mi_page1_data[8], mi_page1_data[9],
					mi_page1_data[10], mi_page1_data[11],
					mi_page1_data[12], mi_page1_data[13],
					mi_page1_data[14], mi_page1_data[15]);
				ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
					mi_page1_data[16], mi_page1_data[17],
					mi_page1_data[18], mi_page1_data[19],
					mi_page1_data[20], mi_page1_data[21],
					mi_page1_data[22], mi_page1_data[23]);
				ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
					mi_page1_data[24], mi_page1_data[25],
					mi_page1_data[26], mi_page1_data[27],
					mi_page1_data[28], mi_page1_data[29],
					mi_page1_data[30], mi_page1_data[31]);
			} else {
				ds_dbg("mi_counter data:\n");
				ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
					mi_counter[0], mi_counter[1],
					mi_counter[2], mi_counter[3],
					mi_counter[4], mi_counter[5],
					mi_counter[6], mi_counter[7]);
				ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
					mi_counter[8], mi_counter[9],
					mi_counter[10], mi_counter[11],
					mi_counter[12], mi_counter[13],
					mi_counter[14], mi_counter[15]);
				ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
					mi_counter[16], mi_counter[17],
					mi_counter[18], mi_counter[19],
					mi_counter[20], mi_counter[21],
					mi_counter[22], mi_counter[23]);
				ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
					mi_counter[24], mi_counter[25],
					mi_counter[26], mi_counter[27],
					mi_counter[28], mi_counter[29],
					mi_counter[30], mi_counter[31]);
			}
			return DS_TRUE;
		}
	}

	return DS_FALSE;
}
EXPORT_SYMBOL(ds28e30_get_page_data_retry);

int ds28e30_get_page_status_retry(unsigned char *data)
{
	int i, pg;

	pg = 0;
	for (i = 0; i < GET_BLOCK_STATUS_RETRY; i++) {
		if (ds28e30_cmd_readStatus
		    (pg, data, MANID, HardwareVersion) == DS_TRUE)
			return DS_TRUE;
	}

	return DS_FALSE;
}

// #endif//end LLT_MODIFY-----------------------------------------------------------------------------------------------------------------------------------------------

/* sysfs group */
static ssize_t ds28e30_ds_readstatus_status_read(struct device *dev, struct device_attribute
						 *attr, char *buf)
{
	int result;
	unsigned char status[16] = { 0x00 };
	int i = 0;
	int count = 0;


	for (i = 0; i < attr_trytimes; i++) {
		result = ds28e30_get_page_status_retry(status);

		if (result == DS_TRUE) {
			count++;
			ds_log("DS28E30_cmd_readStatus success!\n");
		} else {
			ds_log("DS28E30_cmd_readStatus fail!\n");
		}
		ds_dbg
		    ("Status = %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
		     status[0], status[1], status[2], status[3], status[4],
		     status[5], status[6], status[7], status[8], status[9],
		     status[10], status[11], status[12], status[13],
		     status[14], status[15]);
		Delay_us(1000);
	}
	ds_log("test done\nsuccess time : %d\n", count);
	return scnprintf(buf, PAGE_SIZE,
			 "Success = %d\nStatus = %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
			 count, status[0], status[1], status[2], status[3],
			 status[4], status[5], status[6], status[7],
			 status[8], status[9], status[10], status[11],
			 status[12], status[13], status[14], status[15]);
}

static ssize_t ds28e30_ds_romid_status_read(struct device *dev,
					    struct device_attribute *attr,
					    char *buf)
{
	short status;
	unsigned char RomID[10] = { 0x00 };
	int i = 0;
	int count = 0;

	for (i = 0; i < attr_trytimes; i++) {
		status = ds28e30_Read_RomID_retry(RomID);

		if (status == DS_TRUE) {
			count++;
			ds_log("Read_RomID success!\n");
		} else {
			ds_log("Read_RomID fail!\n");
		}
		ds_dbg("RomID = %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
		       RomID[0], RomID[1], RomID[2], RomID[3],
		       RomID[4], RomID[5], RomID[6], RomID[7]);
		Delay_us(1000);
	}
	ds_log("test done\nsuccess time : %d\n", count);
	return scnprintf(buf, PAGE_SIZE,
			 "Success = %d\nRomID = %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
			 count, RomID[0], RomID[1], RomID[2], RomID[3],
			 RomID[4], RomID[5], RomID[6], RomID[7]);
}

static ssize_t ds28e30_ds_pagenumber_status_read(struct device *dev, struct device_attribute
						 *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%02x\n", pagenumber);
}

static ssize_t ds28e30_ds_pagenumber_store(struct device *dev,
					   struct device_attribute *attr,
					   const char *buf, size_t count)
{
	int buf_int;

	if (sscanf(buf, "%d", &buf_int) != 1)
		return -EINVAL;

	ds_dbg("new pagenumber = %d\n", buf_int);

	if ((buf_int >= 0) && (buf_int <= 3))
		pagenumber = buf_int;

	return count;
}

static ssize_t ds28e30_ds_pagedata_status_read(struct device *dev, struct device_attribute
					       *attr, char *buf)
{
	int result;
	unsigned char pagedata[32] = { 0x00 };
	int i = 0;
	int count = 0;

	for (i = 0; i < attr_trytimes; i++) {
		result = ds28e30_get_page_data_retry(pagenumber, pagedata);

		if (result == DS_TRUE) {
			count++;
			ds_log("DS28E30_cmd_readMemory success!\n");
		} else {
			ds_log("DS28E30_cmd_readMemory fail!\n");
		}
		ds_dbg
		    ("pagedata = %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
		     pagedata[0], pagedata[1], pagedata[2], pagedata[3],
		     pagedata[4], pagedata[5], pagedata[6], pagedata[7],
		     pagedata[8], pagedata[9], pagedata[10], pagedata[11],
		     pagedata[12], pagedata[13], pagedata[14],
		     pagedata[15]);
		Delay_us(1000);
	}
	ds_log("test done\nsuccess time : %d\n", count);
	return scnprintf(buf, PAGE_SIZE,
			 "Success = %d\npagedata = %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
			 count, pagedata[0], pagedata[1], pagedata[2],
			 pagedata[3], pagedata[4], pagedata[5],
			 pagedata[6], pagedata[7], pagedata[8],
			 pagedata[9], pagedata[10], pagedata[11],
			 pagedata[12], pagedata[13], pagedata[14],
			 pagedata[15]);
}

static ssize_t ds28e30_ds_pagedata_store(struct device *dev,
					 struct device_attribute *attr,
					 const char *buf, size_t count)
{
	int result;
	unsigned char pagedata[16] = { 0x00 };

	if (sscanf
	    (buf,
	     "%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx",
	     &pagedata[0], &pagedata[1], &pagedata[2], &pagedata[3],
	     &pagedata[4], &pagedata[5], &pagedata[6], &pagedata[7],
	     &pagedata[8], &pagedata[9], &pagedata[10], &pagedata[11],
	     &pagedata[12], &pagedata[13], &pagedata[14],
	     &pagedata[15]) != 16)
		return -EINVAL;

	ds_dbg
	    ("new data = %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
	     pagedata[0], pagedata[1], pagedata[2], pagedata[3],
	     pagedata[4], pagedata[5], pagedata[6], pagedata[7],
	     pagedata[8], pagedata[9], pagedata[10], pagedata[11],
	     pagedata[12], pagedata[13], pagedata[14], pagedata[15]);

	result = ds28e30_cmd_writeMemory(pagenumber, pagedata);
	if (result == DS_TRUE)
		ds_log("DS28E30_cmd_writeMemory success!\n");
	else
		ds_log("DS28E30_cmd_writeMemory fail!\n");

	return count;
}

static ssize_t ds28e30_ds_time_status_read(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%d\n", attr_trytimes);
}

static ssize_t ds28e30_ds_time_store(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t count)
{
	int buf_int;

	if (sscanf(buf, "%d", &buf_int) != 1)
		return -EINVAL;

	ds_log("new trytimes = %d\n", buf_int);

	if (buf_int > 0)
		attr_trytimes = buf_int;

	return count;
}

static ssize_t ds28e30_ds_session_seed_status_read(struct device *dev, struct device_attribute
						   *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE,
			 "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\n",
			 session_seed[0], session_seed[1], session_seed[2],
			 session_seed[3], session_seed[4], session_seed[5],
			 session_seed[6], session_seed[7], session_seed[8],
			 session_seed[9], session_seed[10],
			 session_seed[11], session_seed[12],
			 session_seed[13], session_seed[14],
			 session_seed[15], session_seed[16],
			 session_seed[17], session_seed[18],
			 session_seed[19], session_seed[20],
			 session_seed[21], session_seed[22],
			 session_seed[23], session_seed[24],
			 session_seed[25], session_seed[26],
			 session_seed[27], session_seed[28],
			 session_seed[29], session_seed[30],
			 session_seed[31]);
}

static ssize_t ds28e30_ds_session_seed_store(struct device *dev,
					     struct device_attribute *attr,
					     const char *buf, size_t count)
{
	if (sscanf
	    (buf,
	     "%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx",
	     &session_seed[0], &session_seed[1], &session_seed[2],
	     &session_seed[3], &session_seed[4], &session_seed[5],
	     &session_seed[6], &session_seed[7], &session_seed[8],
	     &session_seed[9], &session_seed[10], &session_seed[11],
	     &session_seed[12], &session_seed[13], &session_seed[14],
	     &session_seed[15], &session_seed[16], &session_seed[17],
	     &session_seed[18], &session_seed[19], &session_seed[20],
	     &session_seed[21], &session_seed[22], &session_seed[23],
	     &session_seed[24], &session_seed[25], &session_seed[26],
	     &session_seed[27], &session_seed[28], &session_seed[29],
	     &session_seed[30], &session_seed[31]) != 32)
		return -EINVAL;

	return count;
}

static ssize_t ds28e30_ds_challenge_status_read(struct device *dev, struct device_attribute
						*attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE,
			 "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\n",
			 challenge[0], challenge[1], challenge[2],
			 challenge[3], challenge[4], challenge[5],
			 challenge[6], challenge[7], challenge[8],
			 challenge[9], challenge[10], challenge[11],
			 challenge[12], challenge[13], challenge[14],
			 challenge[15], challenge[16], challenge[17],
			 challenge[18], challenge[19], challenge[20],
			 challenge[21], challenge[22], challenge[23],
			 challenge[24], challenge[25], challenge[26],
			 challenge[27], challenge[28], challenge[29],
			 challenge[30], challenge[31]);
}

static ssize_t ds28e30_ds_challenge_store(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf, size_t count)
{
	if (sscanf
	    (buf,
	     "%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx",
	     &challenge[0], &challenge[1], &challenge[2], &challenge[3],
	     &challenge[4], &challenge[5], &challenge[6], &challenge[7],
	     &challenge[8], &challenge[9], &challenge[10], &challenge[11],
	     &challenge[12], &challenge[13], &challenge[14],
	     &challenge[15], &challenge[16], &challenge[17],
	     &challenge[18], &challenge[19], &challenge[20],
	     &challenge[21], &challenge[22], &challenge[23],
	     &challenge[24], &challenge[25], &challenge[26],
	     &challenge[27], &challenge[28], &challenge[29],
	     &challenge[30], &challenge[31]) != 32)
		return -EINVAL;

	return count;
}

static ssize_t ds28e30_ds_S_secret_status_read(struct device *dev, struct device_attribute
					       *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE,
			 "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\n",
			 S_secret[0], S_secret[1], S_secret[2],
			 S_secret[3], S_secret[4], S_secret[5],
			 S_secret[6], S_secret[7], S_secret[8],
			 S_secret[9], S_secret[10], S_secret[11],
			 S_secret[12], S_secret[13], S_secret[14],
			 S_secret[15], S_secret[16], S_secret[17],
			 S_secret[18], S_secret[19], S_secret[20],
			 S_secret[21], S_secret[22], S_secret[23],
			 S_secret[24], S_secret[25], S_secret[26],
			 S_secret[27], S_secret[28], S_secret[29],
			 S_secret[30], S_secret[31]);
}

static ssize_t ds28e30_ds_S_secret_store(struct device *dev,
					 struct device_attribute *attr,
					 const char *buf, size_t count)
{
	if (sscanf
	    (buf,
	     "%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx,%2hhx",
	     &S_secret[0], &S_secret[1], &S_secret[2], &S_secret[3],
	     &S_secret[4], &S_secret[5], &S_secret[6], &S_secret[7],
	     &S_secret[8], &S_secret[9], &S_secret[10], &S_secret[11],
	     &S_secret[12], &S_secret[13], &S_secret[14], &S_secret[15],
	     &S_secret[16], &S_secret[17], &S_secret[18], &S_secret[19],
	     &S_secret[20], &S_secret[21], &S_secret[22], &S_secret[23],
	     &S_secret[24], &S_secret[25], &S_secret[26], &S_secret[27],
	     &S_secret[28], &S_secret[29], &S_secret[30],
	     &S_secret[31]) != 32)
		return -EINVAL;

	return count;
}

static ssize_t ds28e30_ds_auth_ANON_status_read(struct device *dev, struct device_attribute
						*attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%02x\n", auth_ANON);
}

static ssize_t ds28e30_ds_auth_ANON_store(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf, size_t count)
{
	if (sscanf(buf, "%d", &auth_ANON) != 1)
		return -EINVAL;

	return count;
}

static ssize_t ds28e30_ds_auth_BDCONST_status_read(struct device *dev, struct device_attribute
						   *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%02x\n", auth_BDCONST);
}

static ssize_t ds28e30_ds_auth_BDCONST_store(struct device *dev,
					     struct device_attribute *attr,
					     const char *buf, size_t count)
{
	if (sscanf(buf, "%d", &auth_BDCONST) != 1)
		return -EINVAL;

	return count;
}

static ssize_t ds28e30_ds_Auth_Result_status_read(struct device *dev, struct device_attribute
						  *attr, char *buf)
{
	int result;

	result = AuthenticateDS28E30();
	if (result == ERROR_R_STATUS)
		return scnprintf(buf, PAGE_SIZE,
				 "Authenticate failed : ERROR_R_STATUS!\n");
	else if (result == ERROR_UNMATCH_MAC)
		return scnprintf(buf, PAGE_SIZE,
				 "Authenticate failed : MAC is not match!\n");
	else if (result == ERROR_R_ROMID)
		return scnprintf(buf, PAGE_SIZE,
				 "Authenticate failed : ERROR_R_ROMID!\n");
	else if (result == ERROR_COMPUTE_MAC)
		return scnprintf(buf, PAGE_SIZE,
				 "Authenticate failed : ERROR_COMPUTE_MAC!\n");
	else if (result == ERROR_S_SECRET)
		return scnprintf(buf, PAGE_SIZE,
				 "Authenticate failed : ERROR_S_SECRET!\n");
	else if (result == DS_TRUE)
		return scnprintf(buf, PAGE_SIZE,
				 "Authenticate success!!!\n");
	else
		return scnprintf(buf, PAGE_SIZE,
				 "Authenticate failed : other reason.\n");
}

static ssize_t ds28e30_ds_page0_data_read(struct device *dev,
					  struct device_attribute *attr,
					  char *buf)
{
	int ret;
	unsigned char page0_data[50];

	ret = ds28e30_get_page_data_retry(0, page0_data);
	return scnprintf(buf, PAGE_SIZE, "%s\n", page0_data);
	if (ret != DS_TRUE)
		return -EAGAIN;
}

static ssize_t ds28e30_ds_page1_data_read(struct device *dev,
					  struct device_attribute *attr,
					  char *buf)
{
	int ret;
	unsigned char page1_data[50];

	ret = ds28e30_get_page_data_retry(1, page1_data);
	return scnprintf(buf, PAGE_SIZE, "%s\n", page1_data);
	if (ret != DS_TRUE)
		return -EAGAIN;
}

static ssize_t ds28e30_ds_verify_model_name_read(struct device *dev, struct device_attribute
						 *attr, char *buf)
{
	int ret;
	ret = ds28e30_Read_RomID_retry(mi_romid);
	if (ret == DS_TRUE)
		return scnprintf(buf, PAGE_SIZE, "ds28e16");
	else
		return scnprintf(buf, PAGE_SIZE, "unknown");
}

static ssize_t ds28e30_ds_chip_ok_read(struct device *dev,
				       struct device_attribute *attr,
				       char *buf)
{
	int ret;
	int chip_ok_status;
	ret = ds28e30_Read_RomID_retry(mi_romid);
	if ((((mi_romid[0] & 0x7F) == FAMILY_CODE) && (mi_romid[6] == Samsung_CID_MSB)
	     && ((mi_romid[5] & 0xf0) == Samsung_CID_LSB)))
		chip_ok_status = true;
	else
		chip_ok_status = false;
	return scnprintf(buf, PAGE_SIZE, "%d\n", chip_ok_status);
}

static ssize_t ds28e30_ds_cycle_count_read(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	int ret;
	unsigned int cycle_count;
	unsigned char pagedata[32] = { 0x00 };

	ret = ds28e30_get_page_data_retry(DC_PAGE, pagedata);
	if (ret == DS_TRUE) {
		cycle_count = (pagedata[2] << 16) + (pagedata[1] << 8)
		    + pagedata[0];
		cycle_count = DC_INIT_VALUE - cycle_count;
	} else {
		cycle_count = 0;
	}
	return scnprintf(buf, PAGE_SIZE, "%d\n", cycle_count);
}

int DS28E30_cmd_decrementCounter(void)
{
	unsigned char write_buf[255];
	unsigned char read_buf[255];
	int write_len = 0;
	int read_len = 1;

	last_result_byte = RESULT_FAIL_NONE;
	/*
	   ?<Start, device address write>
	   ?TX: Decrement Counter Command
	   ?<Stop>
	   ?<Delay>
	   ?<Start, device address read>
	   ?RX: Length (SMBus) [always 1]
	   ?RX: Result byte
	   ?<Stop>
	 */

	//write_buf[write_len++] = 1;
	write_buf[write_len++] = CMD_DECREMENT_CNT;

	if (standard_cmd_flow
	    (write_buf, write_len, DELAY_DS28E30_EE_WRITE_TWM + 50,
	     read_len, read_buf, &read_len)) {
		if (read_len == 1) {
			last_result_byte = read_buf[0];
			if (read_buf[0] == RESULT_SUCCESS)
				return DS_TRUE;
		}
	}

	ow_reset();
	return DS_FALSE;
}

static ssize_t ds28e30_ds_cycle_count_write(struct device *dev,
					    struct device_attribute *attr,
					    const char *buf, size_t count)
{
	int ret = 0;
  	unsigned char flag = DS_TRUE;
	unsigned char buf1[128];
	unsigned char DecrementCounter[3];
  	unsigned int retry = 5;
	int j = 0;
  

  	while(--retry > 0) {
          	pr_err("%s:retry :%d\n",__func__,retry);
        	if (DS28E30_cmd_decrementCounter())
                	break;
          	msleep(100);
    }

	//reading Counter/page 106 data if counter=expected counter value，page106是递减计数器，0x1FFFF for Xiaomi
	//TestingItemResult[CounterValue_Result] = DS_FALSE;
	for (j = 0; j < READ_DEC_COUNTER_RETRY; j++) {
		flag = ds28e30_cmd_readMemory(PG_DEC_COUNTER, buf1);
		if (flag == DS_TRUE) {
			memcpy(DecrementCounter, buf, 3);
			//TestingItemResult[CounterValue_Result] = DS_TRUE;
			ds_err("DecrementCounter:\n");
			ds_err("%02x %02x %02x\n",
				DecrementCounter[0], DecrementCounter[1],
				DecrementCounter[2]);
			break;
		} else {
			ds_err("readMemory page106 failed\n");
			continue;
		}
                msleep(50);
	}

	return ret;
}

static DEVICE_ATTR(ds_readstatus, S_IRUGO,
		   ds28e30_ds_readstatus_status_read, NULL);
static DEVICE_ATTR(ds_romid, S_IRUGO, ds28e30_ds_romid_status_read, NULL);
static DEVICE_ATTR(ds_pagenumber, S_IRUGO | S_IWUSR | S_IWGRP,
		   ds28e30_ds_pagenumber_status_read,
		   ds28e30_ds_pagenumber_store);
static DEVICE_ATTR(ds_pagedata, S_IRUGO | S_IWUSR | S_IWGRP,
		   ds28e30_ds_pagedata_status_read,
		   ds28e30_ds_pagedata_store);
static DEVICE_ATTR(ds_time, S_IRUGO | S_IWUSR | S_IWGRP,
		   ds28e30_ds_time_status_read, ds28e30_ds_time_store);
static DEVICE_ATTR(ds_session_seed, S_IRUGO | S_IWUSR | S_IWGRP,
		   ds28e30_ds_session_seed_status_read,
		   ds28e30_ds_session_seed_store);
static DEVICE_ATTR(ds_challenge, S_IRUGO | S_IWUSR | S_IWGRP,
		   ds28e30_ds_challenge_status_read,
		   ds28e30_ds_challenge_store);
static DEVICE_ATTR(ds_S_secret, S_IRUGO | S_IWUSR | S_IWGRP,
		   ds28e30_ds_S_secret_status_read,
		   ds28e30_ds_S_secret_store);
static DEVICE_ATTR(ds_auth_ANON, S_IRUGO | S_IWUSR | S_IWGRP,
		   ds28e30_ds_auth_ANON_status_read,
		   ds28e30_ds_auth_ANON_store);
static DEVICE_ATTR(ds_auth_BDCONST, S_IRUGO | S_IWUSR | S_IWGRP,
		   ds28e30_ds_auth_BDCONST_status_read,
		   ds28e30_ds_auth_BDCONST_store);
static DEVICE_ATTR(ds_Auth_Result, S_IRUGO,
		   ds28e30_ds_Auth_Result_status_read, NULL);
static DEVICE_ATTR(ds_page0_data, S_IRUGO,
		   ds28e30_ds_page0_data_read, NULL);
static DEVICE_ATTR(ds_page1_data, S_IRUGO,
		   ds28e30_ds_page1_data_read, NULL);
static DEVICE_ATTR(ds_verify_model_name, S_IRUGO,
		   ds28e30_ds_verify_model_name_read, NULL);
static DEVICE_ATTR(ds_chip_ok, S_IRUGO, ds28e30_ds_chip_ok_read, NULL);
static DEVICE_ATTR(ds_cycle_count, S_IRUGO | S_IWUSR | S_IWGRP,
		   ds28e30_ds_cycle_count_read,
		   ds28e30_ds_cycle_count_write);

static struct attribute *ds_attributes[] = {
	&dev_attr_ds_readstatus.attr,
	&dev_attr_ds_romid.attr,
	&dev_attr_ds_pagenumber.attr,
	&dev_attr_ds_pagedata.attr,
	&dev_attr_ds_time.attr,
	&dev_attr_ds_session_seed.attr,
	&dev_attr_ds_challenge.attr,
	&dev_attr_ds_S_secret.attr,
	&dev_attr_ds_auth_ANON.attr,
	&dev_attr_ds_auth_BDCONST.attr,
	&dev_attr_ds_Auth_Result.attr,
	&dev_attr_ds_page0_data.attr,
	&dev_attr_ds_page1_data.attr,
	&dev_attr_ds_verify_model_name.attr,
	&dev_attr_ds_chip_ok.attr,
	&dev_attr_ds_cycle_count.attr,
	NULL,
};

static const struct attribute_group ds_attr_group = {
	.attrs = ds_attributes,
};

static int ds28e30_start_auth_battery(struct auth_device *auth_dev)
{
	if (AuthenticateDS28E30() == DS_TRUE)
		return 0;

	return -1;
}
uint8_t auth_get_batt_id(void)
{
	pr_err("batt_id:%d\n", batt_id);
	return batt_id;
}
EXPORT_SYMBOL(auth_get_batt_id);

static int ds28e30_get_batt_id(struct auth_device *auth_dev, u8 * id)
{
	*id = batt_id;
	pr_err("batt_id:%d\n", batt_id);

	return 0;
}

static int ds28e30_get_batt_sn(struct auth_device *auth_dev, u8 * soh_sn)
{
	// pr_err("%s batt_sn:%s\n", __func__, mi_page0_data);
	soh_sn = mi_page0_data;
	//memcpy(soh_sn, mi_page0_data, len);
	pr_err("soh_sn:%s\n", soh_sn);

	return 0;
}

static int ds28e30_get_ui_soh(struct auth_device *auth_dev, u8 *ui_soh_data, int len)
{
	int flag = 0, i = 0, invalid_count = 0;
	unsigned char buf[128];

	flag = ds28e30_cmd_readMemory(PG_USER_EEPROM_2, buf);
	if (flag == DS_TRUE) {
		memcpy(ui_soh_data, buf, len);
	}

	for (i = 0; i < len; i++) {
		if (ui_soh_data[i] == 0xff)
			invalid_count++;
	}

	if (invalid_count >= 5) {
		pr_err("%s invalid value, set 0\n", __func__);
		memset(ui_soh_data, 0, len);
	}

	pr_err("%s ui_soh:\n", __func__);
	for (i = 0; i < len; i++) {
		pr_err("%s data[%d]:0x%x\n", __func__, i, ui_soh_data[i]);
	}

	return 0;
}

static int ds28e30_set_ui_soh(struct auth_device *auth_dev, u8 *ui_soh_data, int len, int raw_soh)
{
	int flag = 0, i = 0;
	unsigned char buf[128];

	flag = ds28e30_cmd_readMemory(PG_USER_EEPROM_2, buf);
	if (flag != DS_TRUE) {
		pr_err("%s read fail\n", __func__);
		return 0;
	}

	for (i = 0; i < len; i++) {
		buf[i] = ui_soh_data[i];
	}
	buf[15] = raw_soh;

	flag = ds28e30_cmd_writeMemory(PG_USER_EEPROM_2, buf);
	if (flag == DS_TRUE)
		pr_err("%s DS28E30_cmd_writeMemory success!\n", __func__);

	pr_err("%s ui_soh:\n", __func__);
	for (i = 0; i < len; i++) {
		pr_err("%s buf[%d]:0x%x\n", __func__, i, buf[i]);
	}

	return 0;
}

#define MAX_COUNT    20
static int ds28e30_get_cycle_count(struct auth_device *auth_dev)
{
	int flag = 0, i = 0, dc_count = 0;
	unsigned char pagedata[32];
	int fail_count = 0;

	for (i = 0; i < MAX_COUNT; i++) {
		msleep(30);
		flag = ds28e30_cmd_readMemory(PG_DEC_COUNTER, pagedata);
		if (flag == DS_TRUE) {
			dc_count = (pagedata[2] << 16) + (pagedata[1] << 8) + pagedata[0];
			dc_count = DC_INIT_VALUE - dc_count;
			pr_err("%s dc_count:%d\n", __func__, dc_count);
			ds_err("DecrementCounter:\n");
			ds_err("%02x %02x %02x\n",
				pagedata[0], pagedata[1], pagedata[2]);

			fail_count = 0;
			break;
		} else {
			fail_count++;
			pr_err("%s ds28e30_get_page_data_retry fail, flag:%d, fail_count:%d\n", __func__, flag, fail_count);
			if (fail_count == MAX_COUNT) {
				pr_err("%s read error, return\n", __func__);
				fail_count = 0;
				return -1;
			}
		}
	}

	pr_err("%s cycle_count:%d\n", __func__, dc_count);
	return dc_count;
}

static int ds28e30_set_cycle_count(struct auth_device *auth_dev, int count, int get_count)
{
	int flag = 0, i = 0, dc_count = 0;
	unsigned char pagedata[32];
	int fail_count = 0;

	for (i = 0; i < MAX_COUNT; i++) {
		msleep(30);
		flag = ds28e30_cmd_readMemory(PG_DEC_COUNTER, pagedata);
		if (flag == DS_TRUE) {
			dc_count = (pagedata[2] << 16) + (pagedata[1] << 8) + pagedata[0];
			dc_count = DC_INIT_VALUE - dc_count;
			pr_err("%s dc_count:0x%2x\n", __func__, dc_count);
			ds_err("DecrementCounter:\n");
			ds_err("%02x %02x %02x\n",
				pagedata[0], pagedata[1], pagedata[2]);
			fail_count = 0;
			break;
		} else {
			fail_count++;
			pr_err("%s ds28e30_get_page_data_retry fail, flag:%d, fail_count:%d\n", __func__, flag, fail_count);
			if (fail_count == MAX_COUNT) {
				pr_err("%s ds28e30_get_page_data_retry error, return\n", __func__);
				fail_count = 0;
				return -1;
			}
		}
	}

	fail_count = 0;
	dc_count = count - dc_count;
	if (dc_count > 0) {
		while(dc_count--){
			msleep(30);
			flag = DS28E30_cmd_decrementCounter();
			if (flag != DS_TRUE) {
				pr_err("%s DS28E30_cmd_decrementCounter fail!\n", __func__);
				dc_count++;
				fail_count++;
				if (fail_count > MAX_COUNT*2) {
					pr_err("%s DS28E30_cmd_decrementCounter error, return\n", __func__);
					fail_count = 0;
					return -1;
				}
			}
		}
	}

	return 0;
}

static int ds28e30_get_first_use_date(struct auth_device *auth_dev)
{
	unsigned char buf[32];
	int first_use_date = 0xFF;
	int ret;

	ret = ds28e30_get_page_data_retry(PG_USER_EEPROM_1, buf);
	if (ret == DS_TRUE) {
		if ((buf[20] == 0xff) && (buf[21] == 0xff) && (buf[22] == 0xff) && (buf[23] == 0xff)
			 && (buf[24] == 0xff) && (buf[25] == 0xff) && (buf[26] == 0xff) && (buf[27] == 0xff))
			first_use_date = 0xFF;
		else
			first_use_date = ascii2hex(buf[20]) * 10000000 + ascii2hex(buf[21]) * 1000000
				+ ascii2hex(buf[22]) * 100000 + ascii2hex(buf[23]) * 10000 + ascii2hex(buf[24]) * 1000
				+ ascii2hex(buf[25]) * 100 + ascii2hex(buf[26]) * 10 + ascii2hex(buf[27]);
	} else {
		pr_err("read fail\n");
		return first_use_date;
	}
	ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x,%d",
		buf[20], buf[21], buf[22], buf[23],
		buf[24], buf[25], buf[26], buf[27], first_use_date);

	pr_err("first_use_date:%d,ret=%d\n", first_use_date, ret);

	return first_use_date;
}

static int ds28e30_set_first_use_date(struct auth_device *auth_dev, int first_use_date)
{
	int flag = 0;
	unsigned char buf[32];

	flag = ds28e30_cmd_readMemory(PG_USER_EEPROM_1, buf);
	ds_dbg("read: %02x %02x %02x %02x %02x %02x %02x %02x,%d",
		buf[20], buf[21], buf[22], buf[23],
		buf[24], buf[25], buf[26], buf[27], first_use_date);
	if (flag != DS_TRUE) {
		pr_err("read fail\n");
		return -1;
	}
	if (first_use_date == 0xff) {
		buf[27] = 0xff;
		buf[26] = 0xff;
		buf[25] = 0xff;
		buf[24] = 0xff;
		buf[23] = 0xff;
		buf[22] = 0xff;
		buf[21] = 0xff;
		buf[20] = 0xff;
	} else {
		buf[27] = hex2ascii(first_use_date % 10);
		buf[26] = hex2ascii((first_use_date % 100) / 10);
		buf[25] = hex2ascii((first_use_date % 1000) / 100);
		buf[24] = hex2ascii((first_use_date % 10000) / 1000);
		buf[23] = hex2ascii((first_use_date % 100000) / 10000);
		buf[22] = hex2ascii((first_use_date % 1000000) / 100000);
		buf[21] = hex2ascii((first_use_date % 10000000) / 1000000);
		buf[20] = hex2ascii((first_use_date % 100000000) / 10000000);
	}
	ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
		buf[20], buf[21], buf[22], buf[23],
		buf[24], buf[25], buf[26], buf[27]);

	flag = ds28e30_cmd_writeMemory(PG_USER_EEPROM_1, buf);
	if (flag == DS_TRUE) {
		pr_err("ds28e30_cmd_writeMemory %d success!\n", first_use_date);
	} else {
		pr_err("ds28e30_cmd_writeMemory %d fail\n", first_use_date);
		return -1;
	}

	return 0;
}

static int ds28e30_set_batt_discharge_level(struct auth_device *auth_dev, int bat_discharge_level)
{
	int flag = 0;
	unsigned char buf[32];

	flag = ds28e30_cmd_readMemory(PG_USER_EEPROM_1, buf);
	ds_dbg("read: %02x %02x %02x %02x",	buf[16], buf[17], buf[18], buf[19]);
	if (flag != DS_TRUE) {
		pr_err("read fail\n");
		return -1;
	}
	if(bat_discharge_level == 0) {
		buf[19] = 0xff;
		buf[18] = 0xff;
		buf[17] = 0xff;
		buf[16] = 0xff;
	} else {
		buf[19] = hex2ascii(bat_discharge_level % 10);
		buf[18] = hex2ascii((bat_discharge_level % 100) / 10);
		buf[17] = hex2ascii((bat_discharge_level % 1000) / 100);
		buf[16] = hex2ascii((bat_discharge_level % 10000) / 1000);
	}
	ds_dbg("%02x %02x %02x %02x:%d", buf[16], buf[17], buf[18], buf[19], bat_discharge_level);

	flag = ds28e30_cmd_writeMemory(PG_USER_EEPROM_1, buf);
	if (flag == DS_TRUE) {
		pr_err("ds28e30_cmd_writeMemory %d success!\n", bat_discharge_level);
	} else {
		pr_err("ds28e30_cmd_writeMemory %d fail\n", bat_discharge_level);
		return -1;
	}

	return 0;
}

static int ds28e30_get_batt_discharge_level(struct auth_device *auth_dev)
{
	unsigned char buf[32];
	int ret;
	int batt_discharge_level = 0;

	ret = ds28e30_get_page_data_retry(PG_USER_EEPROM_1, buf);
	if (ret == DS_TRUE) {
		if ((buf[16] == 0xff) && (buf[17] == 0xff) && (buf[18] == 0xff) && (buf[19] == 0xff)) {
			batt_discharge_level = 0;
		} else {
			batt_discharge_level = ascii2hex(buf[16]) * 1000 + ascii2hex(buf[17]) * 100
				+ ascii2hex(buf[18]) * 10 + ascii2hex(buf[19]);
		}
	} else {
		pr_err("read fail\n");
		return batt_discharge_level;
	}
	ds_dbg("%02x %02x %02x %02x,%d", buf[16], buf[17], buf[18], buf[19], batt_discharge_level);

	return batt_discharge_level;
}

static int ds28e30_set_batt_full_status_usage(struct auth_device *auth_dev, int bat_full_status_usage)
{
	batt_full_status_usage = bat_full_status_usage;
	pr_err("%d success!\n", batt_full_status_usage);

	return 0;
}

static int ds28e30_get_batt_full_status_usage(struct auth_device *auth_dev)
{
	pr_err("%d success!\n", batt_full_status_usage);
	return batt_full_status_usage;
}

static int ds28e30_set_sync_buf_mem_sts(struct auth_device *auth_dev, int syn_buf_mem_sts)
{
	sync_buf_mem_sts = syn_buf_mem_sts;
	pr_err("%d success!\n", sync_buf_mem_sts);
	return 0;
}

static int ds28e30_get_sync_buf_mem_sts(struct auth_device *auth_dev)
{
	pr_err("%d success!\n", sync_buf_mem_sts);
	return sync_buf_mem_sts;
}

static int ds28e30_set_sync_buf_mem(struct auth_device *auth_dev, int syn_buf_mem)
{
	sync_buf_mem = syn_buf_mem;
	pr_err("%d success!\n", sync_buf_mem);
	return 0;
}

static int ds28e30_get_sync_buf_mem(struct auth_device *auth_dev)
{
	pr_err("%d success!\n", sync_buf_mem);
	return sync_buf_mem;
}

static int ds28e30_set_fai_expired(struct auth_device *auth_dev, int fa_expired)
{
	fai_expired = fa_expired;
	pr_err("%d success!\n", fai_expired);
	return 0;
}

static int ds28e30_get_fai_expired(struct auth_device *auth_dev)
{
	pr_err("%d success!\n", fai_expired);
	return fai_expired;
}

static int ds28e30_set_asoc(struct auth_device *auth_dev, int bat_asoc)
{
	batt_asoc = bat_asoc;
	pr_err("%d success!\n", batt_asoc);
	return 0;
}

static int ds28e30_get_asoc(struct auth_device *auth_dev)
{
	pr_err("%d success!\n", batt_asoc);
	return batt_asoc;
}

static int ds28e30_set_bsoh(struct auth_device *auth_dev, int bsoh)
{
	int flag = 0;
	unsigned char buf[32];
	int bsohint;

	flag = ds28e30_cmd_readMemory(PG_USER_EEPROM_1, buf);
	ds_dbg("read: %02x %02x %02x %02x %02x %02x %02x %02x",
		buf[0], buf[1], buf[2], buf[3],	buf[4], buf[5], buf[6], buf[7]);
	if (flag != DS_TRUE) {
		pr_err("read fail\n");
		return -1;
	}
	bsohint = bsoh * 10;
	if ((bsohint == 100000) || (bsohint == 0)) {
		buf[7] = 0xff;
		buf[6] = 0xff;
		buf[5] = 0xff;
		buf[4] = 0xff;
		buf[3] = 0xff;
		buf[2] = 0xff;
		buf[1] = 0xff;
		buf[0] = 0xff;
	} else {
		buf[7] = hex2ascii(bsohint % 10);
		buf[6] = hex2ascii((bsohint % 100) / 10);
		buf[5] = hex2ascii((bsohint % 1000) / 100);
		buf[4] = 0x2E;
		buf[3] = hex2ascii((bsohint % 10000) / 1000);
		buf[2] = hex2ascii((bsohint % 100000) / 10000);
		buf[1] = hex2ascii((bsohint % 1000000) / 100000);
		buf[0] = hex2ascii((bsohint % 10000000) / 1000000);
	}
	ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
		buf[0], buf[1], buf[2], buf[3],	buf[4], buf[5], buf[6], buf[7]);

	flag = ds28e30_cmd_writeMemory(PG_USER_EEPROM_1, buf);
	if (flag == DS_TRUE) {
		pr_err("ds28e30_cmd_writeMemory %d success!\n", bsohint);
	} else {
		pr_err("ds28e30_cmd_writeMemory %d fail\n", bsohint);
		return -1;
	}

	return 0;
}

static int ds28e30_get_bsoh(struct auth_device *auth_dev)
{
	unsigned char buf[32];
	int bsoh = 10000;
	int ret;

	ret = ds28e30_get_page_data_retry(PG_USER_EEPROM_1, buf);
	if (ret == DS_TRUE) {
		if ((buf[0] == 0xff) && (buf[1] == 0xff) && (buf[2] == 0xff) && (buf[3] == 0xff)
			 && (buf[4] == 0xff) && (buf[5] == 0xff) && (buf[6] == 0xff) && (buf[7] == 0xff)) {
			bsoh = 10000;
		} else {
			bsoh = (ascii2hex(buf[0]) * 1000000 + ascii2hex(buf[1]) * 100000
				+ ascii2hex(buf[2]) * 10000 + ascii2hex(buf[3])  * 1000
				+ ascii2hex(buf[5]) * 100 + ascii2hex(buf[6]) * 10
				+ ascii2hex(buf[7]))/10;
		}
	} else {
		pr_err("read fail\n");
		return bsoh;
	}
	ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x:%d,%d",
		buf[0], buf[1], buf[2], buf[3],
		buf[4], buf[5], buf[6], buf[7], bsoh, ret);

	return bsoh;
}

static int ds28e30_set_bsoh_raw(struct auth_device *auth_dev, int bso_raw)
{
	int flag = 0;
	unsigned char buf[32];
	int bsohrawint;

	flag = ds28e30_cmd_readMemory(PG_USER_EEPROM_1, buf);
	ds_dbg("read: %02x %02x %02x %02x %02x %02x %02x %02x",
		buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
	if (flag != DS_TRUE) {
		pr_err("read fail\n");
		return -1;
	}
	bsohrawint = bso_raw * 10;
	if ((bsohrawint == 100000) || (bsohrawint == 0)) {
		buf[15] = 0xff;
		buf[14] = 0xff;
		buf[13] = 0xff;
		buf[12] = 0xff;
		buf[11] = 0xff;
		buf[10] = 0xff;
		buf[9] = 0xff;
		buf[8] = 0xff;
	} else {
		buf[15] = hex2ascii(bsohrawint % 10);
		buf[14] = hex2ascii((bsohrawint % 100) / 10);
		buf[13] = hex2ascii((bsohrawint % 1000) / 100);
		buf[12] = 0x2E;
		buf[11] = hex2ascii((bsohrawint % 10000) / 1000);
		buf[10] = hex2ascii((bsohrawint % 100000) / 10000);
		buf[9] = hex2ascii((bsohrawint % 1000000) / 100000);
		buf[8] = hex2ascii((bsohrawint % 10000000) / 1000000);
	}
	ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x",
		buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);

	flag = ds28e30_cmd_writeMemory(PG_USER_EEPROM_1, buf);
	if (flag == DS_TRUE) {
		pr_err("ds28e30_cmd_writeMemory %d success!\n", bsohrawint);
	} else {
		pr_err("ds28e30_cmd_writeMemory %d fail\n", bsohrawint);
		return -1;
	}

	return 0;
}

static int ds28e30_get_bsoh_raw(struct auth_device *auth_dev)
{
	unsigned char buf[32];
	int bsoh_raw = 10000;
	int ret;

	ret = ds28e30_get_page_data_retry(PG_USER_EEPROM_1, buf);
	if (ret == DS_TRUE) {
		if ((buf[8] == 0xff) && (buf[9] == 0xff) && (buf[10] == 0xff) && (buf[11] == 0xff)
			 && (buf[12] == 0xff) && (buf[13] == 0xff) && (buf[14] == 0xff) && (buf[15] == 0xff)) {
			bsoh_raw = 10000;
		} else {
			bsoh_raw = (ascii2hex(buf[8]) * 1000000 + ascii2hex(buf[9]) * 100000
				+ ascii2hex(buf[10]) * 10000 + ascii2hex(buf[11])  * 1000
				+ ascii2hex(buf[13]) * 100 + ascii2hex(buf[14]) * 10
				+ ascii2hex(buf[15]))/10;
		}
	} else {
		pr_err("read fail\n");
		return bsoh_raw;
	}
	ds_dbg("%02x %02x %02x %02x %02x %02x %02x %02x:%d,%d", buf[8], buf[9],
		buf[10], buf[11], buf[12], buf[13], buf[14], buf[15], bsoh_raw, ret);

	return bsoh_raw;
}

struct auth_ops ds28e30_auth_ops = {
	.auth_battery = ds28e30_start_auth_battery,
	.get_battery_id = ds28e30_get_batt_id,
	.get_batt_sn = ds28e30_get_batt_sn,
	.get_ui_soh = ds28e30_get_ui_soh,
	.set_ui_soh = ds28e30_set_ui_soh,
	.get_cycle_count = ds28e30_get_cycle_count,
	.set_cycle_count = ds28e30_set_cycle_count,
	.get_first_use_date = ds28e30_get_first_use_date,
	.set_first_use_date = ds28e30_set_first_use_date,
	.get_batt_discharge_level = ds28e30_get_batt_discharge_level,
	.set_batt_discharge_level = ds28e30_set_batt_discharge_level,
	.get_batt_bsoh = ds28e30_get_bsoh,
	.set_batt_bsoh = ds28e30_set_bsoh,
	.get_batt_bsoh_raw = ds28e30_get_bsoh_raw,
	.set_batt_bsoh_raw = ds28e30_set_bsoh_raw,
	.get_batt_full_status_usage = ds28e30_get_batt_full_status_usage,
	.set_batt_full_status_usage = ds28e30_set_batt_full_status_usage,
	.get_batt_asoc = ds28e30_get_asoc,
	.set_batt_asoc = ds28e30_set_asoc,
	.get_fai_expired = ds28e30_get_fai_expired,
	.set_fai_expired = ds28e30_set_fai_expired,
	.get_sync_buf_mem_sts = ds28e30_get_sync_buf_mem_sts,
	.set_sync_buf_mem_sts = ds28e30_set_sync_buf_mem_sts,
	.get_sync_buf_mem = ds28e30_get_sync_buf_mem,
	.set_sync_buf_mem = ds28e30_set_sync_buf_mem,
};

static int parase_battery_authon_gpio (struct auth_device *auth_dev, struct device_node *np)
{
	//struct device_node *np = auth_dev->dev.of_node;
	int ret = 0;

	if (!np) {
		pr_err("device tree node missing\n");
		return -EINVAL;
	}
	if (!auth_dev)
		return -ENOMEM;

	auth_dev->gpio = of_get_named_gpio(np,"authon_data_gpio", 0);
	if (auth_dev->gpio < 0) {
		pr_err("%s, could not get authon_data_gpio\n", __func__);
		return -1;
	} else {
		pr_info("%s, auth_dev->gpio: %d\n", __func__, auth_dev->gpio);
		if(gpio_is_valid(auth_dev->gpio)){
			pr_info("GPIO_102:%d is valid\n", auth_dev->gpio);
			ret = gpio_request(auth_dev->gpio, "GPIO102");
			if(ret < 0){
				pr_err("ERROR: GPIO %d request\n", auth_dev->gpio);
			}
			pr_info("end gpio: GPIO %d request:%d\n", auth_dev->gpio, ret);
	    } else {
			pr_err("GPIO_102 %d is not valid\n", auth_dev->gpio);
			return -1;
	    }
	}

	return 0;
}

static int get_ds28e30_board_id(void) {
	char *bootmode_string = NULL;
	char bootmode_start[32] = " ";
	int rc;

#ifdef CONFIG_QGKI_BUILD
	bootmode_string = strstr(saved_command_line,"board_id=");
#else
	bootmode_string="board_id=P86801AA1";
#endif /* CONFIG_QGKI_BUILD */
	if(NULL != bootmode_string){
		strncpy(bootmode_start, bootmode_string+9, 9);
		ds_err("%s\n", bootmode_start);
		rc = strncmp(bootmode_start, "P86801JA1", 9);
		if(0 == rc){
			return 1;
		}
		rc = strncmp(bootmode_start, "P86803DA1", 9);
		if(0 == rc){
			return 1;
		}
	}
	ds_err("%s fail!\n", __func__);
	return 0;
}

static int ds28e30_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct ds_data *info;
	int status = 0;

	ds_err("%s enter\n", __func__);
	if (get_ds28e30_board_id() == 0)
		return 0;

	control_reg_addr = ioremap(0x566000, 0x4);
	data_reg_addr = ioremap(0x566004, 0x4);

	info =
	    devm_kzalloc(&(pdev->dev), sizeof(struct ds_data), GFP_KERNEL);
	if (!info) {
		ds_err("%s alloc mem fail\n", __func__);
		return -ENOMEM;
	}

	if ((!pdev->dev.of_node
	     || !of_device_is_available(pdev->dev.of_node)))
		return -ENODEV;


	info->dev = &(pdev->dev);
	info->pdev = pdev;
	platform_set_drvdata(pdev, info);

	ret = sysfs_create_group(&(info->dev->kobj), &ds_attr_group);
	if (ret) {
		ds_err("%s failed to register sysfs(%d)\n", __func__, ret);
		return ret;
	}

	ret = of_property_read_string(pdev->dev.of_node, 
		"auth_name", &info->auth_name);
	if (ret < 0) {
		ds_info("%s can not find auth name(%d)\n", __func__, ret);
		info->auth_name = "main_supplier";
	}

	info->auth_dev = auth_device_register(info->auth_name, NULL, info,
					      &ds28e30_auth_ops);
	if (IS_ERR_OR_NULL(info->auth_dev)) {
		ds_err("%s failed to register auth device\n", __func__);
		return PTR_ERR(info->auth_dev);
	}
	if(parase_battery_authon_gpio(info->auth_dev, pdev->dev.of_node) == 0) {		
		info->pinctrl = devm_pinctrl_get(&info->pdev->dev);
		if (IS_ERR(info->pinctrl)) {
			pr_info("%s, No pinctrl config specified\n", __func__);
			return -EINVAL;
		}
		info->pin_suspend = pinctrl_lookup_state(info->pinctrl, "sleep");
		if (IS_ERR(info->pin_suspend)) {
			pr_info("%s, could not get pin_suspend\n", __func__);
			return -EINVAL;
		}
		info->pin_active = pinctrl_lookup_state(info->pinctrl, "active");
		if(IS_ERR(info->pin_active)) {
			pr_err("%s, could not get pin_active\n", __func__);
			return -EINVAL;
		} else {
			pinctrl_select_state(info->pinctrl, info->pin_active);
			pr_info("%s, set pin_active pinctrl_select_state\n", __func__);
		}
		gpio_direction_output(info->auth_dev->gpio, 1);
		gpio_set_value(info->auth_dev->gpio, 1);	// Output High
	} else {
		ds_err("%s failed to parase_battery_authon_gpio\n", __func__);
		return -ENOMEM;
	}

	vreg = regulator_get(&(pdev->dev), DTS_VOlT_REGULATER);
	if (IS_ERR(vreg)) {
		pr_err("%s get vreg fail\n", __func__);
		return -EPERM;
	}

	status = regulator_set_voltage(vreg, 1800000, 1800000);
	status = regulator_enable(vreg);
	status = regulator_get_voltage(vreg);
	pr_err("power on regulator_value %d!!\n", status);

	g_info = info;
#ifdef SPIN_LOCK_ENABLE
	mutex_init(&ds_cmd_lock);
#endif
#if 0
	/* ds28e30 need read status first */
	if (ds28e30_Read_RomID_retry(mi_romid) == DS_TRUE)
		return 0;
#else
	if (AuthenticateDS28E30() == DS_TRUE)
		return 0;
#endif

	ds_err("%s end!\n", __func__);
	return 0;
}

static int ds28e30_remove(struct platform_device *pdev)
{
#ifdef SPIN_LOCK_ENABLE
	mutex_destroy(&ds_cmd_lock);
#endif
	return 0;
}


static const struct of_device_id ds28e30_of_ids[] = {
	{.compatible = "maxim,ds28e30"},
	{},
};

static struct platform_driver ds28e30_driver = {
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "maxim,ds28e30",
		   .of_match_table = ds28e30_of_ids,
		   },
	.probe = ds28e30_probe,
	.remove = ds28e30_remove,
};

static int __init ds28e30_init(void)
{
	ds_err("%s enter\n", __func__);
	return platform_driver_register(&ds28e30_driver);
}

static void __exit ds28e30_exit(void)
{
	ds_err("%s enter\n", __func__);

	platform_driver_unregister(&ds28e30_driver);
}

module_init(ds28e30_init);
module_exit(ds28e30_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WT inc.");
