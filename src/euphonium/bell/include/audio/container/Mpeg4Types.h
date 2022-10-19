// Copyright (c) Kuba Szczodrzyński 2022-1-12.

#pragma once

enum class AtomType {
	/** File Type */
	ATOM_FTYP = 0x66747970,
	/** Movie */
	ATOM_MOOV = 0x6D6F6F76,
	/** Movie Header */
	ATOM_MVHD = 0x6D766864,
	/** Movie Extends */
	ATOM_MVEX = 0x6D766578,
	/** Movie Extends Header */
	ATOM_MEHD = 0x6D656864,
	/** Track Extends */
	ATOM_TREX = 0x74726578,
	/** Track */
	ATOM_TRAK = 0x7472616B,
	/** Track Header */
	ATOM_TKHD = 0x746B6864,
	/** Edit */
	ATOM_EDTS = 0x65647473,
	/** Edit List */
	ATOM_ELST = 0x656C7374,
	/** Media */
	ATOM_MDIA = 0x6D646961,
	/** Media Header */
	ATOM_MDHD = 0x6D646864,
	/** Handler Reference */
	ATOM_HDLR = 0x68646C72,
	/** Handler Type - Sound */
	ATOM_SOUN = 0x736F756E,
	/** Handler Type - Video */
	ATOM_VIDE = 0x76696465,
	/** Handler Type - Subtitle */
	ATOM_SUBT = 0x73756274,
	/** Media Information */
	ATOM_MINF = 0x6D696E66,
	/** Data Information */
	ATOM_DINF = 0x64696E66,
	/** Data Reference */
	ATOM_DREF = 0x64726566,
	/** Data Entry Url */
	ATOM_URL = 0x75726C20,
	/** Sample Table */
	ATOM_STBL = 0x7374626C,
	/** Sample Description */
	ATOM_STSD = 0x73747364,
	/** siDecompressionParam */
	ATOM_WAVE = 0x77617665,
	/** Format Atom */
	ATOM_FRMA = 0x66726D61,
	/** Audio Channel Layout Atom */
	ATOM_CHAN = 0x6368616E,
	/** Terminator Atom */
	ATOM_TERM = 0x00000000,
	/** MPEG-4 Elementary Stream Descriptor */
	ATOM_ESDS = 0x65736473,
	/** Time-to-sample Table */
	ATOM_STTS = 0x73747473,
	/** Sync Sample Table */
	ATOM_STSS = 0x73747373,
	/** Sample-to-chunk Table */
	ATOM_STSC = 0x73747363,
	/** Chunk Offset Table */
	ATOM_STCO = 0x7374636F,
	/** Sample Size Table */
	ATOM_STSZ = 0x7374737A,
	/** Sound Media Header */
	ATOM_SMHD = 0x736D6864,
	/** Segment Index Table */
	ATOM_SIDX = 0x73696478,
	/** Movie Fragment */
	ATOM_MOOF = 0x6D6F6F66,
	/** Movie Fragment Header */
	ATOM_MFHD = 0x6D666864,
	/** Track Fragment */
	ATOM_TRAF = 0x74726166,
	/** Track Fragment Header */
	ATOM_TFHD = 0x74666864,
	/** Track Fragment Run */
	ATOM_TRUN = 0x7472756E,
	/** Media Data */
	ATOM_MDAT = 0x6D646174,
};

// These formats are the direct sub-children of the stsd atom.
// https://mp4ra.org/#/codecs (+additions)
enum class AudioSampleFormat {
	UNDEFINED = 0,
	A3DS = 0x61336473,		// Auro-Cx 3D audio
	AC3 = 0x61632d33,		// AC-3 audio
	AC4 = 0x61632d34,		// AC-4 audio
	AGSM = 0x6167736d,		// GSM
	ALAC = 0x616c6163,		// Apple lossless audio codec
	ALAW = 0x616c6177,		// a-Law
	CAVS = 0x63617673,		// AVS2-P3 codec
	DRA1 = 0x64726131,		// DRA Audio
	DTS_MINUS = 0x6474732d, // Dependent base layer for DTS layered audio
	DTS_PLUS = 0x6474732b,	// Enhancement layer for DTS layered audio
	DTSC = 0x64747363,		// Core Substream
	DTSE = 0x64747365,		// Extension Substream containing only LBR
	DTSH = 0x64747368,		// Core Substream + Extension Substream
	DTSL = 0x6474736c,		// Extension Substream containing only XLL
	DTSX = 0x64747378,		// DTS-UHD profile 2
	DTSY = 0x64747379,		// DTS-UHD profile 3 or higher
	DVI = 0x64766920,		// DVI (as used in RTP, 4:1 compression)
	EC3 = 0x65632d33,		// Enhanced AC-3 audio
	ENCA = 0x656e6361,		// Encrypted/Protected audio
	FL32 = 0x666c3332,		// 32 bit float
	FL64 = 0x666c3634,		// 64 bit float
	FLAC = 0x664c6143,		// Free Lossless Audio Codec
	G719 = 0x67373139,		// ITU-T Recommendation G.719 (2008)
	G726 = 0x67373236,		// ITU-T Recommendation G.726 (1990)
	IMA4 = 0x696d6134,		// IMA (International Multimedia Assocation, defunct, 4:1)
	IN24 = 0x696e3234,		// 24 bit integer uncompressed
	IN32 = 0x696e3332,		// 32 bit integer uncompressed
	LPCM = 0x6c70636d,		// Uncompressed audio (various integer and float formats)
	M4AE = 0x6d346165,		// MPEG-4 Audio Enhancement
	MHA1 = 0x6d686131,		// MPEG-H Audio (single stream, unencapsulated)
	MHA2 = 0x6d686132,		// MPEG-H Audio (multi-stream, unencapsulated)
	MHM1 = 0x6d686d31,		// MPEG-H Audio (single stream, MHAS encapsulated)
	MHM2 = 0x6d686d32,		// MPEG-H Audio (multi-stream, MHAS encapsulated)
	MLPA = 0x6d6c7061,		// MLP Audio
	MP4A = 0x6d703461,		// MPEG-4 Audio
	OPUS = 0x4f707573,		// Opus audio coding
	QCLP = 0x51636c70,		// Qualcomm PureVoice
	QDM2 = 0x51444d32,		// Qdesign music 2
	QDMC = 0x51444d43,		// Qdesign music 1
	RAW = 0x72617720,		// Uncompressed audio
	SAMR = 0x73616d72,		// Narrowband AMR voice
	SAWB = 0x73617762,		// Wideband AMR voice
	SAWP = 0x73617770,		// Extended AMR-WB (AMR-WB+)
	SEVC = 0x73657663,		// EVRC Voice
	SEVS = 0x73657673,		// Enhanced Voice Services (EVS)
	SQCP = 0x73716370,		// 13K Voice
	SSMV = 0x73736d76,		// SMV Voice
	TWOS = 0x74776f73,		// Uncompressed 16-bit audio
	ULAW = 0x756c6177,		// Samples have been compressed using uLaw 2:1.
	VDVA = 0x76647661,		// DV audio (variable duration per video frame)
};

// These are present in the DecoderConfigDescriptor tag in ESDS (for AudioSampleFormat::FORMAT_MP4A).
// Source: https://mp4ra.org/#/codecs
enum class MP4AObjectType {
	UNDEFINED = 0,
	_13K = 0xE1,		 // 13K Voice
	AAC_LC = 0x67,		 // ISO/IEC 13818-7 (AAC) Low Complexity Profile
	AAC_MAIN = 0x66,	 // ISO/IEC 13818-7 (AAC) Main Profile
	AAC_SSR = 0x68,		 // ISO/IEC 13818-7 (AAC) Scaleable Sampling Rate Profile
	AC3 = 0xA5,			 // AC-3
	AC3_ENH = 0xA6,		 // Enhanced AC-3
	AC4 = 0xAE,			 // AC-4
	AURO_CX_3D = 0xAF,	 // Auro-Cx 3D audio
	DRA = 0xA7,			 // DRA Audio
	DTS_CORE = 0xA9,	 // Core Substream
	DTS_CORE_EXT = 0xAA, // Core Substream + Extension Substream
	DTS_LBR = 0xAC,		 // Extension Substream containing only LBR
	DTS_UHD2 = 0xB2,	 // DTS-UHD profile 2
	DTS_UHD3 = 0xB3,	 // DTS-UHD profile 3 or higher
	DTS_XLL = 0xAB,		 // Extension Substream containing only XLL
	EVRC = 0xA0,		 // EVRC Voice
	G719 = 0xA8,		 // ITU G.719 Audio
	MP4A = 0x40,		 // ISO/IEC 14496-3 (MPEG-4 Audio)
	MPEG1 = 0x6B,		 // ISO/IEC 11172-3 (MPEG-1 Part 3)
	MPEG2 = 0x69,		 // ISO/IEC 13818-3 (MPEG-2 Part 3)
	OPUS = 0xAD,		 // Opus audio
	SMV = 0xA1,			 // SMV Voice
	VORBIS = 0xDD,		 // Vorbis
};

// These are present in the DecoderSpecificInfo tag in ESDS (for MP4AObjectType::TYPE_MP4A).
// Source: https://wiki.multimedia.cx/index.php/MPEG-4_Audio
enum class MP4AProfile {
	UNDEFINED = 0,
	AAC_MAIN = 1,							 // AAC main
	AAC_LC = 2,								 // AAC LC
	AAC_SSR = 3,							 // AAC SSR
	AAC_LTP = 4,							 // AAC LTP
	SBR = 5,								 // SBR
	AAC_SCALABLE = 6,						 // AAC Scalable
	TWINVQ = 7,								 // TwinVQ
	CELP = 8,								 // CELP
	HVXC = 9,								 // HVXC
	TTSI = 12,								 // TTSI
	MAIN_SYNTHETIC = 13,					 // Main synthetic
	WAVETABLE_SYNTHESIS = 14,				 // Wavetable synthesis
	GENERAL_MIDI = 15,						 // General MIDI
	ALGORITHMIC_SYNTHESIS_AND_AUDIO_FX = 16, // Algorithmic Synthesis and Audio FX
	ER_AAC_LC = 17,							 // ER AAC LC
	ER_AAC_LTP = 19,						 // ER AAC LTP
	ER_AAC_SCALABLE = 20,					 // ER AAC Scalable
	ER_TWINVQ = 21,							 // ER TwinVQ
	ER_BSAC = 22,							 // ER BSAC
	ER_AAC_LD = 23,							 // ER AAC LD
	ER_CELP = 24,							 // ER CELP
	ER_HVXC = 25,							 // ER HVXC
	ER_HILN = 26,							 // ER HILN
	ER_PARAMETRIC = 27,						 // ER Parametric
	SSC = 28,								 // SSC
	LAYER_1 = 32,							 // Layer-1
	LAYER_2 = 33,							 // Layer-2
	LAYER_3 = 34,							 // Layer-3
	DST = 35,								 // DST
};
