#include "common.h"

/*************************************************************************************
 * Function : convert_hex_to_int ()
 * Input    : number,output
 * Output   : 
 * Purpose  : converting hex to int
 * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int convert_hex_to_int (unsigned char *val_in_hex)
{
	int int_val = *(val_in_hex + 1);
	if (*val_in_hex)
	{
		int_val += *(val_in_hex) * 256;	
	}
	return int_val;
}

/*************************************************************************************
 * Function : convert_int_to_hex()
 * Input    : number,output
 * Output   : 
 * Purpose  : converting integer to hex
 * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
void convert_int_to_hex (unsigned int number, unsigned char *output)
{
	unsigned int temp_number;
	
	temp_number = ( number >> R_SHIFT_24);
	memcpy ((void *)&output[0], &temp_number, 1);
	temp_number = ( number >> R_SHIFT_16);
	memcpy ((void *)&output[1],&temp_number, 1);
	temp_number = ( number >> R_SHIFT_8);
	memcpy ((void *)&output[2], &temp_number, 1);
	memcpy ((void *)&output[3], &number, 1);
}

/*************************************************************************************
  * Function : convert_hex_to_pc ()
  * Input    : m3ua_msg_pc_ptr,ptr
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
void convert_hex_to_pc (unsigned char *ptr, char *m3ua_msg_pc_ptr)
{
	unsigned char msb_byte ;
	unsigned char middle_byte ;
	unsigned char lsb_byte ;
	unsigned char temp_byte;
	
	temp_byte = (unsigned char) m3ua_msg_pc_ptr[2];
	msb_byte = temp_byte >> 3;
	temp_byte = m3ua_msg_pc_ptr[2];
	middle_byte = temp_byte << 5;
	temp_byte = (unsigned char) m3ua_msg_pc_ptr[3];
	temp_byte = temp_byte >> 3;
	middle_byte = middle_byte | temp_byte;
	temp_byte = (unsigned char) m3ua_msg_pc_ptr[3];
	lsb_byte = temp_byte & 7;	

	sprintf ((char*)ptr, "%d-%d-%d", (msb_byte), (middle_byte), (lsb_byte));	
}

/*************************************************************************************
 * Function : inet_aton_buff()
 * Input    : ip_addr,result
 * Output   : 
 * Purpose  : 
 * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
void inet_aton_buff (char *ip_addr, unsigned char *result)
{
	long int long_addr = 0;
	long_addr = ntohl (inet_addr (ip_addr));
	int remainder, iter = 7, small_iter;
	unsigned char conv_vals[8] = {0};

	for (; long_addr > 16; --iter)
	{
		remainder = long_addr % 16;
		conv_vals[iter] = remainder;
		long_addr /= 16;
	}
	remainder = long_addr % 16;
	conv_vals[iter] = remainder;
	iter = 0;

	for (small_iter = 0; small_iter < 4; ++small_iter)
	{
		result[small_iter] = (conv_vals[iter] * 16) + conv_vals[iter + 1];
		iter += 2;
	}
}


/*************************************************************************************
  * Function : encode_number_or_time()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
inline void encode_number_or_time(unsigned char *input,
			 unsigned char * output, int input_length)
{
	int counter_out = 0;
	int counter_in	= 0;
	char input_len	= 0;
	memcpy ((void *)&input_len, (void *)&input_length, sizeof (char));

	while (counter_in < input_length - 1)
	{
		output [counter_out] = (((input [counter_in] - ASCII_ZERO)) | 
				((input [counter_in + 1] - ASCII_ZERO) << 4));
		++counter_out ;
		counter_in = counter_in + 2;
	}

	if (input_length % 2 == 1) {
		output [counter_out] = (((input [counter_in] - ASCII_ZERO)) |
			   	(input_len << 4));
	}   
	output [counter_out + 1] = '\0';
}   
/*************************************************************************************
  * Function : decode_number_or_time()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
inline void decode_number_or_time (unsigned char *input, unsigned char *output, 
		int input_length)
{
	int counter_out = 0;
	int counter_in	= 0;
	while (counter_in < input_length)
	{
		counter_out = counter_in * 2;
		output [counter_out] = ((input [counter_in] & 0x0f) + 48);
		output [counter_out + 1] = 
			(((input [counter_in] & 0xf0) >> 4) + 48);
		++counter_in;   
	}   
	output [counter_out + 2] = '\0';
}  
