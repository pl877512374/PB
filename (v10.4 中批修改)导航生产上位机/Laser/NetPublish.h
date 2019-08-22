//版本W3.13


/*对外接口,要导出的部分*/        

/************************************************************************/
/*   
	 函数名字：     SendKey_rq 
	 参数1：		输入芯片8字节ID
	 参数2：		输出8字节的二级密钥
	 作用：			根据芯片序列号输出二级密钥
*/
/************************************************************************/
extern "C" int _stdcall SendKey_rq(unsigned char *input_chipid,unsigned char *input_rootkey,unsigned char *output_Result);

/************************************************************************/
/*   
	 函数名字：     SendRand_rq 
	 参数1：		输出32字节的随机数
	 作用：			计算随机数
*/
/************************************************************************/

extern "C" int _stdcall SendRand_rq(unsigned char *out_rand_data);

/************************************************************************/
/*   
	 函数名字：     SendRand_rs 
	 参数1：		输入32字节随机数
	 参数2：		输入8字节的二级密钥
	 参数3：		输入MAC验证码
	 作用：			利用参数1和参数2完成对参数3的验证，返回 0 验证通过，否则失败。
*/
/************************************************************************/

extern "C" int _stdcall SendRand_rs(unsigned char *in_rand, unsigned char *subkey, unsigned char *encode);
/************************************************************************/
/*   
	 函数名字：     Encode 
	 参数1：		输入明文长度
	 参数2：		输入明文
	 参数3：		输入密钥,(加密二级分散密钥的密钥暂时定为（8字节ChipID+8字节ChipID）)
	 参数4：        输出密文
	 作用：			加密明文，不为8字节的自动补齐80 00 00..(n).直到为8的整数倍
*/
/************************************************************************/
extern "C" int _stdcall Encode(unsigned int Input_Len,unsigned char *in_wait_code, unsigned char *tempkey, unsigned char *secretcode);

/************************************************************************/
/*   
	 函数名字：     decode 
	 参数1：		输入密文长度
	 参数2：		输入密文
	 参数3：		输入密钥
	 参数4：        输出明文
	 作用：			解密密文
*/
/************************************************************************/
extern "C" int _stdcall Decode(unsigned int Input_Len,unsigned char *in_wait_code, unsigned char *tempkey, unsigned char *plaincode);


/************************************************************************/
/*   
	 函数名字：     hashCalMac 
	 参数1：		输入8字节密钥
	 参数2：		输入32字节的存储器数据
	 参数3：		输入8字节的随机数
	 参数4：        输入HASH因子 TA1
	 参数5：		输入芯片ID
	 参数6：		*输出20字节的HashMac.
	 作用：			通过hash算法计算MAC值
*/
/************************************************************************/
extern "C" int _stdcall hashCalMac(unsigned char *devicesecret, unsigned char *pageback,unsigned char *subpbuffer,int TA1, unsigned char *ROMID, unsigned char *SHAVM_MAC);

