/*=================================================
 :		built-in EEPROM routine
 :	set_EEPROM();						//set EE_mem[] table page
 :	dest = read_EE(addr); 				//dest <= EE_mem[addr]
 :	write_EE(addr,data);  				//EE_mem[addr] <= data
 :	write_EE_array(addr, &data, size)	//EE_mem[addr] <= data[]
=================================================*/
#define read_EE(addr)				read_EE1(__builtin_tbloffset(EE_mem) + (addr)*2)
#define write_EE(addr,data) write_EE1(__builtin_tbloffset(EE_mem) + (addr)*2, data)

extern int read_EE1(int tbl_offset);
extern void write_EE1(int tbl_offset, int data);		//4ms
