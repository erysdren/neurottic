/*
MIT License

Copyright (c) 2024 erysdren (it/she/they)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software", to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "neurottic.h"

static const char *wall_tiles[] = {
	/* 0 */
	[1] = "WALL1",
	[2] = "WALL2",
	[3] = "WALL3",
	[4] = "WALL4",
	[5] = "WALL5",
	[6] = "WALL6",
	[7] = "WALL7",
	[8] = "WALL8",
	[9] = "WALL9",
	[10] = "WALL10",
	[11] = "WALL11",
	[12] = "WALL12",
	[13] = "WALL13",
	[14] = "WALL14",
	[15] = "WALL15",
	[16] = "WALL16",
	[17] = "WALL17",
	[18] = "WALL18",
	[19] = "WALL19",
	[20] = "WALL20",
	[21] = "WALL21",
	[22] = "WALL22",
	[23] = "WALL23",
	[24] = "WALL24",
	[25] = "WALL25",
	[26] = "WALL26",
	[27] = "WALL27",
	[28] = "WALL28",
	[29] = "WALL29",
	[30] = "WALL30",
	[31] = "WALL31",
	[32] = "WALL32",
	/* 33 */
	/* 34 */
	/* 35 */
	[36] = "WALL33",
	[37] = "WALL34",
	[38] = "WALL35",
	[39] = "WALL36",
	[40] = "WALL37",
	[41] = "WALL38",
	[42] = "WALL39",
	[43] = "WALL40",
	/* 44 */
	/* 45 */
	[46] = "WALL74",
	[47] = "EXIT",
	[48] = "ENTRANCE",
	[49] = "WALL41",
	[50] = "WALL42",
	[51] = "WALL43",
	[52] = "WALL44",
	[53] = "WALL45",
	[54] = "WALL46",
	[55] = "WALL47",
	[56] = "WALL48",
	[57] = "WALL49",
	[58] = "WALL50",
	[59] = "WALL51",
	[60] = "WALL52",
	[61] = "WALL53",
	[62] = "WALL54",
	[63] = "WALL55",
	[64] = "WALL56",
	[65] = "WALL57",
	[66] = "WALL58",
	[67] = "WALL59",
	[68] = "WALL60",
	[69] = "WALL61",
	[70] = "WALL62",
	[71] = "WALL63",
	[72] = "ELEV1",
	[73] = "ELEV2",
	[74] = "ELEV3",
	[75] = "ELEV4",
	[76] = "ELEV5",
	[77] = "ELEV6",
	[78] = "ELEV7",
	[79] = "ELEV8",
	[80] = "WALL64",
	[81] = "WALL65",
	[82] = "WALL66",
	[83] = "WALL67",
	[84] = "WALL68",
	[85] = "WALL69",
	[86] = "WALL70",
	[87] = "WALL71",
	[88] = "WALL72",
	[89] = "WALL73"
};

static const int num_wall_tiles = sizeof(wall_tiles) / sizeof(const char *);

/* retrieve texture name from tile index */
const char *DarkWar_GetWallTexture(int tile)
{
	if (tile < 0 || tile >= num_wall_tiles)
		return NULL;

	return wall_tiles[tile];
}
