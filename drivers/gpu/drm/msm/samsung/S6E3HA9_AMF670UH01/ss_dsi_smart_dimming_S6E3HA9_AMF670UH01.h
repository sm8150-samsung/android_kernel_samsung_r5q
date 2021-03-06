/*
 * =================================================================
 *
 *       Filename:  smart_mtp_s6e3.h
 *
 *    Description:  Smart dimming algorithm implementation
 *
 *        Company:  Samsung Electronics
 *
 * ================================================================
 */
/*
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2012, Samsung Electronics. All rights reserved.

 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _SS_DSI_SMART_DIMMING_S6E3HA9_AMF670UH01_H_
#define _SS_DSI_SMART_DIMMING_S6E3HA9_AMF670UH01_H_

#include "ss_dsi_panel_common.h"
#include "ss_dsi_smart_dimming_common.h"

static int id1, id2, id3;

#define LUMINANCE_MAX 74
#define HMT_LUMINANCE_MAX 37
#define GAMMA_SET_MAX 34
#define GRAY_SCALE_MAX 256
#define RGB_COMPENSATION 27

/* BIT_SHIFT is used for right bit shfit */
#define BIT_SHIFT 22
#define BIT_SHFIT_MUL 4194304 // pow(2,BIT_SHIFT)

#define VREG1_REF_6P8 28521267	/* 6.8*4194304 */
#define VREG1_REF_6P5 27262976	/* 6.5*4194304 */
#define VREG1_REF_6P4 26843546	/* 6.4*4194304 */
#define VREG1_REF_6P3 26424115	/* 6.3*4194304 */
#define VREG1_REF_6P2 26004685	/* 6.2*4194304 */

#define VREF_1P0 4194304 /* 1.0*4194304 */

#define HBM_INTERPOLATION_STEP 80

enum {
	GAMMA_CURVE_1P9 = 0,
	GAMMA_CURVE_2P15,
	GAMMA_CURVE_2P2,
	GAMMA_CURVE_2P2_400CD,
};

/* TP */
enum {
	VT = 0,
	V0,
	V1,
	V7,
	V11,
	V23,
	V35,
	V51,
	V87,
	V151,
	V203,
	V255,
	V_MAX,
};

/* RGB */
enum {
	R = 0,
	G = 1,
	B = 2,
	RGB_MAX,
};

static char V_LIST[V_MAX][5] = {
	"VT",
	"V0",
	"V1",
	"V7",
	"V11",
	"V23",
	"V35",
	"V51",
	"V87",
	"V151",
	"V203",
	"V255",
};

struct illuminance_table {
	int lux;
	char gamma_setting[GAMMA_SET_MAX];
} __packed;

struct SMART_DIM {
	/* read C8h 1st ~ 35th */
	char MTP_ORIGN[GAMMA_SET_MAX];

	/* copy MTP_ORIGN -> MTP */
	int MTP[V_MAX][RGB_MAX];

	/* TP's gamma voltage */
	int RGB_OUTPUT[V_MAX][RGB_MAX];

	/* GRAY (0~255) */
	int GRAY[GRAY_SCALE_MAX][RGB_MAX];

	/* Because of AID funtion, below members are added*/
	int lux_table_max;
	int *plux_table;
	struct illuminance_table gen_table[LUMINANCE_MAX];

	int brightness_level;
	int ldi_revision;
	int vregout_voltage;
	int vref;
	char panel_revision;

	/* HBM interpolation */
	struct illuminance_table hbm_interpolation_table[HBM_INTERPOLATION_STEP + 1];
	char *hbm_payload;
	int hbm_brightness_level;

	struct illuminance_table hmt_gen_table[HMT_LUMINANCE_MAX];
} __packed;

/* V0,V1,V7,V11,V23,V35,V51,V87,V151,V203,V255 */
static int INFLECTION_VOLTAGE_ARRAY[V_MAX] = {0, 0, 1, 7, 11, 23, 35, 51, 87, 151, 203, 255};

static int vt_coefficient[] = {
	0, 24, 48, 72,
	96, 120, 144, 168,
	192, 216, 276, 296,
	316, 336, 356, 372
};

static int v0_coefficient[] = {
	0, 12, 24, 36,
	48, 60, 72, 84,
	96, 108, 120, 132,
	144, 156, 168, 180
};

/* center (max) gamma value (Hex) */
static int center_gamma[V_MAX][RGB_MAX] = {
	{0x0, 0x0, 0x0},	/* VT */
	{0x03, 0x03, 0x03},	/* V0 */
	{0x80, 0x80, 0x80},	/* V1 */
	{0x80, 0x80, 0x80},	/* V7 */
	{0x80, 0x80, 0x80},	/* V11 */
	{0x80, 0x80, 0x80},	/* V23 */
	{0x80, 0x80, 0x80},	/* V35 */
	{0x80, 0x80, 0x80},	/* V51 */
	{0x80, 0x80, 0x80},	/* V87 */
	{0x80, 0x80, 0x80},	/* V151 */
	{0x80, 0x80, 0x80},	/* V203 */
	{0x200, 0x200, 0x200},	/* V255 */
};

/* fraction for gamma code */
static int fraction[V_MAX][2] = {
/* {numerator, denominator} */
	{0,  1200},		/* VT */
	{0,  1200},		/* V0 */
	{0,  256},		/* V1 */
	{0,  256},		/* V7 */
	{64, 320},		/* V11 */
	{64, 320},		/* V23 */
	{64, 320},		/* V35 */
	{64, 320},		/* V51 */
	{64, 320},		/* V87 */
	{64, 320},		/* V151 */
	{64, 320},		/* V203 */
	{177, 1200},		/* V255 */
};

static int hbm_interpolation_candela_table_revA[HBM_INTERPOLATION_STEP] = {
	425, 430, 433, 438, 444, 448, 453, 458, 463, 468,
	472, 477, 481, 486, 491, 495, 500, 505, 510, 515,
	520, 525, 528, 533, 539, 543, 548, 553, 558, 563,
	567, 572, 576, 581, 586, 591, 596, 599, 604, 610,
	614, 619, 624, 629, 634, 639, 644, 647, 652, 658,
	662, 667, 671, 676, 681, 686, 691, 694, 699, 705,
	710, 715, 718, 723, 729, 733, 738, 743, 748, 753,
	757, 762, 766, 771, 776, 780, 785, 790, 795, 800,
};

static unsigned int base_luminance_star_revA[LUMINANCE_MAX][2] = {
	{2, 92},
	{3, 92},
	{4, 92},
	{5, 92},
	{6, 92},
	{7, 92},
	{8, 92},
	{9, 92},
	{10, 92},
	{11, 92},
	{12, 92},
	{13, 92},
	{14, 92},
	{15, 92},
	{16, 92},
	{17, 92},
	{19, 92},
	{20, 92},
	{21, 92},
	{22, 92},
	{24, 92},
	{25, 92},
	{27, 92},
	{29, 92},
	{30, 92},
	{32, 92},
	{34, 92},
	{37, 92},
	{39, 92},
	{41, 92},
	{44, 92},
	{47, 92},
	{50, 92},
	{53, 92},
	{56, 92},
	{60, 94},
	{64, 100},
	{68, 105},
	{72, 112},
	{77, 119},
	{82, 125},
	{87, 131},
	{93, 140},
	{98, 151},
	{105, 161},
	{111, 168},
	{119, 181},
	{126, 190},
	{134, 205},
	{143, 217},
	{152, 233},
	{162, 246},
	{172, 265},
	{183, 280},
	{195, 280},
	{207, 280},
	{220, 280},
	{234, 280},
	{249, 289},
	{265, 307},
	{282, 325},
	{300, 341},
	{316, 354},
	{333, 371},
	{350, 388},
	{357, 395},
	{365, 402},
	{372, 402},
	{380, 402},
	{387, 402},
	{395, 402},
	{403, 409},
	{412, 416},
	{420, 420},
};

/* Gray leveled shift */
static int gradation_offset_star_revA[LUMINANCE_MAX][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{-3, 4, 3, 10, 15, 20, 24, 26, 24},
	{-2, 3, 4, 7, 12, 16, 19, 22, 23},
	{-1, 4, 4, 5, 9, 13, 16, 18, 19},
	{-2, 1, 2, 3, 7, 10, 13, 16, 17},
	{-2, 0, 1, 3, 5, 9, 12, 14, 16},
	{-1, 3, 2, 3, 5, 8, 11, 14, 15},
	{-1, 2, 3, 3, 5, 8, 10, 13, 14},
	{-2, 0, 2, 2, 3, 6, 9, 12, 13},
	{-1, 1, 1, 2, 3, 6, 8, 12, 13},
	{-1, 1, 1, 3, 3, 5, 8, 11, 12},
	{-1, 1, 0, 2, 2, 5, 7, 10, 12},
	{-1, 1, 1, 2, 2, 5, 7, 10, 12},
	{0, 0, 0, 2, 2, 4, 6, 9, 11},
	{-1, 1, 1, 1, 1, 4, 6, 9, 10},
	{-1, 1, 1, 1, 1, 4, 5, 8, 10},
	{-1, 2, 0, 0, 1, 3, 5, 8, 9},
	{-1, 2, 1, 1, 1, 3, 5, 7, 9},
	{-1, 2, -1, 0, 0, 3, 5, 7, 8},
	{0, 1, 0, 0, 0, 2, 4, 6, 8},
	{-1, 2, -1, -1, 0, 2, 4, 6, 7},
	{-1, 2, -1, -1, 0, 2, 3, 5, 7},
	{0, 1, 0, 0, 0, 2, 3, 5, 6},
	{-1, 2, -1, -1, -1, 1, 2, 4, 6},
	{-1, 2, -1, -1, -1, 1, 2, 3, 4},
	{0, 0, 0, -1, -1, 1, 2, 3, 4},
	{0, 1, 0, 0, 0, 1, 2, 3, 4},
	{0, 0, -1, -1, -1, 1, 1, 3, 3},
	{0, 1, 0, -1, -1, 0, 1, 2, 3},
	{0, 1, 0, -1, 0, 0, 1, 2, 2},
	{0, 0, 0, 0, -1, 0, 1, 1, 2},
	{0, 0, -1, -1, -1, 0, 0, 1, 2},
	{0, 0, -1, -2, -1, -1, 0, 1, 2},
	{0, 0, -1, -2, -2, -1, -1, 1, 1},
	{0, 0, -1, -1, -1, -1, -1, 0, 1},
	{0, 0, -1, -2, -1, 0, -1, 0, 1},
	{0, -2, 0, -2, -2, -2, -1, 0, 1},
	{0, 0, -1, -2, -2, -1, -2, 0, 1},
	{0, 0, -1, -2, -1, -2, -1, 0, 1},
	{0, -1, -1, -2, -2, -1, -1, -1, 0},
	{0, 1, 0, -1, -1, -2, -2, -1, 1},
	{0, 1, -1, -1, -1, -1, -1, 0, 1},
	{0, 2, 0, -1, -1, -1, -1, 0, 1},
	{0, 1, 0, -1, -1, -1, -1, 0, 0},
	{0, 1, 0, -1, -1, -1, -1, 0, 0},
	{0, 0, 0, -1, -1, -2, -1, -1, 0},
	{0, 0, 0, 0, -1, -1, -1, -1, 1},
	{0, 0, 1, 1, -1, -1, -1, -1, 1},
	{0, -1, 1, 0, -1, -1, -1, 0, 1},
	{0, 0, 2, 1, 0, -1, -1, -1, 1},
	{0, 0, 2, 1, 0, 0, 0, 0, 1},
	{0, -1, 3, 1, 0, 0, 0, 1, 2},
	{0, -2, 2, 1, 0, 0, 0, 1, 3},
	{0, 0, 1, 1, 1, 0, 0, 1, 3},
	{0, 1, 2, 2, 1, 0, 1, 1, 4},
	{0, 1, 2, 2, 0, 0, 1, 1, 3},
	{0, 0, 0, 2, 0, -1, 1, 0, 2},
	{-1, 0, 0, 2, 0, -1, 0, -1, 1},
	{0, 0, 0, 1, -1, -1, 0, -1, 1},
	{0, -1, 0, 0, -1, -1, -1, -1, 0},
	{0, -2, 1, 0, -2, -1, -1, -1, -1},
	{0, -1, 1, 0, -1, -1, -2, -1, 0},
	{0, 0, 1, 0, -1, -1, -1, -2, -1},
	{0, 0, 1, -1, 0, -1, -1, -1, 0},
	{0, -1, 0, 0, -1, -1, -1, -1, 0},
	{0, 0, 0, -1, -1, -1, -1, -1, 0},
	{0, 1, 0, -1, -1, -1, -2, -1, 0},
	{0, -1, -1, -1, -1, -1, -1, -2, 0},
	{-1, 0, 0, 0, 0, -1, -1, -2, 0},
	{0, -1, -1, 0, -1, -2, -2, -2, 0},
	{0, -1, -2, -1, -2, -2, -1, -2, 0},
	{0, -1, -1, -1, -2, -1, -1, -2, 0},
	{0, -2, -1, -2, -2, -1, -2, -2, 0},
	{0, -3, -3, -3, -2, -2, -1, -2, -1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static int rgb_offset_star_revA[LUMINANCE_MAX][RGB_COMPENSATION] = {
/*	R255 G255 B255 R203 G203 B203 R151 G151 B151
	R87 G87 B87 R51 G51 B51 R35 G35 B35
	R23 G23 B23 R11 G11 B11
*/
	{-8, 1, -4, -4, -1, -3, -1, 0, 1, -20, 0, -10, -37, 1, -13, -48, 0, -14, -48, -1, -11, 2, 0, -8, 12, -1, 0},
	{-4, 1, -1, -2, 0, -2, -5, -1, -2, -11, 0, -4, -31, -1, -13, -45, 0, -15, -50, 0, -14, 0, -1, -12, 2, 1, 0},
	{-4, 1, -3, -3, -1, -1, -2, 0, -1, -7, 0, -2, -29, -1, -13, -36, 2, -11, -54, -2, -18, 2, 2, -13, 3, 0, -1},
	{-2, 1, 0, 0, 0, 0, -4, -1, -2, -6, 0, -2, -24, -1, -12, -36, 2, -12, -45, -1, -15, -6, -2, -19, 5, 0, -1},
	{0, 1, 1, -2, 0, -1, -3, 0, -2, -7, 0, -3, -15, 1, -6, -35, -1, -14, -50, -2, -20, 4, 4, -8, -1, -4, -10},
	{-2, 1, -1, -2, -1, -2, -1, 0, 0, -7, 0, -2, -15, 0, -7, -29, 1, -11, -42, -3, -19, -13, 1, -16, 5, 2, -2},
	{0, 1, 0, -2, -1, -1, -3, 0, -2, -5, 0, -2, -12, 0, -6, -27, -1, -11, -40, 1, -18, -12, 0, -16, 6, 2, -2},
	{1, 1, 1, -1, 0, 0, -4, -1, -2, -5, -1, -1, -11, 1, -5, -25, 1, -11, -36, -2, -16, -15, 1, -16, 6, 1, -3},
	{-1, 1, -1, -1, -1, -1, 0, 1, 1, -7, -1, -3, -7, 1, -4, -28, -1, -12, -27, 3, -12, -25, -5, -23, 6, 1, -4},
	{-1, 1, -1, -1, -1, 0, -1, 0, 0, -6, 0, -3, -8, 0, -4, -23, 1, -11, -31, -1, -16, -17, 0, -16, 6, 1, -3},
	{0, 1, 0, -2, -1, -1, 1, 0, 1, -5, 1, -3, -8, -1, -3, -23, 0, -12, -26, 2, -11, -18, -1, -21, -1, -3, -6},
	{-1, 1, -1, 0, -1, 0, -2, 0, -1, -4, 1, -2, -7, 0, -3, -22, -3, -13, -27, 3, -11, -18, 0, -19, -1, -3, -7},
	{2, 0, 0, -2, 0, 0, -1, 0, -1, -4, 0, -1, -7, -1, -4, -18, 2, -10, -26, 1, -12, -15, 0, -18, -1, -2, -8},
	{0, 1, -1, -1, -1, 0, -2, 0, -1, -3, 0, -2, -3, 2, 0, -20, -3, -13, -25, 3, -12, -18, -2, -19, 6, 1, -4},
	{-1, 1, -3, -2, -1, 0, -1, 0, 0, -3, 0, -1, -2, 1, -1, -18, -2, -11, -23, 4, -10, -14, -1, -19, -1, -3, -9},
	{0, 1, 0, -1, -1, -1, 0, 0, 0, -4, 1, -1, -4, -1, -2, -13, 1, -7, -29, -1, -15, -12, -2, -23, 6, 2, 0},
	{0, 1, -1, -1, -1, -1, -1, 0, 1, -4, 0, -3, -2, -1, -2, -13, 0, -6, -24, 0, -13, -11, 2, -18, -1, -2, -5},
	{0, 1, -1, -1, -1, 0, 1, 1, 1, -3, -1, -1, -2, 2, 0, -16, -4, -11, -23, 0, -13, -11, 1, -19, 6, 2, 4},
	{2, 0, 0, -1, 0, -1, -2, -1, 0, -1, 1, 1, -3, 1, -1, -8, 0, -6, -30, -2, -16, -4, 2, -15, -1, -2, -2},
	{0, 1, -2, -1, -1, 0, 1, 1, 2, -1, 0, 1, -5, -2, -3, -8, 0, -6, -24, 0, -12, -10, -1, -20, 7, 3, 6},
	{0, 1, -1, -1, -1, 0, 1, 1, 2, -1, 0, 1, -4, -2, -4, -9, -1, -4, -17, 2, -11, -9, 0, -15, -1, -2, -1},
	{2, 0, 0, -1, 0, 0, -1, 0, 0, -2, -1, -1, -3, 0, -2, -8, -1, -5, -21, -1, -11, -5, 1, -14, 8, 4, 7},
	{2, 1, -1, -2, -1, 0, 2, 1, 2, -2, 0, 0, -2, 1, 0, -6, -2, -5, -15, 3, -8, -6, -2, -13, -1, -3, -1},
	{2, 1, 1, -3, -1, -2, 2, 1, 3, -2, -1, -1, -2, 0, 1, -4, -1, -5, -19, 0, -12, 7, 4, -1, -1, -2, -1},
	{1, 0, -1, 0, 0, 1, -1, 0, 0, -2, 0, 0, -2, 0, -1, -4, -1, -4, -19, -2, -11, 7, 5, -2, -1, -2, -1},
	{2, 0, -1, -2, 0, 0, 0, 0, 0, -3, -1, -2, -1, 2, 2, -3, -1, -3, -17, 0, -11, 4, 2, -2, -1, -3, -1},
	{2, 0, -1, 0, 0, 1, 0, 0, 1, -2, 0, -1, -1, 0, 1, -7, -3, -6, -10, 5, -5, 1, -4, -6, 8, 2, 7},
	{2, 0, -1, -1, 0, 1, 0, 0, 0, -1, 0, -1, -1, 1, 1, -1, 0, 1, -16, -2, -12, 9, 1, 2, -1, -2, -1},
	{2, 0, -1, -1, 0, 0, 0, 0, 1, -2, 0, -1, -2, -2, -1, 1, 2, 2, -14, -3, -10, 7, 1, 2, 9, 3, 7},
	{2, 0, -1, -1, 0, 0, -1, 0, 0, -3, -1, -2, 2, 1, 2, -3, -1, -2, -13, -2, -8, 17, 5, 9, -1, -4, -1},
	{2, 0, -1, -1, 0, 0, 1, 0, 0, -2, -1, 1, -1, -1, 0, 0, 1, 0, -7, 1, -5, 9, -7, 5, -1, -5, -1},
	{2, 0, -1, -1, 0, 0, 1, 0, 2, 0, 0, 1, -2, -2, -2, 5, 4, 5, -15, -4, -13, 11, -10, 8, -1, -5, -1},
	{2, 0, -1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 2, 0, 3, 2, 0, 2, -7, 1, -5, 3, -22, -1, -1, 28, -1},
	{2, 0, -1, 0, 0, 1, 0, 0, 1, -2, -1, 0, -1, -1, -2, 6, 3, 6, -5, 0, -6, 1, 16, 1, -1, -5, -1},
	{1, 0, -2, 0, 0, 1, 1, 0, 1, -1, 0, 1, -1, -2, -2, 1, -1, 1, -2, 2, -2, 2, 15, 1, -1, -8, -1},
	{-2, 0, -3, 2, 1, 2, -1, -1, -1, 0, 0, 0, 3, 1, 5, 5, 1, 2, -9, -3, -8, 2, 11, 1, -1, -8, -1},
	{-1, 0, -3, 1, 1, 1, 0, -1, 0, 0, -1, 0, 2, 2, 2, 0, -2, 0, 0, 2, 0, -2, 12, -3, -1, -6, -1},
	{1, 0, -3, 0, 0, 1, -1, 0, -1, 1, 0, 2, 0, 0, 2, 5, 1, 2, -4, 0, -2, -2, 14, -2, -1, -9, -1},
	{0, 0, -5, 0, 0, 1, 0, 0, 1, 0, 0, 1, 2, 2, 4, 0, -1, -1, -1, -1, -2, -1, 15, -1, -1, -7, -1},
	{1, 1, -2, 0, 0, 0, -1, 0, -1, 0, 1, 2, 0, -2, 2, 8, 4, 4, -2, 0, 0, -5, 13, -6, -1, -8, -1},
	{0, 0, -2, 0, 0, 1, -1, 0, -1, -1, -1, -1, 0, 0, 1, 7, 3, 5, -5, -3, -4, 0, -18, -2, -1, 24, -1},
	{1, 1, 0, -1, -1, 0, 1, 1, 0, -2, -1, 0, 4, 1, 3, -3, -3, -2, 4, 4, 4, -4, -21, -5, -1, 23, -1},
	{1, 0, -1, -1, 0, 1, 0, 0, 1, -1, -1, 0, 2, 1, 3, 4, 1, 0, -5, -4, -3, 1, -13, 0, -1, 21, -1},
	{1, 1, -2, -1, 0, 1, 1, 0, 0, -1, -1, 0, -1, -1, 1, 2, 1, 2, 3, 0, 1, -3, -13, -3, -1, 22, -1},
	{1, 1, -1, 0, 0, -1, 0, 0, 1, -1, 0, 1, 1, 0, 1, 2, 0, 1, -1, -2, -2, 0, -8, -1, -1, 20, -1},
	{0, 0, -1, 0, 0, -1, 0, 0, 2, -1, -1, 0, 1, -1, 2, 2, 1, 0, 2, 2, 2, -3, -6, -3, -1, -9, -1},
	{1, 0, -2, 0, 0, 0, 0, 0, 1, -1, 0, -1, 0, 1, 1, 1, 1, 1, 0, -2, -1, -1, 2, -1, -1, -12, -1},
	{0, 1, -2, 1, 0, 1, -1, 1, 0, 1, 0, 1, -1, 0, 1, 0, 0, 0, 5, 3, 4, -13, -3, -11, 9, -3, 6},
	{0, 1, -1, 0, 0, -1, 0, 0, 0, 0, 0, 1, -2, 1, 0, 0, 0, 0, 4, 1, 1, -12, 1, -5, 9, -2, 4},
	{0, 1, -1, -1, 0, -3, 0, 0, 1, 0, 0, 0, -2, -1, 0, 2, 2, 1, 0, 0, -1, -15, -1, -6, 8, -1, 0},
	{1, 1, 2, 0, 0, -1, 0, 0, 0, 0, 0, 1, -2, -1, -1, 2, 1, 1, 0, -3, -2, -20, 0, -11, 13, 2, 5},
	{0, 1, 1, 0, 0, -1, 0, 0, 0, -1, 1, 0, 0, 0, 1, 1, 0, 0, 2, 1, 1, -17, -1, -10, -2, -2, -6},
	{-1, 1, -1, -1, 0, -1, 1, 0, 0, 0, -1, 0, -2, 0, -1, 3, 2, 2, 0, 0, -1, -22, -3, -13, 4, 2, -2},
	{-2, 0, 0, 0, 0, -1, -1, 0, 0, 1, 0, 0, 0, -1, 1, -1, 2, 0, -2, -2, -3, -11, 3, -7, -11, -2, -12},
	{-1, 0, -1, -1, 0, -1, -1, 0, 0, 2, 0, 1, 0, 0, 1, -2, -2, -2, 0, 1, -1, -15, -1, -8, -1, 1, -4},
	{0, 1, 2, 0, 0, -1, 0, 0, 0, -2, -1, -1, 1, 1, 2, 2, 0, 0, -3, -2, -3, -9, 2, -4, 2, 1, -2},
	{0, 1, -1, 0, 0, -1, 0, 0, 1, -1, -1, -2, 0, 0, 2, 1, 0, 0, 0, -1, -1, -3, 3, 0, -6, -3, -7},
	{0, 1, 1, 0, 0, -1, 0, 0, 0, -1, 0, 1, 0, 1, 0, 0, 0, 1, 1, -3, -2, -5, 3, -1, -5, -2, -6},
	{0, 1, 0, -2, -1, -2, 1, 0, 2, 0, 1, -1, -1, -1, 0, 2, 0, 1, 6, 3, 4, -11, -4, -7, 6, 1, 2},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 1, 1, 2, -1, -2, 1, 4, 3, 3, -6, -1, -3, 3, -1, -1},
	{1, 1, 1, 0, 0, -1, 0, 0, -1, 0, 0, 1, 0, 0, 1, 2, 1, 2, 5, 1, 3, -6, -1, -3, 1, -1, -2},
	{-1, 0, -1, 0, 0, 0, 0, 0, -1, 0, -1, 1, 0, 0, 1, 1, 1, 2, 1, -1, -1, 0, 3, 2, -1, -4, -3},
	{0, 1, 1, 0, 0, -1, 0, 1, -1, 1, 0, 0, -2, -1, 0, 0, 0, 3, 6, 2, 4, -10, -4, -6, 5, 3, 2},
	{0, 1, 1, 0, 0, 0, 0, 0, -1, -1, -1, 0, 0, 0, 0, -1, -1, 0, 2, 0, 2, -4, -1, -2, 4, 2, 0},
	{1, 1, 1, 0, 0, 0, 0, 0, -1, 0, 0, 0, -2, 0, -1, 1, 0, 2, 2, -1, 1, -4, -1, -2, 2, -2, 0},
	{0, 0, 1, -1, 0, -1, -1, 0, -1, 2, 0, 1, 1, 1, 2, -3, -2, -2, 7, 4, 5, -5, -3, -2, 1, 0, -2},
	{0, 0, 0, 0, 0, 0, 0, 0, -1, 1, 0, 1, -1, 0, 0, -1, -1, 0, 1, 0, 0, -2, 1, 2, 2, 1, -2},
	{0, 1, 1, 0, 0, 0, -1, 0, -2, 1, 0, 1, -2, -1, 1, 0, -1, 0, 2, 0, 1, 1, 2, 2, -1, 0, -3},
	{0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, 0, 0, 1, 1, 1, 2, 3, 1, 2, -3, -3, -2, 0, -1, -2},
	{-1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 1, 1, 1, 1, 0, 0, 1, -2, -3, -3, 1, 2, 1, 0, 0, 0},
	{-1, 0, -1, 0, 0, 0, -1, 0, -1, 1, 0, 1, 1, 1, 1, -2, -2, -1, 0, -1, -1, 5, 3, 3, -4, -5, -2},
	{-4, 1, -3, 1, 0, 1, 0, 0, -1, 1, 0, -1, 1, 1, 3, -2, -1, -1, 4, 1, 2, 2, 0, -1, -3, 0, -1},
	{0, 0, 0, 1, 0, 1, -1, 0, -1, 1, 1, 1, 0, 0, 0, 1, 1, 2, 0, -2, -1, 3, 3, 2, -4, -2, -3},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

/* Bloom has no hmt data? */
/************
 *	  HMT   *
 ************/
static unsigned int base_luminance_reverse_hmt_single[][2] = {
	{10, 46},
	{11, 50},
	{12, 55},
	{13, 59},
	{14, 64},
	{15, 67},
	{16, 71},
	{17, 77},
	{19, 85},
	{20, 89},
	{21, 93},
	{22, 98},
	{23, 100},
	{25, 104},
	{27, 112},
	{29, 119},
	{31, 126},
	{33, 134},
	{35, 141},
	{37, 148},
	{39, 155},
	{41, 164},
	{44, 175},
	{47, 185},
	{50, 195},
	{53, 204},
	{56, 214},
	{60, 228},
	{64, 242},
	{68, 255},
	{72, 266},
	{77, 208},
	{82, 219},
	{87, 231},
	{93, 247},
	{99, 259},
	{105, 274},
};

static int gradation_offset_reverse_hmt_single[][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{0, -1, -1, 2, 5, 6, 7, 9, 8},
	{0, -2, -1, 2, 5, 6, 7, 9, 8},
	{0, -1, 0, 2, 5, 5, 7, 9, 8},
	{0, -1, -1, 1, 4, 5, 7, 9, 8},
	{0, -1, 0, 1, 5, 5, 7, 9, 8},
	{0, 0, 1, 2, 4, 5, 6, 9, 8},
	{0, 0, 1, 1, 4, 5, 6, 9, 8},
	{0, 1, 0, 2, 4, 5, 7, 9, 9},
	{0, 0, -1, 1, 3, 4, 5, 8, 8},
	{0, 1, 0, 1, 4, 5, 6, 8, 8},
	{0, 1, 0, 1, 4, 5, 6, 9, 8},
	{0, 2, -1, 0, 3, 4, 5, 9, 8},
	{0, 2, 0, 1, 4, 5, 5, 8, 8},
	{0, 2, 1, 2, 3, 5, 6, 9, 8},
	{0, 1, 1, 1, 3, 4, 5, 8, 8},
	{0, 2, 3, 3, 4, 5, 5, 8, 9},
	{0, 4, 5, 3, 4, 5, 6, 9, 9},
	{0, 3, 3, 2, 4, 6, 5, 9, 8},
	{0, 3, 4, 2, 4, 5, 5, 9, 8},
	{0, 3, 5, 2, 3, 6, 6, 8, 8},
	{0, 3, 6, 3, 4, 5, 5, 8, 8},
	{0, 3, 4, 3, 4, 5, 5, 8, 8},
	{0, 3, 5, 3, 4, 5, 5, 8, 9},
	{0, 3, 6, 3, 4, 5, 5, 8, 9},
	{0, 2, 6, 4, 4, 5, 5, 9, 9},
	{0, 2, 7, 4, 4, 5, 5, 7, 8},
	{0, 4, 7, 4, 3, 4, 5, 7, 8},
	{0, 4, 7, 4, 4, 5, 5, 8, 8},
	{0, 3, 8, 4, 4, 5, 5, 8, 8},
	{0, 4, 8, 5, 4, 4, 5, 7, 9},
	{0, 5, 9, 5, 4, 5, 5, 7, 9},
	{0, 3, 6, 3, 2, 3, 3, 5, 5},
	{0, 3, 6, 3, 2, 3, 3, 5, 5},
	{0, 2, 5, 4, 2, 3, 2, 5, 5},
	{0, 3, 6, 4, 2, 3, 3, 5, 5},
	{0, 3, 5, 4, 2, 3, 2, 4, 5},
	{0, 7, 7, 5, 3, 3, 3, 5, 5},
};

static int rgb_offset_reverse_hmt_single[][RGB_COMPENSATION] = {
/*	R255 G255 B255 R203 G203 B203 R151 G151 B151
	R87 G87 B87 R51 G51 B51 R35 G35 B35
	R23 G23 B23 R11 G11 B11
*/
	{0, 0, 1, 3, -1, 2, 0, 0, 1, -3, 1, -3, -6, 2, -7, -5, 3, -6, -4, 0, -5, -8, 2, -7, 4, 1, -8},
	{0, 0, 1, 5, 0, 3, 0, 0, 1, -3, 1, -2, -4, 2, -5, -5, 3, -6, -4, 0, -5, -8, 2, -8, 4, 1, -8},
	{0, 0, 1, 3, 0, 1, 0, 0, 1, -3, 1, -2, -4, 2, -5, -5, 3, -6, -4, 0, -5, -8, 2, -8, 4, 1, -8},
	{2, 0, 4, 0, 0, -1, 3, 0, 2, -3, 1, -3, -4, 2, -6, -5, 3, -6, -4, 0, -5, -8, 2, -8, 4, 1, -8},
	{2, 0, 4, 0, 0, -1, 2, 0, 2, -2, 1, 0, -4, 2, -6, -5, 3, -6, -4, 0, -5, -8, 0, -8, 4, 3, -8},
	{2, 0, 3, 0, 0, -1, 2, 0, 1, -2, 0, 0, -4, 2, -6, -5, 3, -6, -4, 0, -5, -8, 2, -8, 3, 3, -8},
	{2, 0, 3, -1, 0, -1, 2, 0, 2, -2, 0, 0, -4, 2, -6, -6, 3, -6, -4, -1, -5, -8, 1, -8, 3, 4, -8},
	{3, -1, 4, -1, 0, -1, 3, 0, 2, -2, 0, 0, -4, 2, -6, -6, 3, -6, -4, -1, -5, -8, 0, -8, 2, 3, -6},
	{4, -1, 5, 0, 0, -1, 2, 0, 2, -2, 0, 0, -4, 2, -6, -6, 3, -6, -4, 2, -5, -8, 0, -8, 2, 3, -6},
	{3, -1, 5, 0, 0, -1, 2, 0, 2, -2, 0, 0, -4, 2, -6, -6, 3, -6, -4, 2, -5, -8, 0, -8, 2, 3, -6},
	{5, -1, 6, -1, 0, -1, 2, 0, 1, -2, 0, 0, -4, 2, -6, -6, 3, -6, -4, 2, -5, -8, 0, -8, 2, 4, -6},
	{5, -1, 6, -1, 0, -1, 3, 0, 2, -2, 1, 0, -5, 2, -6, -6, 2, -6, -4, 2, -5, -6, 0, -8, 1, 4, -5},
	{6, -1, 7, -1, 0, -1, 0, 0, 0, 0, 0, 0, -5, 2, -5, -3, 2, -4, -5, 2, -6, -6, 0, -9, 1, 3, -5},
	{6, -1, 6, -1, 0, -1, -3, 0, -2, 0, -1, 1, -4, 2, -5, -4, 2, -5, -5, 2, -6, -6, 0, -8, 0, 4, -7},
	{7, -1, 7, -1, 0, -1, -2, 0, -1, 0, -1, 0, -4, 2, -4, -4, 3, -5, -4, 2, -4, -5, 1, -8, 0, 3, -6},
	{7, -1, 8, -1, 0, -1, -3, 0, -3, 1, -1, 2, -3, 1, -4, -5, 2, -6, -5, 1, -5, -5, 1, -8, -1, 3, -4},
	{7, -2, 8, -1, 0, -1, -4, 0, -3, 2, 0, 2, -3, 2, -4, -5, 2, -6, -5, 1, -6, -3, 0, -8, -3, 4, -6},
	{8, -2, 8, -1, 0, -1, -2, 0, -1, 0, 0, 0, -4, 0, -4, -5, 2, -6, -5, 2, -6, -8, -1, -10, 1, 5, -7},
	{8, -2, 9, -1, 0, -1, -3, 0, -2, 0, 0, 1, -2, 1, -2, -6, 2, -6, -5, 1, -6, -7, -1, -9, 1, 4, -6},
	{9, -2, 9, 0, 0, -1, -2, 0, -2, 0, 0, 1, -3, 1, -3, -5, 1, -5, -5, 2, -6, -7, 1, -10, 0, 3, -5},
	{9, -2, 9, -1, 0, -1, -2, 0, -2, 0, 0, 1, -2, 1, -2, -4, 2, -5, -6, 2, -6, -7, 0, -10, 0, 3, -6},
	{10, -2, 9, 0, 0, -1, -3, 0, -3, 0, 0, 2, -1, 1, -2, -7, 2, -5, -5, 2, -6, -6, -1, -10, 0, 4, -5},
	{10, -2, 10, 0, 0, 0, -2, 0, -2, 0, 0, 0, -1, 1, -1, -6, 2, -4, -4, 2, -5, -8, 0, -11, 0, 2, -3},
	{11, -2, 12, -1, 0, -1, -3, 0, -3, 0, 0, 1, -1, 1, -2, -5, 2, -5, -5, 2, -5, -8, 0, -10, 0, 3, -3},
	{10, -2, 10, 0, 0, -1, -2, 0, -2, 1, 0, 1, -2, 1, -3, -6, 2, -6, -5, 1, -5, -6, 0, -7, 0, 4, -5},
	{10, -2, 11, 0, 0, 0, -1, 0, -1, 0, 0, 0, -2, 1, -4, -5, 2, -5, -3, 2, -4, -9, 1, -10, 1, 4, -3},
	{10, -2, 11, 0, 0, -1, -1, 0, -1, 0, 0, 1, -1, 1, -2, -6, 2, -6, -4, 2, -4, -9, 1, -10, 2, 3, -2},
	{11, -2, 11, 0, 0, -1, -1, 0, -1, 0, 0, 0, -1, 1, -1, -4, 2, -4, -5, 3, -5, -7, 1, -9, 1, 3, -5},
	{12, -2, 12, 0, 0, -1, -2, 0, -1, 0, -1, 0, -2, 0, -3, -5, 1, -4, -5, 2, -6, -8, 1, -10, 1, 4, -5},
	{12, -2, 12, 0, 0, -1, -2, 0, -1, 0, 0, 1, 0, 1, -1, -5, 1, -4, -5, 2, -6, -8, 3, -10, 0, 1, -3},
	{13, -3, 12, 0, 0, -1, -2, 0, -1, 0, 0, 0, -1, 0, -2, -5, 1, -4, -5, 2, -5, -7, 2, -10, 0, 1, -5},
	{8, -2, 9, 0, 0, -1, -1, 0, -1, 0, -1, 0, -1, 0, -1, -4, 0, -4, -4, 1, -3, -5, 0, -7, -1, 5, -6},
	{7, -2, 9, 0, 0, 0, -1, 0, 0, -1, 2, 0, 0, -2, -2, -3, 0, -3, -5, 0, -5, -6, -1, -8, -3, 3, -6},
	{8, -2, 8, 0, 0, 0, 0, 0, 0, -4, 0, -2, 0, -1, 0, -2, 1, -2, -5, 0, -4, -4, 1, -7, -3, 2, -6},
	{8, -2, 8, 0, 0, 0, 0, 1, 0, -3, 0, -2, -1, -2, 0, -1, 1, -3, -5, 1, -5, -6, 0, -7, -2, 3, -5},
	{8, -2, 9, 0, 0, -1, 0, 0, -1, -3, 0, -2, 0, -2, 0, -2, 0, -2, -5, 3, -4, -6, 0, -8, -2, 1, -7},
	{9, -2, 9, 0, 0, 0, 0, 1, 0, -2, 1, -2, 0, -2, 0, -1, 1, -2, -5, 2, -4, -2, 1, -4, 0, 5, -3},
};
#endif
