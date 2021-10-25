struct  TRead_buffer {
      unsigned short vt;
      unsigned short im;
      signed short temp;
      unsigned short crc;
      };



int set_interface_attribs(int fd);

void set_mincount(int fd, int mcount);

int POpen(char *portname);

 int PWrite(int fd, unsigned char wdata);

struct TRead_buffer PRead(int fd);



