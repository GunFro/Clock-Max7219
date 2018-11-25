const uint8_t fontB11pt7bBitmaps[] PROGMEM = {
  0xFF, 0xFF, 0xFC, 0x0F, 0x03, 0xC0, 0xF0, 0x3C, 0x0F, 0x03, 0xC0, 0xF0, 
  0x3C, 0x0F, 0x03, 0xFF, 0xFF, 0xF0, 0x0C, 0x07, 0x03, 0xC0, 0x30, 0x0C, 
  0x03, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0x00, 
  0xFF, 0xFF, 0xF0, 0x0C, 0x03, 0x00, 0xC0, 0x3F, 0xFF, 0xFF, 0xC0, 0x30, 
  0x0C, 0x03, 0x00, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xF0, 0x0C, 0x03, 0x00, 
  0xC0, 0x30, 0xFC, 0x3F, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0xFF, 0xFF, 0xF0, 
  0xC0, 0xF0, 0x3C, 0x0F, 0x03, 0xC0, 0xF0, 0x3F, 0xFF, 0xFF, 0x00, 0xC0, 
  0x30, 0x0C, 0x03, 0x00, 0xC0, 0x30, 0xFF, 0xFF, 0xFC, 0x03, 0x00, 0xC0, 
  0x30, 0x0F, 0xFF, 0xFF, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0xFF, 0xFF, 0xF0, 
  0xFF, 0xFF, 0xFC, 0x03, 0x00, 0xC0, 0x30, 0x0F, 0xFF, 0xFF, 0xC0, 0xF0, 
  0x3C, 0x0F, 0x03, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xF0, 0x0C, 0x03, 0x00, 
  0xC0, 0x30, 0x0C, 0x03, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0xC0, 0x30, 
  0xFF, 0xFF, 0xFC, 0x0F, 0x03, 0xC0, 0xF0, 0x3F, 0xFF, 0xFF, 0xC0, 0xF0, 
  0x3C, 0x0F, 0x03, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFC, 0x0F, 0x03, 0xC0, 
  0xF0, 0x3F, 0xFF, 0xFF, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0xFF, 0xFF, 0xF0, 
  0x03, 0xC0, 0x3C, 0x00
};

const GFXglyph fontB11pt7bGlyphs[] PROGMEM = {
  {     0,   0,   0,  13,    0,    1 }   // ' '
 ,{     0,   0,   0,   1,    0,    1 }   // '!'
 ,{     0,   0,   0,   1,    0,    1 }   // '"'
 ,{     0,   0,   0,   1,    0,    1 }   // '#'
 ,{     0,   0,   0,   1,    0,    1 }   // '$'
 ,{     0,   0,   0,   1,    0,    1 }   // '%'
 ,{     0,   0,   0,   1,    0,    1 }   // '&'
 ,{     0,   0,   0,   1,    0,    1 }   // '''
 ,{     0,   0,   0,   1,    0,    1 }   // '('
 ,{     0,   0,   0,   1,    0,    1 }   // ')'
 ,{     0,   0,   0,   1,    0,    1 }   // '*'
 ,{     0,   0,   0,   1,    0,    1 }   // '+'
 ,{     0,   0,   0,   1,    0,    1 }   // ','
 ,{     0,   0,   0,   1,    0,    1 }   // '-'
 ,{     0,   0,   0,   1,    0,    1 }   // '.'
 ,{     0,   0,   0,   1,    0,    1 }   // '/'
 ,{     0,  10,  14,  13,    0,  -17 }   // '0'
 ,{    18,  10,  14,  13,    0,  -17 }   // '1'
 ,{    36,  10,  14,  13,    0,  -17 }   // '2'
 ,{    54,  10,  14,  13,    0,  -17 }   // '3'
 ,{    72,  10,  14,  13,    0,  -17 }   // '4'
 ,{    90,  10,  14,  13,    0,  -17 }   // '5'
 ,{   108,  10,  14,  13,    0,  -17 }   // '6'
 ,{   126,  10,  14,  13,    0,  -17 }   // '7'
 ,{   144,  10,  14,  13,    0,  -17 }   // '8'
 ,{   162,  10,  14,  13,    0,  -17 }   // '9'
 ,{   180,   2,  14,   5,    0,  -17 }   // ':'
};

const GFXfont fontB11pt7b PROGMEM = {
  (uint8_t  *)fontB11pt7bBitmaps,
  (GFXglyph *)fontB11pt7bGlyphs,
  0x20, 0x3A, 14
};
