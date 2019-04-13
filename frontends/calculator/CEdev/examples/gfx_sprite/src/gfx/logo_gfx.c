// convpng
#include <stdint.h>
#include "logo_gfx.h"

uint16_t logo_gfx_pal[189] = {
 0xFFFF,  // 00 :: rgb(255,255,255)
 0x7E20,  // 01 :: rgb(251,139,1)
 0xFE20,  // 02 :: rgb(251,140,4)
 0xFFFF,  // 03 :: rgb(255,253,253)
 0x6800,  // 04 :: rgb(212,2,2)
 0xE821,  // 05 :: rgb(213,11,11)
 0x7E20,  // 06 :: rgb(251,139,2)
 0x7E20,  // 07 :: rgb(251,139,0)
 0x7920,  // 08 :: rgb(244,72,0)
 0xFFFF,  // 09 :: rgb(255,254,254)
 0xFFFF,  // 10 :: rgb(255,254,253)
 0xFECC,  // 11 :: rgb(252,182,97)
 0x7A90,  // 12 :: rgb(249,163,128)
 0x7FDC,  // 13 :: rgb(254,244,231)
 0xF985,  // 14 :: rgb(245,100,40)
 0x7F9A,  // 15 :: rgb(253,225,213)
 0x7FDD,  // 16 :: rgb(253,242,242)
 0x6D6B,  // 17 :: rgb(226,90,90)
 0xFFBC,  // 18 :: rgb(254,239,233)
 0xF921,  // 19 :: rgb(244,77,7)
 0x7EED,  // 20 :: rgb(252,187,107)
 0xFF99,  // 21 :: rgb(254,231,203)
 0xFFFE,  // 22 :: rgb(255,253,250)
 0xFF16,  // 23 :: rgb(251,199,177)
 0xF921,  // 24 :: rgb(244,78,8)
 0xFE87,  // 25 :: rgb(251,167,61)
 0xFFBA,  // 26 :: rgb(254,237,216)
 0x7F31,  // 27 :: rgb(253,204,143)
 0xFF76,  // 28 :: rgb(253,222,183)
 0x7B7B,  // 29 :: rgb(249,220,220)
 0xFAF7,  // 30 :: rgb(244,192,192)
 0x7FDD,  // 31 :: rgb(252,241,241)
 0xF18C,  // 32 :: rgb(229,102,102)
 0xFE87,  // 33 :: rgb(251,164,56)
 0x7B5A,  // 34 :: rgb(247,212,212)
 0xFF76,  // 35 :: rgb(253,221,181)
 0xF652,  // 36 :: rgb(237,151,151)
 0x7652,  // 37 :: rgb(236,145,145)
 0x6D29,  // 38 :: rgb(224,74,74)
 0xE800,  // 39 :: rgb(212,3,3)
 0x718C,  // 40 :: rgb(228,98,98)
 0xFF54,  // 41 :: rgb(253,214,165)
 0x7673,  // 42 :: rgb(237,153,153)
 0xFF32,  // 43 :: rgb(252,205,148)
 0x7EA8,  // 44 :: rgb(251,169,68)
 0x6CC6,  // 45 :: rgb(219,48,48)
 0x7FBA,  // 46 :: rgb(254,234,211)
 0xE842,  // 47 :: rgb(215,20,20)
 0xFB7B,  // 48 :: rgb(250,224,224)
 0xFF77,  // 49 :: rgb(253,224,187)
 0xF652,  // 50 :: rgb(237,149,149)
 0x7F75,  // 51 :: rgb(253,218,175)
 0x7FFE,  // 52 :: rgb(254,249,249)
 0x7EEC,  // 53 :: rgb(252,185,102)
 0x7FBD,  // 54 :: rgb(252,236,236)
 0xFFFF,  // 55 :: rgb(255,253,251)
 0x7EED,  // 56 :: rgb(252,188,109)
 0x7FFF,  // 57 :: rgb(254,252,252)
 0xFAF7,  // 58 :: rgb(243,189,189)
 0x7B9B,  // 59 :: rgb(250,225,225)
 0x7AD2,  // 60 :: rgb(250,179,149)
 0xED09,  // 61 :: rgb(224,70,70)
 0xECA5,  // 62 :: rgb(218,43,43)
 0xFAF4,  // 63 :: rgb(250,190,165)
 0x6D4A,  // 64 :: rgb(225,82,82)
 0xED08,  // 65 :: rgb(223,69,69)
 0x6D08,  // 66 :: rgb(222,63,63)
 0x76D6,  // 67 :: rgb(242,178,178)
 0xECC6,  // 68 :: rgb(220,51,51)
 0xF1AD,  // 69 :: rgb(229,109,109)
 0xFFDD,  // 70 :: rgb(254,246,235)
 0x718C,  // 71 :: rgb(228,97,97)
 0xFFBC,  // 72 :: rgb(254,239,234)
 0xFF77,  // 73 :: rgb(253,223,187)
 0x76D6,  // 74 :: rgb(242,179,179)
 0x6863,  // 75 :: rgb(216,26,26)
 0x7AB1,  // 76 :: rgb(249,169,136)
 0xE843,  // 77 :: rgb(215,21,21)
 0xFF9C,  // 78 :: rgb(251,230,230)
 0xF9A6,  // 79 :: rgb(245,108,51)
 0x7F97,  // 80 :: rgb(253,225,189)
 0x7942,  // 81 :: rgb(244,81,13)
 0x7FBA,  // 82 :: rgb(254,236,215)
 0xFE21,  // 83 :: rgb(251,143,11)
 0x7FBC,  // 84 :: rgb(251,234,234)
 0xFB18,  // 85 :: rgb(245,197,197)
 0x76D6,  // 86 :: rgb(242,177,177)
 0x71AD,  // 87 :: rgb(229,106,106)
 0xFAB2,  // 88 :: rgb(249,176,144)
 0x7F9A,  // 89 :: rgb(253,227,217)
 0x7FDD,  // 90 :: rgb(254,242,237)
 0x7FDE,  // 91 :: rgb(253,243,243)
 0x7AB1,  // 92 :: rgb(249,171,139)
 0xFFBB,  // 93 :: rgb(254,239,221)
 0xF694,  // 94 :: rgb(240,167,167)
 0xFF7A,  // 95 :: rgb(253,224,213)
 0x7F0E,  // 96 :: rgb(253,193,117)
 0xFA6F,  // 97 :: rgb(249,159,121)
 0x7FFE,  // 98 :: rgb(255,251,247)
 0xFEAA,  // 99 :: rgb(251,175,80)
 0x7694,  // 100 :: rgb(239,161,161)
 0x7AF3,  // 101 :: rgb(250,186,160)
 0xE801,  // 102 :: rgb(213,5,5)
 0xFA6F,  // 103 :: rgb(249,159,122)
 0x7FDB,  // 104 :: rgb(254,241,224)
 0xF920,  // 105 :: rgb(244,75,4)
 0x7A8F,  // 106 :: rgb(249,160,123)
 0x7694,  // 107 :: rgb(239,163,163)
 0xE822,  // 108 :: rgb(214,14,14)
 0x6D29,  // 109 :: rgb(223,71,71)
 0xFF55,  // 110 :: rgb(253,215,169)
 0x7A8F,  // 111 :: rgb(249,161,124)
 0xFF32,  // 112 :: rgb(253,207,151)
 0x7FBB,  // 113 :: rgb(253,233,224)
 0xF694,  // 114 :: rgb(239,165,165)
 0xFFBC,  // 115 :: rgb(254,238,231)
 0xFB39,  // 116 :: rgb(247,208,208)
 0x7B38,  // 117 :: rgb(246,201,201)
 0x6842,  // 118 :: rgb(214,16,16)
 0x7FFF,  // 119 :: rgb(254,251,251)
 0xFB7B,  // 120 :: rgb(250,223,223)
 0xF16C,  // 121 :: rgb(227,95,95)
 0x7672,  // 122 :: rgb(237,152,152)
 0x7631,  // 123 :: rgb(235,138,138)
 0xFE87,  // 124 :: rgb(251,165,60)
 0xFFDD,  // 125 :: rgb(254,245,240)
 0xFF33,  // 126 :: rgb(252,208,154)
 0x7F79,  // 127 :: rgb(252,220,207)
 0x7E41,  // 128 :: rgb(251,144,12)
 0x7E42,  // 129 :: rgb(251,146,19)
 0x7F36,  // 130 :: rgb(251,202,182)
 0x7FDC,  // 131 :: rgb(254,243,230)
 0x7963,  // 132 :: rgb(244,89,24)
 0x7F0E,  // 133 :: rgb(252,193,119)
 0xFE87,  // 134 :: rgb(251,164,57)
 0x7A2B,  // 135 :: rgb(247,138,92)
 0xFA6E,  // 136 :: rgb(249,157,119)
 0xFF9B,  // 137 :: rgb(253,229,219)
 0x7920,  // 138 :: rgb(244,74,3)
 0xFFDE,  // 139 :: rgb(254,246,243)
 0xF964,  // 140 :: rgb(245,95,32)
 0xF921,  // 141 :: rgb(244,76,6)
 0xFAD3,  // 142 :: rgb(250,183,155)
 0xFAD3,  // 143 :: rgb(250,182,154)
 0xF985,  // 144 :: rgb(245,103,44)
 0x7FFE,  // 145 :: rgb(255,252,250)
 0x7FFE,  // 146 :: rgb(255,251,250)
 0x6800,  // 147 :: rgb(212,1,1)
 0x7B18,  // 148 :: rgb(245,195,195)
 0x7AD2,  // 149 :: rgb(250,178,148)
 0x7ECB,  // 150 :: rgb(251,179,87)
 0x7EA8,  // 151 :: rgb(251,169,69)
 0xFEAA,  // 152 :: rgb(251,176,82)
 0x7652,  // 153 :: rgb(236,146,146)
 0xF942,  // 154 :: rgb(244,83,16)
 0xF9E9,  // 155 :: rgb(246,126,76)
 0xFFDD,  // 156 :: rgb(254,246,242)
 0x71EF,  // 157 :: rgb(231,120,120)
 0xFB39,  // 158 :: rgb(247,206,206)
 0xFF37,  // 159 :: rgb(252,208,190)
 0x6D08,  // 160 :: rgb(222,65,65)
 0x7FDE,  // 161 :: rgb(253,244,244)
 0x7A4D,  // 162 :: rgb(248,147,106)
 0xFB5A,  // 163 :: rgb(248,216,216)
 0x7F79,  // 164 :: rgb(253,220,206)
 0xFAD3,  // 165 :: rgb(250,181,153)
 0x7F15,  // 166 :: rgb(251,196,173)
 0x7F31,  // 167 :: rgb(252,201,137)
 0xF9C8,  // 168 :: rgb(246,116,62)
 0xFAB2,  // 169 :: rgb(250,176,145)
 0xFF10,  // 170 :: rgb(252,198,131)
 0x7A6E,  // 171 :: rgb(248,155,117)
 0xFF9B,  // 172 :: rgb(253,232,224)
 0xFFBB,  // 173 :: rgb(254,240,221)
 0x7920,  // 174 :: rgb(244,74,2)
 0x7FDC,  // 175 :: rgb(254,243,229)
 0xFE21,  // 176 :: rgb(251,141,6)
 0x7F53,  // 177 :: rgb(253,210,155)
 0xF9E9,  // 178 :: rgb(247,127,77)
 0xFF58,  // 179 :: rgb(252,213,197)
 0xFE87,  // 180 :: rgb(251,164,58)
 0xFF54,  // 181 :: rgb(253,213,164)
 0xFEA9,  // 182 :: rgb(251,174,78)
 0xFE21,  // 183 :: rgb(251,143,10)
 0x79E8,  // 184 :: rgb(246,120,67)
 0x7E87,  // 185 :: rgb(251,163,54)
 0xFFDD,  // 186 :: rgb(254,248,240)
 0x7F57,  // 187 :: rgb(252,210,193)
 0x6800,  // 188 :: rgb(212,0,0)
};