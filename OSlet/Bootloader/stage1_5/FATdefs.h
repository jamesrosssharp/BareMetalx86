; FAT Boot Sector Definitions (from Microsoft)

FAT_BPB_BS_OEMName	equ 	3
FAT_BPB_BPB_BytsPerSec  equ	11

FAT_BPB_SecPerClus 	equ	13 ;1 byte
FAT_BPB_RsvdSecCnt 	equ	14 ;2 bytes
FAT_BPB_NumFATs		equ	16 ;1 byte
FAT_BPB_RootEntCnt 	equ	17 ;2
FAT_BPB_TotSec16 	equ	19 ;2
FAT_BPB_Media 		equ	21 ;1
FAT_BPB_FATSz16 	equ	22 ;2
FAT_BPB_SecPerTrk 	equ	24 ;2
FAT_BPB_NumHeads 	equ	26 ;2
FAT_BPB_HiddSec 	equ	28 ;4
FAT_BPB_TotSec32 	equ	32 ;4

; FAT 12 / 16
FAT_BS_DrvNum 		equ	36
FAT_BS_Reserved1 	equ	37 ;1
FAT_BS_BootSig 		equ	38 ;1
FAT_BS_VolID 		equ	39 ;4
FAT_BS_VolLab 		equ	43 ;11
FAT_BS_FilSysType 	equ	54 ;8

; FAT 32
FAT32_BPB_FATSz32 		equ	36 ;4
FAT32_BPB_ExtFlags 		equ	40 ;2
FAT32_BPB_FSVer 		equ	42 ;2
FAT32_BPB_RootClus 		equ	44 ;4
FAT32_BPB_FSInfo 		equ	48 ;2
FAT32_BPB_BkBootSec 		equ	50 ;2
FAT32_BPB_Reserved 		equ	52 ;12

FAT32_BS_DrvNum 		equ	64 ;1
FAT32_BS_Reserved1 		equ	65 ;1
FAT32_BS_BootSig 		equ	66 ;1
FAT32_BS_VolID 			equ	67 ;4
FAT32_BS_VolLab 		equ	71 ;11
FAT32_BS_FilSysType 		equ	82 ;8

; FAT directory

FAT_DIR_Name			equ	0
FAT_DIR_Attr			equ	11
FAT_DIR_NTRes 			equ	12 ;1
FAT_DIR_CrtTimeTenth 		equ	13 ;1
FAT_DIR_CrtTime			equ	14
FAT_DIR_CrtDate			equ	16
FAT_DIR_LstAccDate 		equ	18
FAT_DIR_FstClusHI 		equ	20
FAT_DIR_WrtTime			equ	22
FAT_DIR_WrtDate			equ	24
FAT_DIR_FstClusLO		equ	26
FAT_DIR_FileSize		equ	28

