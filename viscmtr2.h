extern union tagPacket {
   struct {
      unsigned short vt;
      unsigned short im;
      signed short temp;
      unsigned short crc;
      };
   unsigned char raw[1];
   } data;
