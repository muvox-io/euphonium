// Copyright (c) Kuba Szczodrzyński 2022-1-16.

#pragma once

enum class ElementId {
	/** [sub-elements] Set the EBML characteristics of the data to follow. Each EBML document has to start with this. */
	EBML = 0x1A45DFA3,
	/** [u-integer] The version of EBML parser used to create the file. */
	EBMLVersion = 0x4286,
	/** [u-integer] The minimum EBML version a parser has to support to read this file. */
	EBMLReadVersion = 0x42F7,
	/** [u-integer] The maximum length of the IDs you'll find in this file (4 or less in Matroska). */
	EBMLMaxIDLength = 0x42F2,
	/** [u-integer] The maximum length of the sizes you'll find in this file (8 or less in Matroska). This does not
	   override the element size indicated at the beginning of an element. Elements that have an indicated size which is
	   larger than what is allowed by EBMLMaxSizeLength shall be considered invalid. */
	EBMLMaxSizeLength = 0x42F3,
	/** [string] A string that describes the type of document that follows this EBML header ('matroska' in our case). */
	DocType = 0x4282,
	/** [u-integer] The version of DocType interpreter used to create the file. */
	DocTypeVersion = 0x4287,
	/** [u-integer] The minimum DocType version an interpreter has to support to read this file. */
	DocTypeReadVersion = 0x4285,
	/** [binary] The CRC is computed on all the data from the last CRC element (or start of the upper level element), up
	   to the CRC element, including other previous CRC elements. All level 1 elements should include a CRC-32. */
	CRC32 = 0xBF,
	/** [binary] Used to void damaged data, to avoid unexpected behaviors when using damaged data. The content is
	   discarded. Also used to reserve space in a sub-element for later use. */
	Void = 0xEC,
	/** [sub-elements] Contain signature of some (coming) elements in the stream. */
	SignatureSlot = 0x1B538667,
	/** [u-integer] Signature algorithm used (1=RSA, 2=elliptic). */
	SignatureAlgo = 0x7E8A,
	/** [u-integer] Hash algorithm used (1=SHA1-160, 2=MD5). */
	SignatureHash = 0x7E9A,
	/** [binary] The public key to use with the algorithm (in the case of a PKI-based signature). */
	SignaturePublicKey = 0x7EA5,
	/** [binary] The signature of the data (until a new. */
	Signature = 0x7EB5,
	/** [sub-elements] Contains elements that will be used to compute the signature. */
	SignatureElements = 0x7E5B,
	/** [sub-elements] A list consists of a number of consecutive elements that represent one case where data is used in
	   signature. Ex: Cluster|Block|BlockAdditional means that the BlockAdditional of all Blocks in all Clusters is used
	   for encryption. */
	SignatureElementList = 0x7E7B,
	/** [binary] An element ID whose data will be used to compute the signature. */
	SignedElement = 0x6532,

	/* ebml_matroska.xml */
	/** [master] The Root Element that contains all other Top-Level Elements (Elements defined only at Level 1). A
	   Matroska file is composed of 1 Segment. */
	Segment = 0x18538067,
	/** [master] Contains the Segment Position of other Top-Level Elements. */
	SeekHead = 0x114D9B74,
	/** [master] Contains a single seek entry to an EBML Element. */
	Seek = 0x4DBB,
	/** [binary] The binary ID corresponding to the Element name. */
	SeekID = 0x53AB,
	/** [uinteger] The Segment Position of the Element. */
	SeekPosition = 0x53AC,
	/** [master] Contains general information about the Segment. */
	Info = 0x1549A966,
	/** [binary] A randomly generated unique ID to identify the Segment amongst many others (128 bits). */
	SegmentUID = 0x73A4,
	/** [utf-8] A filename corresponding to this Segment. */
	SegmentFilename = 0x7384,
	/** [binary] A unique ID to identify the previous Segment of a Linked Segment (128 bits). */
	PrevUID = 0x3CB923,
	/** [utf-8] A filename corresponding to the file of the previous Linked Segment. */
	PrevFilename = 0x3C83AB,
	/** [binary] A unique ID to identify the next Segment of a Linked Segment (128 bits). */
	NextUID = 0x3EB923,
	/** [utf-8] A filename corresponding to the file of the next Linked Segment. */
	NextFilename = 0x3E83BB,
	/** [binary] A randomly generated unique ID that all Segments of a Linked Segment **MUST** share (128 bits). */
	SegmentFamily = 0x4444,
	/** [master] The mapping between this `Segment` and a segment value in the given Chapter Codec. */
	ChapterTranslate = 0x6924,
	/** [binary] The binary value used to represent this Segment in the chapter codec data. The format depends on the
	   ChapProcessCodecID used; see (#chapprocesscodecid-element). */
	ChapterTranslateID = 0x69A5,
	/** [uinteger] This `ChapterTranslate` applies to this chapter codec of the given chapter edition(s); see
	   (#chapprocesscodecid-element). */
	ChapterTranslateCodec = 0x69BF,
	/** [uinteger] Specify a chapter edition UID on which this `ChapterTranslate` applies. */
	ChapterTranslateEditionUID = 0x69FC,
	/** [uinteger] Timestamp scale in nanoseconds (1.000.000 means all timestamps in the Segment are expressed in
	   milliseconds). */
	TimestampScale = 0x2AD7B1,
	/** [float] Duration of the Segment in nanoseconds based on TimestampScale. */
	Duration = 0x4489,
	/** [date] The date and time that the Segment was created by the muxing application or library. */
	DateUTC = 0x4461,
	/** [utf-8] General name of the Segment. */
	Title = 0x7BA9,
	/** [utf-8] Muxing application or library (example: "libmatroska-0.4.3"). */
	MuxingApp = 0x4D80,
	/** [utf-8] Writing application (example: "mkvmerge-0.3.3"). */
	WritingApp = 0x5741,
	/** [master] The Top-Level Element containing the (monolithic) Block structure. */
	Cluster = 0x1F43B675,
	/** [uinteger] Absolute timestamp of the cluster (based on TimestampScale). */
	Timestamp = 0xE7,
	/** [master] The list of tracks that are not used in that part of the stream. It is useful when using overlay tracks
	   on seeking or to decide what track to use. */
	SilentTracks = 0x5854,
	/** [uinteger] One of the track number that are not used from now on in the stream. It could change later if not
	   specified as silent in a further Cluster. */
	SilentTrackNumber = 0x58D7,
	/** [uinteger] The Segment Position of the Cluster in the Segment (0 in live streams). It might help to
	   resynchronise offset on damaged streams. */
	Position = 0xA7,
	/** [uinteger] Size of the previous Cluster, in octets. Can be useful for backward playing. */
	PrevSize = 0xAB,
	/** [binary] Similar to Block, see (#block-structure), but without all the extra information, mostly used to reduced
	   overhead when no extra feature is needed; see (#simpleblock-structure) on SimpleBlock Structure. */
	SimpleBlock = 0xA3,
	/** [master] Basic container of information containing a single Block and information specific to that Block. */
	BlockGroup = 0xA0,
	/** [binary] Block containing the actual data to be rendered and a timestamp relative to the Cluster Timestamp; see
	   (#block-structure) on Block Structure. */
	Block = 0xA1,
	/** [binary] A Block with no data. It **MUST** be stored in the stream at the place the real Block would be in
	   display order. */
	BlockVirtual = 0xA2,
	/** [master] Contain additional blocks to complete the main one. An EBML parser that has no knowledge of the Block
	   structure could still see and use/skip these data. */
	BlockAdditions = 0x75A1,
	/** [master] Contain the BlockAdditional and some parameters. */
	BlockMore = 0xA6,
	/** [uinteger] An ID to identify the BlockAdditional level. If BlockAddIDType of the corresponding block is 0, this
	   value is also the value of BlockAddIDType for the meaning of the content of BlockAdditional. */
	BlockAddID = 0xEE,
	/** [binary] Interpreted by the codec as it wishes (using the BlockAddID). */
	BlockAdditional = 0xA5,
	/** [uinteger] The duration of the Block (based on TimestampScale). The BlockDuration Element can be useful at the
	   end of a Track to define the duration of the last frame (as there is no subsequent Block available), or when
	   there is a break in a track like for subtitle tracks. */
	BlockDuration = 0x9B,
	/** [uinteger] This frame is referenced and has the specified cache priority. In cache only a frame of the same or
	   higher priority can replace this frame. A value of 0 means the frame is not referenced. */
	ReferencePriority = 0xFA,
	/** [integer] A timestamp value, relative to the timestamp of the Block in this BlockGroup. This is used to
	   reference other frames necessary to decode this frame. The relative value **SHOULD** correspond to a valid
	   `Block` this `Block` depends on. Historically Matroska Writer didn't write the actual `Block(s)` this `Block`
	   depends on, but *some* `Block` in the past.  The value "0" **MAY** also be used to signify this `Block` cannot be
	   decoded on its own, but without knownledge of which `Block` is necessary. In this case, other `ReferenceBlock`
	   **MUST NOT** be found in the same `BlockGroup`.  If the `BlockGroup` doesn't have any `ReferenceBlock` element,
	   then the `Block` it contains can be decoded without using any other `Block` data. */
	ReferenceBlock = 0xFB,
	/** [integer] The Segment Position of the data that would otherwise be in position of the virtual block. */
	ReferenceVirtual = 0xFD,
	/** [binary] The new codec state to use. Data interpretation is private to the codec. This information **SHOULD**
	   always be referenced by a seek entry. */
	CodecState = 0xA4,
	/** [integer] Duration in nanoseconds of the silent data added to the Block (padding at the end of the Block for
	   positive value, at the beginning of the Block for negative value). The duration of DiscardPadding is not
	   calculated in the duration of the TrackEntry and **SHOULD** be discarded during playback. */
	DiscardPadding = 0x75A2,
	/** [master] Contains slices description. */
	Slices = 0x8E,
	/** [master] Contains extra time information about the data contained in the Block. Being able to interpret this
	   Element is not **REQUIRED** for playback. */
	TimeSlice = 0xE8,
	/** [uinteger] The reverse number of the frame in the lace (0 is the last frame, 1 is the next to last, etc). Being
	   able to interpret this Element is not **REQUIRED** for playback. */
	LaceNumber = 0xCC,
	/** [uinteger] The number of the frame to generate from this lace with this delay (allow you to generate many frames
	   from the same Block/Frame). */
	FrameNumber = 0xCD,
	/** [uinteger] The ID of the BlockAdditional Element (0 is the main Block). */
	BlockAdditionID = 0xCB,
	/** [uinteger] The (scaled) delay to apply to the Element. */
	Delay = 0xCE,
	/** [uinteger] The (scaled) duration to apply to the Element. */
	SliceDuration = 0xCF,
	/** [master] Contains information about the last reference frame. See [@?DivXTrickTrack]. */
	ReferenceFrame = 0xC8,
	/** [uinteger] The relative offset, in bytes, from the previous BlockGroup element for this Smooth FF/RW video track
	   to the containing BlockGroup element. See [@?DivXTrickTrack]. */
	ReferenceOffset = 0xC9,
	/** [uinteger] The timecode of the BlockGroup pointed to by ReferenceOffset. See [@?DivXTrickTrack]. */
	ReferenceTimestamp = 0xCA,
	/** [binary] Similar to SimpleBlock, see (#simpleblock-structure), but the data inside the Block are Transformed
	   (encrypt and/or signed). */
	EncryptedBlock = 0xAF,
	/** [master] A Top-Level Element of information with many tracks described. */
	Tracks = 0x1654AE6B,
	/** [master] Describes a track with all Elements. */
	TrackEntry = 0xAE,
	/** [uinteger] The track number as used in the Block Header (using more than 127 tracks is not encouraged, though
	   the design allows an unlimited number). */
	TrackNumber = 0xD7,
	/** [uinteger] A unique ID to identify the Track. */
	TrackUID = 0x73C5,
	/** [uinteger] The `TrackType` defines the type of each frame found in the Track. The value **SHOULD** be stored on
	   1 octet. */
	TrackType = 0x83,
	/** [uinteger] Set to 1 if the track is usable. It is possible to turn a not usable track into a usable track using
	   chapter codecs or control tracks. */
	FlagEnabled = 0xB9,
	/** [uinteger] Set if that track (audio, video or subs) **SHOULD** be eligible for automatic selection by the
	   player; see (#default-track-selection) for more details. */
	FlagDefault = 0x88,
	/** [uinteger] Applies only to subtitles. Set if that track **SHOULD** be eligible for automatic selection by the
	   player if it matches the user's language preference, even if the user's preferences would normally not enable
	   subtitles with the selected audio track; this can be used for tracks containing only translations of
	   foreign-language audio or onscreen text. See (#default-track-selection) for more details. */
	FlagForced = 0x55AA,
	/** [uinteger] Set to 1 if that track is suitable for users with hearing impairments, set to 0 if it is unsuitable
	   for users with hearing impairments. */
	FlagHearingImpaired = 0x55AB,
	/** [uinteger] Set to 1 if that track is suitable for users with visual impairments, set to 0 if it is unsuitable
	   for users with visual impairments. */
	FlagVisualImpaired = 0x55AC,
	/** [uinteger] Set to 1 if that track contains textual descriptions of video content, set to 0 if that track does
	   not contain textual descriptions of video content. */
	FlagTextDescriptions = 0x55AD,
	/** [uinteger] Set to 1 if that track is in the content's original language, set to 0 if it is a translation. */
	FlagOriginal = 0x55AE,
	/** [uinteger] Set to 1 if that track contains commentary, set to 0 if it does not contain commentary. */
	FlagCommentary = 0x55AF,
	/** [uinteger] Set to 1 if the track **MAY** contain blocks using lacing. When set to 0 all blocks **MUST** have
	   their lacing flags set to No lacing; see (#block-lacing) on Block Lacing. */
	FlagLacing = 0x9C,
	/** [uinteger] The minimum number of frames a player **SHOULD** be able to cache during playback. If set to 0, the
	   reference pseudo-cache system is not used. */
	MinCache = 0x6DE7,
	/** [uinteger] The maximum cache size necessary to store referenced frames in and the current frame. 0 means no
	   cache is needed. */
	MaxCache = 0x6DF8,
	/** [uinteger] Number of nanoseconds (not scaled via TimestampScale) per frame (frame in the Matroska sense -- one
	   Element put into a (Simple)Block). */
	DefaultDuration = 0x23E383,
	/** [uinteger] The period in nanoseconds (not scaled by TimestampScale) between two successive fields at the output
	   of the decoding process, see (#defaultdecodedfieldduration) for more information */
	DefaultDecodedFieldDuration = 0x234E7A,
	/** [float] DEPRECATED, DO NOT USE. The scale to apply on this track to work at normal speed in relation with other
	   tracks (mostly used to adjust video speed when the audio length differs). */
	TrackTimestampScale = 0x23314F,
	/** [integer] A value to add to the Block's Timestamp. This can be used to adjust the playback offset of a track. */
	TrackOffset = 0x537F,
	/** [uinteger] The maximum value of BlockAddID ((#blockaddid-element)). A value 0 means there is no BlockAdditions
	   ((#blockadditions-element)) for this track. */
	MaxBlockAdditionID = 0x55EE,
	/** [master] Contains elements that extend the track format, by adding content either to each frame, with BlockAddID
	   ((#blockaddid-element)), or to the track as a whole with BlockAddIDExtraData. */
	BlockAdditionMapping = 0x41E4,
	/** [uinteger] If the track format extension needs content beside frames, the value refers to the BlockAddID
	   ((#blockaddid-element)), value being described. To keep MaxBlockAdditionID as low as possible, small values
	   **SHOULD** be used. */
	BlockAddIDValue = 0x41F0,
	/** [string] A human-friendly name describing the type of BlockAdditional data, as defined by the associated Block
	   Additional Mapping. */
	BlockAddIDName = 0x41A4,
	/** [uinteger] Stores the registered identifier of the Block Additional Mapping to define how the BlockAdditional
	   data should be handled. */
	BlockAddIDType = 0x41E7,
	/** [binary] Extra binary data that the BlockAddIDType can use to interpret the BlockAdditional data. The
	   interpretation of the binary data depends on the BlockAddIDType value and the corresponding Block Additional
	   Mapping. */
	BlockAddIDExtraData = 0x41ED,
	/** [utf-8] A human-readable track name. */
	Name = 0x536E,
	/** [string] Specifies the language of the track in the Matroska languages form; see (#language-codes) on language
	   codes. This Element **MUST** be ignored if the LanguageIETF Element is used in the same TrackEntry. */
	Language = 0x22B59C,
	/** [string] Specifies the language of the track according to [@!BCP47] and using the IANA Language Subtag Registry
	   [@!IANALangRegistry]. If this Element is used, then any Language Elements used in the same TrackEntry **MUST** be
	   ignored. */
	LanguageIETF = 0x22B59D,
	/** [string] An ID corresponding to the codec, see [@!MatroskaCodec] for more info. */
	CodecID = 0x86,
	/** [binary] Private data only known to the codec. */
	CodecPrivate = 0x63A2,
	/** [utf-8] A human-readable string specifying the codec. */
	CodecName = 0x258688,
	/** [uinteger] The UID of an attachment that is used by this codec. */
	AttachmentLink = 0x7446,
	/** [utf-8] A string describing the encoding setting used. */
	CodecSettings = 0x3A9697,
	/** [string] A URL to find information about the codec used. */
	CodecInfoURL = 0x3B4040,
	/** [string] A URL to download about the codec used. */
	CodecDownloadURL = 0x26B240,
	/** [uinteger] Set to 1 if the codec can decode potentially damaged data. */
	CodecDecodeAll = 0xAA,
	/** [uinteger] Specify that this track is an overlay track for the Track specified (in the u-integer). That means
	   when this track has a gap, see (#silenttracks-element) on SilentTracks, the overlay track **SHOULD** be used
	   instead. The order of multiple TrackOverlay matters, the first one is the one that **SHOULD** be used. If not
	   found it **SHOULD** be the second, etc. */
	TrackOverlay = 0x6FAB,
	/** [uinteger] CodecDelay is The codec-built-in delay in nanoseconds. This value **MUST** be subtracted from each
	   block timestamp in order to get the actual timestamp. The value **SHOULD** be small so the muxing of tracks with
	   the same actual timestamp are in the same Cluster. */
	CodecDelay = 0x56AA,
	/** [uinteger] After a discontinuity, SeekPreRoll is the duration in nanoseconds of the data the decoder **MUST**
	   decode before the decoded data is valid. */
	SeekPreRoll = 0x56BB,
	/** [master] The mapping between this `TrackEntry` and a track value in the given Chapter Codec. */
	TrackTranslate = 0x6624,
	/** [binary] The binary value used to represent this `TrackEntry` in the chapter codec data. The format depends on
	   the `ChapProcessCodecID` used; see (#chapprocesscodecid-element). */
	TrackTranslateTrackID = 0x66A5,
	/** [uinteger] This `TrackTranslate` applies to this chapter codec of the given chapter edition(s); see
	   (#chapprocesscodecid-element). */
	TrackTranslateCodec = 0x66BF,
	/** [uinteger] Specify a chapter edition UID on which this `TrackTranslate` applies. */
	TrackTranslateEditionUID = 0x66FC,
	/** [master] Video settings. */
	Video = 0xE0,
	/** [uinteger] Specify whether the video frames in this track are interlaced or not. */
	FlagInterlaced = 0x9A,
	/** [uinteger] Specify the field ordering of video frames in this track. */
	FieldOrder = 0x9D,
	/** [uinteger] Stereo-3D video mode. There are some more details in (#multi-planar-and-3d-videos). */
	StereoMode = 0x53B8,
	/** [uinteger] Alpha Video Mode. Presence of this Element indicates that the BlockAdditional Element could contain
	   Alpha data. */
	AlphaMode = 0x53C0,
	/** [uinteger] DEPRECATED, DO NOT USE. Bogus StereoMode value used in old versions of libmatroska. */
	OldStereoMode = 0x53B9,
	/** [uinteger] Width of the encoded video frames in pixels. */
	PixelWidth = 0xB0,
	/** [uinteger] Height of the encoded video frames in pixels. */
	PixelHeight = 0xBA,
	/** [uinteger] The number of video pixels to remove at the bottom of the image. */
	PixelCropBottom = 0x54AA,
	/** [uinteger] The number of video pixels to remove at the top of the image. */
	PixelCropTop = 0x54BB,
	/** [uinteger] The number of video pixels to remove on the left of the image. */
	PixelCropLeft = 0x54CC,
	/** [uinteger] The number of video pixels to remove on the right of the image. */
	PixelCropRight = 0x54DD,
	/** [uinteger] Width of the video frames to display. Applies to the video frame after cropping (PixelCrop*
	   Elements). */
	DisplayWidth = 0x54B0,
	/** [uinteger] Height of the video frames to display. Applies to the video frame after cropping (PixelCrop*
	   Elements). */
	DisplayHeight = 0x54BA,
	/** [uinteger] How DisplayWidth & DisplayHeight are interpreted. */
	DisplayUnit = 0x54B2,
	/** [uinteger] Specify the possible modifications to the aspect ratio. */
	AspectRatioType = 0x54B3,
	/** [binary] Specify the uncompressed pixel format used for the Track's data as a FourCC. This value is similar in
	   scope to the biCompression value of AVI's `BITMAPINFO` [@?AVIFormat]. See the YUV video formats [@?FourCC-YUV]
	   and RGB video formats [@?FourCC-RGB] for common values. */
	UncompressedFourCC = 0x2EB524,
	/** [float] Gamma Value. */
	GammaValue = 0x2FB523,
	/** [float] Number of frames per second. This value is Informational only. It is intended for constant frame rate
	   streams, and **SHOULD NOT** be used for a variable frame rate TrackEntry. */
	FrameRate = 0x2383E3,
	/** [master] Settings describing the colour format. */
	Colour = 0x55B0,
	/** [uinteger] The Matrix Coefficients of the video used to derive luma and chroma values from red, green, and blue
	   color primaries. For clarity, the value and meanings for MatrixCoefficients are adopted from Table 4 of ISO/IEC
	   23001-8:2016 or ITU-T H.273. */
	MatrixCoefficients = 0x55B1,
	/** [uinteger] Number of decoded bits per channel. A value of 0 indicates that the BitsPerChannel is unspecified. */
	BitsPerChannel = 0x55B2,
	/** [uinteger] The amount of pixels to remove in the Cr and Cb channels for every pixel not removed horizontally.
	   Example: For video with 4:2:0 chroma subsampling, the ChromaSubsamplingHorz **SHOULD** be set to 1. */
	ChromaSubsamplingHorz = 0x55B3,
	/** [uinteger] The amount of pixels to remove in the Cr and Cb channels for every pixel not removed vertically.
	   Example: For video with 4:2:0 chroma subsampling, the ChromaSubsamplingVert **SHOULD** be set to 1. */
	ChromaSubsamplingVert = 0x55B4,
	/** [uinteger] The amount of pixels to remove in the Cb channel for every pixel not removed horizontally. This is
	   additive with ChromaSubsamplingHorz. Example: For video with 4:2:1 chroma subsampling, the ChromaSubsamplingHorz
	   **SHOULD** be set to 1 and CbSubsamplingHorz **SHOULD** be set to 1. */
	CbSubsamplingHorz = 0x55B5,
	/** [uinteger] The amount of pixels to remove in the Cb channel for every pixel not removed vertically. This is
	   additive with ChromaSubsamplingVert. */
	CbSubsamplingVert = 0x55B6,
	/** [uinteger] How chroma is subsampled horizontally. */
	ChromaSitingHorz = 0x55B7,
	/** [uinteger] How chroma is subsampled vertically. */
	ChromaSitingVert = 0x55B8,
	/** [uinteger] Clipping of the color ranges. */
	Range = 0x55B9,
	/** [uinteger] The transfer characteristics of the video. For clarity, the value and meanings for
	   TransferCharacteristics are adopted from Table 3 of ISO/IEC 23091-4 or ITU-T H.273. */
	TransferCharacteristics = 0x55BA,
	/** [uinteger] The colour primaries of the video. For clarity, the value and meanings for Primaries are adopted from
	   Table 2 of ISO/IEC 23091-4 or ITU-T H.273. */
	Primaries = 0x55BB,
	/** [uinteger] Maximum brightness of a single pixel (Maximum Content Light Level) in candelas per square meter
	   (cd/m^2^). */
	MaxCLL = 0x55BC,
	/** [uinteger] Maximum brightness of a single full frame (Maximum Frame-Average Light Level) in candelas per square
	   meter (cd/m^2^). */
	MaxFALL = 0x55BD,
	/** [master] SMPTE 2086 mastering data. */
	MasteringMetadata = 0x55D0,
	/** [float] Red X chromaticity coordinate, as defined by CIE 1931. */
	PrimaryRChromaticityX = 0x55D1,
	/** [float] Red Y chromaticity coordinate, as defined by CIE 1931. */
	PrimaryRChromaticityY = 0x55D2,
	/** [float] Green X chromaticity coordinate, as defined by CIE 1931. */
	PrimaryGChromaticityX = 0x55D3,
	/** [float] Green Y chromaticity coordinate, as defined by CIE 1931. */
	PrimaryGChromaticityY = 0x55D4,
	/** [float] Blue X chromaticity coordinate, as defined by CIE 1931. */
	PrimaryBChromaticityX = 0x55D5,
	/** [float] Blue Y chromaticity coordinate, as defined by CIE 1931. */
	PrimaryBChromaticityY = 0x55D6,
	/** [float] White X chromaticity coordinate, as defined by CIE 1931. */
	WhitePointChromaticityX = 0x55D7,
	/** [float] White Y chromaticity coordinate, as defined by CIE 1931. */
	WhitePointChromaticityY = 0x55D8,
	/** [float] Maximum luminance. Represented in candelas per square meter (cd/m^2^). */
	LuminanceMax = 0x55D9,
	/** [float] Minimum luminance. Represented in candelas per square meter (cd/m^2^). */
	LuminanceMin = 0x55DA,
	/** [master] Describes the video projection details. Used to render spherical, VR videos or flipping videos
	   horizontally/vertically. */
	Projection = 0x7670,
	/** [uinteger] Describes the projection used for this video track. */
	ProjectionType = 0x7671,
	/** [binary] Private data that only applies to a specific projection.  *  If `ProjectionType` equals 0
	   (Rectangular),      then this element must not be present. *  If `ProjectionType` equals 1 (Equirectangular),
	   then this element must be present and contain the same binary data that would be stored inside       an ISOBMFF
	   Equirectangular Projection Box ('equi'). *  If `ProjectionType` equals 2 (Cubemap), then this element must be
	   present and contain the same binary data that would be stored       inside an ISOBMFF Cubemap Projection Box
	   ('cbmp'). *  If `ProjectionType` equals 3 (Mesh), then this element must be present and contain the same binary
	   data that would be stored inside        an ISOBMFF Mesh Projection Box ('mshp'). */
	ProjectionPrivate = 0x7672,
	/** [float] Specifies a yaw rotation to the projection.  Value represents a clockwise rotation, in degrees, around
	   the up vector. This rotation must be applied before any `ProjectionPosePitch` or `ProjectionPoseRoll` rotations.
	   The value of this element **MUST** be in the -180 to 180 degree range, both included.  Setting
	   `ProjectionPoseYaw` to 180 or -180 degrees, with the `ProjectionPoseRoll` and `ProjectionPosePitch` set to 0
	   degrees flips the image horizontally. */
	ProjectionPoseYaw = 0x7673,
	/** [float] Specifies a pitch rotation to the projection.  Value represents a counter-clockwise rotation, in
	   degrees, around the right vector. This rotation must be applied after the `ProjectionPoseYaw` rotation and before
	   the `ProjectionPoseRoll` rotation. The value of this element **MUST** be in the -90 to 90 degree range, both
	   included. */
	ProjectionPosePitch = 0x7674,
	/** [float] Specifies a roll rotation to the projection.  Value represents a counter-clockwise rotation, in degrees,
	   around the forward vector. This rotation must be applied after the `ProjectionPoseYaw` and `ProjectionPosePitch`
	   rotations. The value of this element **MUST** be in the -180 to 180 degree range, both included.  Setting
	   `ProjectionPoseRoll` to 180 or -180 degrees, the `ProjectionPoseYaw` to 180 or -180 degrees with
	   `ProjectionPosePitch` set to 0 degrees flips the image vertically.  Setting `ProjectionPoseRoll` to 180 or -180
	   degrees, with the `ProjectionPoseYaw` and `ProjectionPosePitch` set to 0 degrees flips the image horizontally and
	   vertically. */
	ProjectionPoseRoll = 0x7675,
	/** [master] Audio settings. */
	Audio = 0xE1,
	/** [float] Sampling frequency in Hz. */
	SamplingFrequency = 0xB5,
	/** [float] Real output sampling frequency in Hz (used for SBR techniques). */
	OutputSamplingFrequency = 0x78B5,
	/** [uinteger] Numbers of channels in the track. */
	Channels = 0x9F,
	/** [binary] Table of horizontal angles for each successive channel. */
	ChannelPositions = 0x7D7B,
	/** [uinteger] Bits per sample, mostly used for PCM. */
	BitDepth = 0x6264,
	/** [master] Operation that needs to be applied on tracks to create this virtual track. For more details look at
	   (#track-operation). */
	TrackOperation = 0xE2,
	/** [master] Contains the list of all video plane tracks that need to be combined to create this 3D track */
	TrackCombinePlanes = 0xE3,
	/** [master] Contains a video plane track that need to be combined to create this 3D track */
	TrackPlane = 0xE4,
	/** [uinteger] The trackUID number of the track representing the plane. */
	TrackPlaneUID = 0xE5,
	/** [uinteger] The kind of plane this track corresponds to. */
	TrackPlaneType = 0xE6,
	/** [master] Contains the list of all tracks whose Blocks need to be combined to create this virtual track */
	TrackJoinBlocks = 0xE9,
	/** [uinteger] The trackUID number of a track whose blocks are used to create this virtual track. */
	TrackJoinUID = 0xED,
	/** [uinteger] The TrackUID of the Smooth FF/RW video in the paired EBML structure corresponding to this video
	   track. See [@?DivXTrickTrack]. */
	TrickTrackUID = 0xC0,
	/** [binary] The SegmentUID of the Segment containing the track identified by TrickTrackUID. See [@?DivXTrickTrack].
	 */
	TrickTrackSegmentUID = 0xC1,
	/** [uinteger] Set to 1 if this video track is a Smooth FF/RW track. If set to 1, MasterTrackUID and
	   MasterTrackSegUID should must be present and BlockGroups for this track must contain ReferenceFrame structures.
	   Otherwise, TrickTrackUID and TrickTrackSegUID must be present if this track has a corresponding Smooth FF/RW
	   track. See [@?DivXTrickTrack]. */
	TrickTrackFlag = 0xC6,
	/** [uinteger] The TrackUID of the video track in the paired EBML structure that corresponds to this Smooth FF/RW
	   track. See [@?DivXTrickTrack]. */
	TrickMasterTrackUID = 0xC7,
	/** [binary] The SegmentUID of the Segment containing the track identified by MasterTrackUID. See
	   [@?DivXTrickTrack]. */
	TrickMasterTrackSegmentUID = 0xC4,
	/** [master] Settings for several content encoding mechanisms like compression or encryption. */
	ContentEncodings = 0x6D80,
	/** [master] Settings for one content encoding like compression or encryption. */
	ContentEncoding = 0x6240,
	/** [uinteger] Tells when this modification was used during encoding/muxing starting with 0 and counting upwards.
	   The decoder/demuxer has to start with the highest order number it finds and work its way down. This value has to
	   be unique over all ContentEncodingOrder Elements in the TrackEntry that contains this ContentEncodingOrder
	   element. */
	ContentEncodingOrder = 0x5031,
	/** [uinteger] A bit field that describes which Elements have been modified in this way. Values (big-endian) can be
	   OR'ed. */
	ContentEncodingScope = 0x5032,
	/** [uinteger] A value describing what kind of transformation is applied. */
	ContentEncodingType = 0x5033,
	/** [master] Settings describing the compression used. This Element **MUST** be present if the value of
	   ContentEncodingType is 0 and absent otherwise. Each block **MUST** be decompressable even if no previous block is
	   available in order not to prevent seeking. */
	ContentCompression = 0x5034,
	/** [uinteger] The compression algorithm used. */
	ContentCompAlgo = 0x4254,
	/** [binary] Settings that might be needed by the decompressor. For Header Stripping (`ContentCompAlgo`=3), the
	   bytes that were removed from the beginning of each frames of the track. */
	ContentCompSettings = 0x4255,
	/** [master] Settings describing the encryption used. This Element **MUST** be present if the value of
	   `ContentEncodingType` is 1 (encryption) and **MUST** be ignored otherwise. */
	ContentEncryption = 0x5035,
	/** [uinteger] The encryption algorithm used. The value "0" means that the contents have not been encrypted. */
	ContentEncAlgo = 0x47E1,
	/** [binary] For public key algorithms this is the ID of the public key the the data was encrypted with. */
	ContentEncKeyID = 0x47E2,
	/** [master] Settings describing the encryption algorithm used. If `ContentEncAlgo` != 5 this **MUST** be ignored.
	 */
	ContentEncAESSettings = 0x47E7,
	/** [uinteger] The AES cipher mode used in the encryption. */
	AESSettingsCipherMode = 0x47E8,
	/** [binary] A cryptographic signature of the contents. */
	ContentSignature = 0x47E3,
	/** [binary] This is the ID of the private key the data was signed with. */
	ContentSigKeyID = 0x47E4,
	/** [uinteger] The algorithm used for the signature. */
	ContentSigAlgo = 0x47E5,
	/** [uinteger] The hash algorithm used for the signature. */
	ContentSigHashAlgo = 0x47E6,
	/** [master] A Top-Level Element to speed seeking access. All entries are local to the Segment. */
	Cues = 0x1C53BB6B,
	/** [master] Contains all information relative to a seek point in the Segment. */
	CuePoint = 0xBB,
	/** [uinteger] Absolute timestamp according to the Segment time base. */
	CueTime = 0xB3,
	/** [master] Contain positions for different tracks corresponding to the timestamp. */
	CueTrackPositions = 0xB7,
	/** [uinteger] The track for which a position is given. */
	CueTrack = 0xF7,
	/** [uinteger] The Segment Position of the Cluster containing the associated Block. */
	CueClusterPosition = 0xF1,
	/** [uinteger] The relative position inside the Cluster of the referenced SimpleBlock or BlockGroup with 0 being the
	   first possible position for an Element inside that Cluster. */
	CueRelativePosition = 0xF0,
	/** [uinteger] The duration of the block according to the Segment time base. If missing the track's DefaultDuration
	   does not apply and no duration information is available in terms of the cues. */
	CueDuration = 0xB2,
	/** [uinteger] Number of the Block in the specified Cluster. */
	CueBlockNumber = 0x5378,
	/** [uinteger] The Segment Position of the Codec State corresponding to this Cue Element. 0 means that the data is
	   taken from the initial Track Entry. */
	CueCodecState = 0xEA,
	/** [master] The Clusters containing the referenced Blocks. */
	CueReference = 0xDB,
	/** [uinteger] Timestamp of the referenced Block. */
	CueRefTime = 0x96,
	/** [uinteger] The Segment Position of the Cluster containing the referenced Block. */
	CueRefCluster = 0x97,
	/** [uinteger] Number of the referenced Block of Track X in the specified Cluster. */
	CueRefNumber = 0x535F,
	/** [uinteger] The Segment Position of the Codec State corresponding to this referenced Element. 0 means that the
	   data is taken from the initial Track Entry. */
	CueRefCodecState = 0xEB,
	/** [master] Contain attached files. */
	Attachments = 0x1941A469,
	/** [master] An attached file. */
	AttachedFile = 0x61A7,
	/** [utf-8] A human-friendly name for the attached file. */
	FileDescription = 0x467E,
	/** [utf-8] Filename of the attached file. */
	FileName = 0x466E,
	/** [string] MIME type of the file. */
	FileMimeType = 0x4660,
	/** [binary] The data of the file. */
	FileData = 0x465C,
	/** [uinteger] Unique ID representing the file, as random as possible. */
	FileUID = 0x46AE,
	/** [binary] A binary value that a track/codec can refer to when the attachment is needed. */
	FileReferral = 0x4675,
	/** [uinteger] The timecode at which this optimized font attachment comes into context, based on the Segment
	   TimecodeScale. This element is reserved for future use and if written must be the segment start time. See
	   [@?DivXWorldFonts]. */
	FileUsedStartTime = 0x4661,
	/** [uinteger] The timecode at which this optimized font attachment goes out of context, based on the Segment
	   TimecodeScale. This element is reserved for future use and if written must be the segment end time. See
	   [@?DivXWorldFonts]. */
	FileUsedEndTime = 0x4662,
	/** [master] A system to define basic menus and partition data. For more detailed information, look at the Chapters
	   explanation in (#chapters). */
	Chapters = 0x1043A770,
	/** [master] Contains all information about a Segment edition. */
	EditionEntry = 0x45B9,
	/** [uinteger] A unique ID to identify the edition. It's useful for tagging an edition. */
	EditionUID = 0x45BC,
	/** [uinteger] Set to 1 if an edition is hidden. Hidden editions **SHOULD NOT** be available to the user interface
	   (but still to Control Tracks; see (#chapter-flags) on Chapter flags). */
	EditionFlagHidden = 0x45BD,
	/** [uinteger] Set to 1 if the edition **SHOULD** be used as the default one. */
	EditionFlagDefault = 0x45DB,
	/** [uinteger] Set to 1 if the chapters can be defined multiple times and the order to play them is enforced; see
	   (#editionflagordered). */
	EditionFlagOrdered = 0x45DD,
	/** [master] Contains the atom information to use as the chapter atom (apply to all tracks). */
	ChapterAtom = 0xB6,
	/** [uinteger] A unique ID to identify the Chapter. */
	ChapterUID = 0x73C4,
	/** [utf-8] A unique string ID to identify the Chapter. Use for WebVTT cue identifier storage [@!WebVTT]. */
	ChapterStringUID = 0x5654,
	/** [uinteger] Timestamp of the start of Chapter (not scaled). */
	ChapterTimeStart = 0x91,
	/** [uinteger] Timestamp of the end of Chapter (timestamp excluded, not scaled). The value **MUST** be greater than
	   or equal to the `ChapterTimeStart` of the same `ChapterAtom`. */
	ChapterTimeEnd = 0x92,
	/** [uinteger] Set to 1 if a chapter is hidden. Hidden chapters **SHOULD NOT** be available to the user interface
	   (but still to Control Tracks; see (#chapterflaghidden) on Chapter flags). */
	ChapterFlagHidden = 0x98,
	/** [uinteger] Set to 1 if the chapter is enabled. It can be enabled/disabled by a Control Track. When disabled, the
	   movie **SHOULD** skip all the content between the TimeStart and TimeEnd of this chapter; see (#chapter-flags) on
	   Chapter flags. */
	ChapterFlagEnabled = 0x4598,
	/** [binary] The SegmentUID of another Segment to play during this chapter. */
	ChapterSegmentUID = 0x6E67,
	/** [uinteger] The EditionUID to play from the Segment linked in ChapterSegmentUID. If ChapterSegmentEditionUID is
	   undeclared, then no Edition of the linked Segment is used; see (#medium-linking) on medium-linking Segments. */
	ChapterSegmentEditionUID = 0x6EBC,
	/** [uinteger] Specify the physical equivalent of this ChapterAtom like "DVD" (60) or "SIDE" (50); see
	   (#physical-types) for a complete list of values. */
	ChapterPhysicalEquiv = 0x63C3,
	/** [master] List of tracks on which the chapter applies. If this Element is not present, all tracks apply */
	ChapterTrack = 0x8F,
	/** [uinteger] UID of the Track to apply this chapter to. In the absence of a control track, choosing this chapter
	   will select the listed Tracks and deselect unlisted tracks. Absence of this Element indicates that the Chapter
	   **SHOULD** be applied to any currently used Tracks. */
	ChapterTrackUID = 0x89,
	/** [master] Contains all possible strings to use for the chapter display. */
	ChapterDisplay = 0x80,
	/** [utf-8] Contains the string to use as the chapter atom. */
	ChapString = 0x85,
	/** [string] A language corresponding to the string, in the bibliographic ISO-639-2 form [@!ISO639-2]. This Element
	 **MUST** be ignored if a ChapLanguageIETF Element is used within the same ChapterDisplay Element. */
	ChapLanguage = 0x437C,
	/** [string] Specifies a language corresponding to the ChapString in the format defined in [@!BCP47] and using the
	   IANA Language Subtag Registry [@!IANALangRegistry]. If a ChapLanguageIETF Element is used, then any ChapLanguage
	   and ChapCountry Elements used in the same ChapterDisplay **MUST** be ignored. */
	ChapLanguageIETF = 0x437D,
	/** [string] A country corresponding to the string, using the same 2 octets country-codes as in Internet domains
	   [@!IANADomains] based on [@!ISO3166-1] alpha-2 codes. This Element **MUST** be ignored if a ChapLanguageIETF
	   Element is used within the same ChapterDisplay Element. */
	ChapCountry = 0x437E,
	/** [master] Contains all the commands associated to the Atom. */
	ChapProcess = 0x6944,
	/** [uinteger] Contains the type of the codec used for the processing. A value of 0 means native Matroska processing
	   (to be defined), a value of 1 means the DVD command set is used; see (#menu-features) on DVD menus. More codec
	   IDs can be added later. */
	ChapProcessCodecID = 0x6955,
	/** [binary] Some optional data attached to the ChapProcessCodecID information.     For ChapProcessCodecID = 1, it
	   is the "DVD level" equivalent; see (#menu-features) on DVD menus. */
	ChapProcessPrivate = 0x450D,
	/** [master] Contains all the commands associated to the Atom. */
	ChapProcessCommand = 0x6911,
	/** [uinteger] Defines when the process command **SHOULD** be handled */
	ChapProcessTime = 0x6922,
	/** [binary] Contains the command information. The data **SHOULD** be interpreted depending on the
	   ChapProcessCodecID value. For ChapProcessCodecID = 1, the data correspond to the binary DVD cell pre/post
	   commands; see (#menu-features) on DVD menus. */
	ChapProcessData = 0x6933,
	/** [master] Element containing metadata describing Tracks, Editions, Chapters, Attachments, or the Segment as a
	   whole. A list of valid tags can be found in [@!MatroskaTags]. */
	Tags = 0x1254C367,
	/** [master] A single metadata descriptor. */
	Tag = 0x7373,
	/** [master] Specifies which other elements the metadata represented by the Tag applies to. If empty or not present,
	   then the Tag describes everything in the Segment. */
	Targets = 0x63C0,
	/** [uinteger] A number to indicate the logical level of the target. */
	TargetTypeValue = 0x68CA,
	/** [string] An informational string that can be used to display the logical level of the target like "ALBUM",
	   "TRACK", "MOVIE", "CHAPTER", etc ; see Section 6.4 of [@!MatroskaTags]. */
	TargetType = 0x63CA,
	/** [uinteger] A unique ID to identify the Track(s) the tags belong to. */
	TagTrackUID = 0x63C5,
	/** [uinteger] A unique ID to identify the EditionEntry(s) the tags belong to. */
	TagEditionUID = 0x63C9,
	/** [uinteger] A unique ID to identify the Chapter(s) the tags belong to. */
	TagChapterUID = 0x63C4,
	/** [uinteger] A unique ID to identify the Attachment(s) the tags belong to. */
	TagAttachmentUID = 0x63C6,
	/** [master] Contains general information about the target. */
	SimpleTag = 0x67C8,
	/** [utf-8] The name of the Tag that is going to be stored. */
	TagName = 0x45A3,
	/** [string] Specifies the language of the tag specified, in the Matroska languages form; see (#language-codes) on
	   language codes. This Element **MUST** be ignored if the TagLanguageIETF Element is used within the same SimpleTag
	   Element. */
	TagLanguage = 0x447A,
	/** [string] Specifies the language used in the TagString according to [@!BCP47] and using the IANA Language Subtag
	   Registry [@!IANALangRegistry]. If this Element is used, then any TagLanguage Elements used in the same SimpleTag
	   **MUST** be ignored. */
	TagLanguageIETF = 0x447B,
	/** [uinteger] A boolean value to indicate if this is the default/original language to use for the given tag. */
	TagDefault = 0x4484,
	/** [uinteger] A variant of the TagDefault element with a bogus Element ID; see (#tagdefault-element). */
	TagDefaultBogus = 0x44B4,
	/** [utf-8] The value of the Tag. */
	TagString = 0x4487,
	/** [binary] The values of the Tag, if it is binary. Note that this cannot be used in the same SimpleTag as
	   TagString. */
	TagBinary = 0x4485,
};
