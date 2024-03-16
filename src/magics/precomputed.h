#ifndef INTEGRAL_MAGICS_PRECOMPUTED_H_
#define INTEGRAL_MAGICS_PRECOMPUTED_H_

#include "entry.h"
#include <array>

namespace magics {

const std::array<MagicEntry, Square::kSquareCount> kRookMagics = {{
                                                                      {0x000101010101017eULL, 0x2480001240002088ULL, 52},
                                                                      {0x000202020202027cULL, 0x0040002000405000ULL, 53},
                                                                      {0x000404040404047aULL, 0x2080088010012001ULL, 53},
                                                                      {0x0008080808080876ULL, 0x0080049001080080ULL, 53},
                                                                      {0x001010101010106eULL, 0x0b00020800945100ULL, 53},
                                                                      {0x002020202020205eULL, 0x04800a0080211400ULL, 53},
                                                                      {0x004040404040403eULL, 0xc0801a0000800100ULL, 53},
                                                                      {0x008080808080807eULL, 0x0600084300802204ULL, 52},
                                                                      {0x0001010101017e00ULL, 0x4240800140006080ULL, 53},
                                                                      {0x0002020202027c00ULL, 0x0889002080400111ULL, 54},
                                                                      {0x0004040404047a00ULL, 0x4823004210200100ULL, 54},
                                                                      {0x0008080808087600ULL, 0xa000808008001000ULL, 54},
                                                                      {0x0010101010106e00ULL, 0x0020800800340082ULL, 54},
                                                                      {0x0020202020205e00ULL, 0x004100040039004eULL, 54},
                                                                      {0x0040404040403e00ULL, 0x0802003408150200ULL, 54},
                                                                      {0x0080808080807e00ULL, 0x0800800080104100ULL, 53},
                                                                      {0x00010101017e0100ULL, 0x0040008002245080ULL, 53},
                                                                      {0x00020202027c0200ULL, 0x0030004003402000ULL, 54},
                                                                      {0x00040404047a0400ULL, 0x2400110042600100ULL, 54},
                                                                      {0x0008080808760800ULL, 0xa000808008001000ULL, 54},
                                                                      {0x00101010106e1000ULL, 0x0123030004080010ULL, 54},
                                                                      {0x00202020205e2000ULL, 0x10c0808012000400ULL, 54},
                                                                      {0x00404040403e4000ULL, 0x0200040022080110ULL, 54},
                                                                      {0x00808080807e8000ULL, 0x028a420000a10844ULL, 53},
                                                                      {0x000101017e010100ULL, 0x8040028480014021ULL, 53},
                                                                      {0x000202027c020200ULL, 0x0841006d00400082ULL, 54},
                                                                      {0x000404047a040400ULL, 0x0803004300146004ULL, 54},
                                                                      {0x0008080876080800ULL, 0x0000500280080080ULL, 54},
                                                                      {0x001010106e101000ULL, 0x0806140080080081ULL, 54},
                                                                      {0x002020205e202000ULL, 0x0014820080800400ULL, 54},
                                                                      {0x004040403e404000ULL, 0x1004084400021001ULL, 54},
                                                                      {0x008080807e808000ULL, 0x0800800080104100ULL, 53},
                                                                      {0x0001017e01010100ULL, 0x0080800301002041ULL, 53},
                                                                      {0x0002027c02020200ULL, 0x0030004003402000ULL, 54},
                                                                      {0x0004047a04040400ULL, 0x9200502001004102ULL, 54},
                                                                      {0x0008087608080800ULL, 0x0c0200100a004020ULL, 54},
                                                                      {0x0010106e10101000ULL, 0x0141018801001004ULL, 54},
                                                                      {0x0020205e20202000ULL, 0x2248040080801a00ULL, 54},
                                                                      {0x0040403e40404000ULL, 0x0000100804000201ULL, 54},
                                                                      {0x0080807e80808000ULL, 0x0806004086002514ULL, 53},
                                                                      {0x00017e0101010100ULL, 0x10807880c0008000ULL, 53},
                                                                      {0x00027c0202020200ULL, 0x0800200250004002ULL, 54},
                                                                      {0x00047a0404040400ULL, 0x0102100020008080ULL, 54},
                                                                      {0x0008760808080800ULL, 0x8200300021010009ULL, 54},
                                                                      {0x00106e1010101000ULL, 0x6280040801010010ULL, 54},
                                                                      {0x00205e2020202000ULL, 0x0020844010280120ULL, 54},
                                                                      {0x00403e4040404000ULL, 0x00000a0108040010ULL, 54},
                                                                      {0x00807e8080808000ULL, 0x0000040260820001ULL, 53},
                                                                      {0x007e010101010100ULL, 0x0040008002245080ULL, 53},
                                                                      {0x007c020202020200ULL, 0x0010600092c00b80ULL, 54},
                                                                      {0x007a040404040400ULL, 0x8006900188200080ULL, 54},
                                                                      {0x0076080808080800ULL, 0x0080601202084200ULL, 54},
                                                                      {0x006e101010101000ULL, 0x4424008115880080ULL, 54},
                                                                      {0x005e202020202000ULL, 0x0002000408308600ULL, 54},
                                                                      {0x003e404040404000ULL, 0x0100080201100400ULL, 54},
                                                                      {0x007e808080808000ULL, 0x008005025c940a00ULL, 53},
                                                                      {0x7e01010101010100ULL, 0x0220409300208001ULL, 52},
                                                                      {0x7c02020202020200ULL, 0x8002400980241101ULL, 53},
                                                                      {0x7a04040404040400ULL, 0x5402005280200842ULL, 53},
                                                                      {0x7608080808080800ULL, 0x3801000850000423ULL, 53},
                                                                      {0x6e10101010101000ULL, 0x300a002008043142ULL, 53},
                                                                      {0x5e20202020202000ULL, 0x1009000400220821ULL, 53},
                                                                      {0x3e40404040404000ULL, 0x9240028810012224ULL, 53},
                                                                      {0x7e80808080808000ULL, 0x0001940028410092ULL, 52}
                                                                  }};

const std::array<MagicEntry, Square::kSquareCount> kBishopMagics = {{
                                                                        {0x0040201008040200ULL, 0x0040440812022420ULL, 58},
                                                                        {0x0000402010080400ULL, 0x3209080800504480ULL, 59},
                                                                        {0x0000004020100a00ULL, 0x401000a189000040ULL, 59},
                                                                        {0x0000000040221400ULL, 0x2104070200108490ULL, 59},
                                                                        {0x0000000002442800ULL, 0x4004242000403041ULL, 59},
                                                                        {0x0000000204085000ULL, 0x0001012010880821ULL, 59},
                                                                        {0x0000020408102000ULL, 0x242108084a490101ULL, 59},
                                                                        {0x0002040810204000ULL, 0x200202b108080400ULL, 58},
                                                                        {0x0020100804020000ULL, 0x0000044448020400ULL, 59},
                                                                        {0x0040201008040000ULL, 0xc200088808208020ULL, 59},
                                                                        {0x00004020100a0000ULL, 0x0000105182810000ULL, 59},
                                                                        {0x0000004022140000ULL, 0x180078204c400000ULL, 59},
                                                                        {0x0000000244280000ULL, 0x0001060a10005040ULL, 59},
                                                                        {0x0000020408500000ULL, 0x00c0950120100000ULL, 59},
                                                                        {0x0002040810200000ULL, 0x0000050110026100ULL, 59},
                                                                        {0x0004081020400000ULL, 0x200202b108080400ULL, 59},
                                                                        {0x0010080402000200ULL, 0x4808000410640820ULL, 59},
                                                                        {0x0020100804000400ULL, 0x10200a0206020e00ULL, 59},
                                                                        {0x004020100a000a00ULL, 0x1002001003220020ULL, 57},
                                                                        {0x0000402214001400ULL, 0x0808000082004000ULL, 57},
                                                                        {0x0000024428002800ULL, 0x2001001820280200ULL, 57},
                                                                        {0x0002040850005000ULL, 0x1101008201030552ULL, 57},
                                                                        {0x0004081020002000ULL, 0x0151000245082000ULL, 59},
                                                                        {0x0008102040004000ULL, 0x0001001024032462ULL, 59},
                                                                        {0x0008040200020400ULL, 0x4820042010942810ULL, 59},
                                                                        {0x0010080400040800ULL, 0x00a8210082040900ULL, 59},
                                                                        {0x0020100a000a1000ULL, 0x8180a08010010040ULL, 57},
                                                                        {0x0040221400142200ULL, 0xc24004002a010810ULL, 55},
                                                                        {0x0002442800284400ULL, 0x0003001001004012ULL, 55},
                                                                        {0x0004085000500800ULL, 0x0010038022405020ULL, 57},
                                                                        {0x0008102000201000ULL, 0x01042100040a8200ULL, 59},
                                                                        {0x0010204000402000ULL, 0x0000444049040200ULL, 59},
                                                                        {0x0004020002040800ULL, 0x2001101000882008ULL, 59},
                                                                        {0x0008040004081000ULL, 0x0158026600580800ULL, 59},
                                                                        {0x00100a000a102000ULL, 0x4001680804440061ULL, 57},
                                                                        {0x0022140014224000ULL, 0x4000400a00042200ULL, 55},
                                                                        {0x0044280028440200ULL, 0x0000420021020080ULL, 55},
                                                                        {0x0008500050080400ULL, 0x0010020020020080ULL, 57},
                                                                        {0x0010200020100800ULL, 0x5008010040210800ULL, 59},
                                                                        {0x0020400040201000ULL, 0x000604002001108cULL, 59},
                                                                        {0x0002000204081000ULL, 0x0808010920050821ULL, 59},
                                                                        {0x0004000408102000ULL, 0x4000c41220020800ULL, 59},
                                                                        {0x000a000a10204000ULL, 0x4000804040400808ULL, 57},
                                                                        {0x0014001422400000ULL, 0x0000814200809802ULL, 57},
                                                                        {0x0028002844020000ULL, 0x0000080104020041ULL, 57},
                                                                        {0x0050005008040200ULL, 0x0414010c42000101ULL, 57},
                                                                        {0x0020002010080400ULL, 0x3209080800504480ULL, 59},
                                                                        {0x0040004020100800ULL, 0x8031050202000280ULL, 59},
                                                                        {0x0000020408102000ULL, 0x242108084a490101ULL, 59},
                                                                        {0x0000040810204000ULL, 0x082080481a101000ULL, 59},
                                                                        {0x00000a1020400000ULL, 0x4000004a00902042ULL, 59},
                                                                        {0x0000142240000000ULL, 0x0100218108480800ULL, 59},
                                                                        {0x0000284402000000ULL, 0x0c28201202020840ULL, 59},
                                                                        {0x0000500804020000ULL, 0x0c28201202020840ULL, 59},
                                                                        {0x0000201008040200ULL, 0x0424040408020088ULL, 59},
                                                                        {0x0000402010080400ULL, 0x3209080800504480ULL, 59},
                                                                        {0x0002040810204000ULL, 0x200202b108080400ULL, 58},
                                                                        {0x0004081020400000ULL, 0x200202b108080400ULL, 59},
                                                                        {0x000a102040000000ULL, 0x0034c00448441005ULL, 59},
                                                                        {0x0014224000000000ULL, 0x0000810850208802ULL, 59},
                                                                        {0x0028440200000000ULL, 0x02000808403d0101ULL, 59},
                                                                        {0x0050080402000000ULL, 0x0000801006900108ULL, 59},
                                                                        {0x0020100804020000ULL, 0x0000044448020400ULL, 59},
                                                                        {0x0040201008040200ULL, 0x0040440812022420ULL, 58}
                                                                    }};

}

#endif // INTEGRAL_MAGICS_PRECOMPUTED_H_