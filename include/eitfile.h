#ifndef __is_included__555b8739_1220_4609_8f37_d639ae02000d
#define __is_included__555b8739_1220_4609_8f37_d639ae02000d

#include <stdint.h>

#ifdef __cplusplus
	extern "C" {
#endif

enum eitfileError {
	eitfileE_Ok								= 0,

	eitfileE_InvalidParam,
	eitfileE_OutOfMemory,
	eitfileE_EncodingError,
	eitfileE_UnknownDescriptorId,
};

enum eitfileTableId {
	eitfileTable_ProgramAssociationSection					= 0x00,
	eitfileTable_ConditionalAccessSection					= 0x01,
	eitfileTable_ProgramMapSection							= 0x02,
	eitfileTable_TransportStreamDescriptionSection			= 0x03,
	eitfileTable_NetworkInformationSection_Actual			= 0x40,
	eitfileTable_NetworkInformationSection_Other			= 0x41,
	eitfileTable_ServiceDescriptionSection_Actual			= 0x42,
	eitfileTable_ServiceDescriptionSection_Other			= 0x46,
	eitfileTable_BouquetAssociationSection					= 0x4A,
	eitfileTable_EventInformationSection_Actual				= 0x4E,
	eitfileTable_EventInformationSection_Other				= 0x4F,
	eitfileTable_EventInformationSection_Actual_Shedule_1	= 0x50,
	eitfileTable_EventInformationSection_Actual_Shedule_2	= 0x51,
	eitfileTable_EventInformationSection_Actual_Shedule_3	= 0x52,
	eitfileTable_EventInformationSection_Actual_Shedule_4	= 0x53,
	eitfileTable_EventInformationSection_Actual_Shedule_5	= 0x54,
	eitfileTable_EventInformationSection_Actual_Shedule_6	= 0x55,
	eitfileTable_EventInformationSection_Actual_Shedule_7	= 0x56,
	eitfileTable_EventInformationSection_Actual_Shedule_8	= 0x57,
	eitfileTable_EventInformationSection_Actual_Shedule_9	= 0x58,
	eitfileTable_EventInformationSection_Actual_Shedule_10	= 0x59,
	eitfileTable_EventInformationSection_Actual_Shedule_11	= 0x5A,
	eitfileTable_EventInformationSection_Actual_Shedule_12	= 0x5B,
	eitfileTable_EventInformationSection_Actual_Shedule_13	= 0x5C,
	eitfileTable_EventInformationSection_Actual_Shedule_14	= 0x5D,
	eitfileTable_EventInformationSection_Actual_Shedule_15	= 0x5E,
	eitfileTable_EventInformationSection_Actual_Shedule_16	= 0x5F,
	eitfileTable_EventInformationSection_Other_Shedule_1	= 0x60,
	eitfileTable_EventInformationSection_Other_Shedule_2	= 0x61,
	eitfileTable_EventInformationSection_Other_Shedule_3	= 0x62,
	eitfileTable_EventInformationSection_Other_Shedule_4	= 0x63,
	eitfileTable_EventInformationSection_Other_Shedule_5	= 0x64,
	eitfileTable_EventInformationSection_Other_Shedule_6	= 0x65,
	eitfileTable_EventInformationSection_Other_Shedule_7	= 0x66,
	eitfileTable_EventInformationSection_Other_Shedule_8	= 0x67,
	eitfileTable_EventInformationSection_Other_Shedule_9	= 0x68,
	eitfileTable_EventInformationSection_Other_Shedule_19	= 0x69,
	eitfileTable_EventInformationSection_Other_Shedule_11	= 0x6A,
	eitfileTable_EventInformationSection_Other_Shedule_12	= 0x6B,
	eitfileTable_EventInformationSection_Other_Shedule_13	= 0x6C,
	eitfileTable_EventInformationSection_Other_Shedule_14	= 0x6D,
	eitfileTable_EventInformationSection_Other_Shedule_15	= 0x6E,
	eitfileTable_EventInformationSection_Other_Shedule_16	= 0x6F,
	eitfileTable_TimeDateSection							= 0x70,
	eitfileTable_RunningStatusSection						= 0x71,
	eitfileTable_StuffingSection							= 0x72,
	eitfileTable_TimeOffsetSection							= 0x73,
	eitfileTable_ApplicationInformationSection				= 0x74,
	eitfileTable_ContainerSection							= 0x75,
	eitfileTable_RelatedContentSection						= 0x76,
	eitfileTable_ContentIdentifierSection					= 0x77,
	eitfileTable_MPEFECSection								= 0x78,
	eitfileTable_ResolutionNotificationSection				= 0x79,
	eitfileTable_DiscontinuityInformationSection			= 0x7E,
	eitfileTable_SelectionInformationSection				= 0x7F,
};

/*
	Currently only lists descriptor types allowed inside EIT table.
*/
enum eitfileDescriptorType {
	eitfileDescriptorType_StuffingDescriptor				= 0x42,
	eitfileDescriptorType_LinkageDescriptor					= 0x4A,
	eitfileDescriptorType_ShortEventDescriptor				= 0x4D,
	eitfileDescriptorType_ExtendedEventDescriptor			= 0x4E,
	eitfileDescriptorType_TimeShiftedEventDescriptor		= 0x4F,
	eitfileDescriptorType_ComponentDescriptor				= 0x50,
	eitfileDescriptorType_CAIdentifierDescriptor			= 0x53,
	eitfileDescriptorType_ContentDescriptor					= 0x54,
	eitfileDescriptorType_ParentalRatingDescriptor			= 0x55,
	eitfileDescriptorType_TelephoneDescriptor				= 0x57,
	eitfileDescriptorType_MultiligualComponentDescriptor	= 0x5E,
	eitfileDescriptorType_PrivateDataSpecifier				= 0x5F,
	eitfileDescriptorType_ShortSmoothingBufferDescriptor	= 0x61,
	eitfileDescriptorType_DataBroadcastDescriptor			= 0x64,
	eitfileDescriptorType_PDCDescriptor						= 0x69,
	eitfileDescriptorType_TVAIDDescriptor					= 0x75,
	eitfileDescriptorType_ContentIdentifierDescriptor		= 0x76,
	eitfileDescriptorType_XAITLocationDescriptor			= 0x7D,
	eitfileDescriptorType_FTAContentManagementDescriptor	= 0x7E,
	eitfileDescriptorType_ExtensionDescriptor				= 0x7F,
};

enum eitRunningStatus {
	eitRunningStatus_Undefined							= 0,
	eitRunningStatus_NotRunning							= 1,
	eitRunningStatus_StartsFewSeconds					= 2,
	eitRunningStatus_Pausing							= 3,
	eitRunningStatus_Running							= 4,
	eitRunningStatus_ServiceOffAir						= 5,
};

struct eitfileDescriptor {
	enum eitfileDescriptorType							descriptorType;
	struct {
		struct eitfileDescriptor*						lpNext;
		struct eitfileDescriptor*						lpPrev;
	} descriptorList;
};
struct eitfileDescriptor_ShortEventDescriptor {
	struct eitfileDescriptor							base;
	unsigned char										langCode[3];
	unsigned long int									dwEventNameLength;
	unsigned long int									dwTextLength;
	unsigned char										bNameAndTextData[];	/* First eventNameLength bytes, then dwTextLength bytes */
};


struct eitfileDescriptor_ExtendedEventDescriptor_Item {
	struct eitfileDescriptor_ExtendedEventDescriptor_Item* lpNext;
	unsigned long int									dwLength_ItemDescripton;
	unsigned long int									dwLength_Item;
	unsigned char										bItemAndDescription[]; /* Frist dwLength_ItemDescripton description followed by dwLength_Item bytes item */
};

struct eitfileDescriptor_ExtendedEventDescriptor {
	struct eitfileDescriptor							base;
	unsigned long int									dwDescriptorNumber;
	unsigned long int									dwLastDescriptorNumber;
	unsigned char										langCode[3];
	struct eitfileDescriptor_ExtendedEventDescriptor_Item* lpItemizedItems;
	unsigned long int									dwTextLength;
	unsigned char*										lpText; /* Use THIS and not bText directly to access the text .... */
	unsigned char										bText[];
};

struct eitfileDescriptor_ContentDescriptor {
	struct eitfileDescriptor							base;
	unsigned long int									dwClassCount;
	unsigned char										bClasses[];
};

#define eitfileEITEvent_Entry__FLAGS__FREE_CA_MODE		0x00000001
struct eitfileEITEvent_Entry {
	uint32_t											flags;
	uint16_t											eventId;
	struct {
		unsigned long int year;
		unsigned long int month;
		unsigned long int day;
		unsigned long int hour;
		unsigned long int minute;
		unsigned long int second;
	} startTime;
	struct {
		unsigned long int hour;
		unsigned long int minute;
		unsigned long int second;
	} duration;
	unsigned long int									durationSeconds;
	enum eitRunningStatus								runningStatus;

	struct {
		struct eitfileDescriptor*						lpFirst;
		struct eitfileDescriptor*						lpLast;
	} descriptorList;
};

void eitfileReleaseDescriptor(
	struct eitfileDescriptor* lpTable
);
void eitfileReleaseEvent(
	struct eitfileEITEvent_Entry* lpEvent
);

enum eitfileError eitfileParse_Descriptor(
	unsigned char* lpData,
	unsigned long int dwBufferSize,
	unsigned long int dwCurrentBase,

	unsigned long int* lpBytesConsumed,
	struct eitfileDescriptor** lpOut
);

enum eitfileError eitfileParse_EIT_Event(
	unsigned char* lpData,
	unsigned long int dwBufferSize,
	unsigned long int dwEventBase,

	unsigned long int* lpBytesConsumed,
	struct eitfileEITEvent_Entry** lpEventEntryOut
);


struct eitfileContentCategoryEntry {
	uint8_t				categoryId;
	char*				lpReadable;
};
extern struct eitfileContentCategoryEntry eitfileContentCategory[];



#ifdef __cplusplus
	} /* extern "C" */
#endif


#endif /* __is_included__555b8739_1220_4609_8f37_d639ae02000d */
