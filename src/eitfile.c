#include "../include/eitfile.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifdef DEBUG
	#include <stdio.h>
#endif

#ifdef __cplusplus
	extern "C" {
#endif

void eitfileReleaseDescriptor(
	struct eitfileDescriptor* lpTable
) {
	if(lpTable != NULL) {
		free(lpTable);
	}
}

void eitfileReleaseEvent(
	struct eitfileEITEvent_Entry* lpEvent
) {
	struct eitfileDescriptor* lpCur;
	struct eitfileDescriptor* lpNext;

	if(lpEvent == NULL) { return; }

	lpCur = lpEvent->descriptorList.lpFirst;
	while(lpCur != NULL) {
		lpNext = lpCur->descriptorList.lpNext;
		eitfileReleaseDescriptor(lpCur);
		lpCur = lpNext;
	}
	free(lpEvent);
}

enum eitfileError eitfileParse_Descriptor(
	unsigned char* lpData,
	unsigned long int dwBufferSize,
	unsigned long int dwCurrentBase,

	unsigned long int* lpBytesConsumed,
	struct eitfileDescriptor** lpOut
) {
	uint8_t bDescriptorType;
	uint8_t bDescriptorLength;
	struct eitfileDescriptor_ShortEventDescriptor* lpNewShortEventDesc;
	struct eitfileDescriptor_ExtendedEventDescriptor* lpNewExtendedEventDesc;
	struct eitfileDescriptor_ContentDescriptor* lpNewContentDescriptor;
	unsigned long int dwEventNameLength;
	unsigned long int dwTextLength;
	#if 0
		unsigned long int dwRelO;
	#endif
	unsigned long int dwItemizedBytesRemaining;
	unsigned long int dwByteSize;
	unsigned long int i;

	if(lpBytesConsumed != NULL) { (*lpBytesConsumed) = 0; }
	if(lpOut != NULL) { (*lpOut) = 0; }

	if((lpData == NULL) || (dwBufferSize == 0) || (dwCurrentBase >= dwBufferSize) || ((dwBufferSize - dwCurrentBase) < 2)) {
		return eitfileE_InvalidParam;
	}

	/* Get the buffer type */
	bDescriptorType = lpData[dwCurrentBase];
	bDescriptorLength = lpData[dwCurrentBase+1];

	switch(bDescriptorType) {
		case eitfileDescriptorType_StuffingDescriptor:
			/* we ignore stuffing descriptors */
			(*lpOut) = NULL;
			(*lpBytesConsumed) = 2+bDescriptorLength;
			return eitfileE_Ok;
		case eitfileDescriptorType_ShortEventDescriptor:
			if((dwBufferSize - dwCurrentBase) < 6) { return eitfileE_EncodingError; }
			dwEventNameLength = lpData[dwCurrentBase+5];
			if((dwBufferSize - dwCurrentBase) < 6+dwEventNameLength+1) { return eitfileE_EncodingError; }
			dwTextLength = lpData[dwCurrentBase+6+dwEventNameLength];
			if((dwBufferSize - dwCurrentBase) < 6+dwEventNameLength+1+dwTextLength) { return eitfileE_EncodingError; }

			lpNewShortEventDesc = (struct eitfileDescriptor_ShortEventDescriptor*)malloc(sizeof(struct eitfileDescriptor_ShortEventDescriptor)+dwTextLength+dwEventNameLength);
			if(lpNewShortEventDesc == NULL) { return eitfileE_OutOfMemory; }

			lpNewShortEventDesc->base.descriptorType 			= eitfileDescriptorType_ShortEventDescriptor;
			lpNewShortEventDesc->base.descriptorList.lpNext 	= NULL;
			lpNewShortEventDesc->base.descriptorList.lpPrev 	= NULL;
			lpNewShortEventDesc->langCode[0] 					= lpData[dwCurrentBase+2];
			lpNewShortEventDesc->langCode[1] 					= lpData[dwCurrentBase+3];
			lpNewShortEventDesc->langCode[2] 					= lpData[dwCurrentBase+4];
			lpNewShortEventDesc->dwEventNameLength 				= dwEventNameLength;
			lpNewShortEventDesc->dwTextLength 					= dwTextLength;
			if(dwEventNameLength > 0) {
				memcpy(lpNewShortEventDesc->bNameAndTextData, &(lpData[dwCurrentBase+6]), dwEventNameLength);
			}
			if(dwTextLength > 0) {
				memcpy(&(lpNewShortEventDesc->bNameAndTextData[dwEventNameLength]), &(lpData[dwCurrentBase+6+dwEventNameLength+1]), dwTextLength);
			}

			(*lpBytesConsumed) = 7+dwEventNameLength+dwTextLength;
			(*lpOut) = (struct eitfileDescriptor*)lpNewShortEventDesc;
			return eitfileE_Ok;

		case eitfileDescriptorType_ExtendedEventDescriptor:
			/* First determine sizes, then allocate and copy strings / initialize pointers */
			if((dwBufferSize - dwCurrentBase) < 7) { return eitfileE_EncodingError; }
			dwByteSize = sizeof(struct eitfileDescriptor_ExtendedEventDescriptor);
			#if 0
				dwRelO = 0;
			#endif
			if((lpData[dwCurrentBase+6]) > 0) {
				/* We have itemized elements */
				dwItemizedBytesRemaining = lpData[dwCurrentBase+6];
				if(dwBufferSize < 1+dwCurrentBase+dwItemizedBytesRemaining+7+1) { return eitfileE_EncodingError; }

				#if 0
					/* TODO: Note this is currently not supported, itemized entires are just skipped */
					while(dwItemizedBytesRemaining > 0) {
						/*
							Offset									Content
							7+dwRelO								item_descriptionlength
							7+dwRelO+1+item_descriptionlength		item_length
							7+dwRelO+1+item_descriptionlength+1+item_length -> next element
						*/
						if(dwItemizedBytesRemaining < 1+dwRelO) { return eitfileE_EncodingError; }
						if(dwItemizedBytesRemaining < (1+dwRelO+1+lpData[dwCurrentBase+7+dwRelO])) { return eitfileE_EncodingError; }
						dwByteSize = dwByteSize + lpData[dwCurrentBase+7+dwRelO] + lpData[dwCurrentBase+7+dwRelO+1+lpData[dwCurrentBase+7+dwRelO]] + sizeof(struct eitfileDescriptor_ExtendedEventDescriptor_Item);
						dwRelO = dwRelO + lpData[dwCurrentBase+7+dwRelO] + lpData[dwCurrentBase+7+dwRelO+1+lpData[dwCurrentBase+7+dwRelO]] + 2;
					}
					dwByteSize = dwByteSize + lpData[dwCurrentBase+7+dwRelO];
				#endif

				/* This offset now points at "text_length" */
				dwByteSize = dwByteSize + lpData[dwCurrentBase+7+dwItemizedBytesRemaining];
			} else {
				if(dwBufferSize < 1+dwCurrentBase+8) { return eitfileE_EncodingError; }
				dwByteSize = dwByteSize + lpData[dwCurrentBase+8];
			}

			lpNewExtendedEventDesc = (struct eitfileDescriptor_ExtendedEventDescriptor*)malloc(dwBufferSize);
			if(lpNewExtendedEventDesc == NULL) {
				 return eitfileE_OutOfMemory;
			}

			lpNewExtendedEventDesc->base.descriptorType = eitfileDescriptorType_ExtendedEventDescriptor;
			lpNewExtendedEventDesc->base.descriptorList.lpNext = NULL;
			lpNewExtendedEventDesc->base.descriptorList.lpPrev = NULL;
			lpNewExtendedEventDesc->dwDescriptorNumber = (lpData[dwCurrentBase+2] >> 4) & 0x0F;
			lpNewExtendedEventDesc->dwLastDescriptorNumber = (lpData[dwCurrentBase+2]) & 0x0F;
			lpNewExtendedEventDesc->langCode[0] = lpData[dwCurrentBase+3];
			lpNewExtendedEventDesc->langCode[1] = lpData[dwCurrentBase+4];
			lpNewExtendedEventDesc->langCode[2] = lpData[dwCurrentBase+5];

			if((lpData[dwCurrentBase+6]) > 0) {
				/* Now we are initializing the itemized tables one after another */
				/* TODO Itemized is currently not supported */
				dwItemizedBytesRemaining = lpData[dwCurrentBase+6];
				if(dwBufferSize < 1+dwCurrentBase+dwItemizedBytesRemaining+7+1) {
					free(lpNewExtendedEventDesc);
					return eitfileE_EncodingError;
				}

				/* And finally copy the non-itemized text */
				lpNewExtendedEventDesc->dwTextLength = lpData[dwCurrentBase+dwItemizedBytesRemaining+7];
				if(dwBufferSize < 1+dwCurrentBase+dwItemizedBytesRemaining+7+1+lpNewExtendedEventDesc->dwTextLength) {
					free(lpNewExtendedEventDesc);
					return eitfileE_EncodingError;
				}
				memcpy(lpNewExtendedEventDesc->bText, &(lpData[dwCurrentBase+dwItemizedBytesRemaining+8]), lpNewExtendedEventDesc->dwTextLength);
				lpNewExtendedEventDesc->lpText = lpNewExtendedEventDesc->bText;
				(*lpBytesConsumed) = 8+lpNewExtendedEventDesc->dwTextLength+dwItemizedBytesRemaining;
			} else {
				/* Only non itemized text */
				lpNewExtendedEventDesc->lpItemizedItems = 0;
				lpNewExtendedEventDesc->dwTextLength = lpData[dwCurrentBase+7];
				if(dwBufferSize < dwCurrentBase+7+lpNewExtendedEventDesc->dwTextLength) {
					free(lpNewExtendedEventDesc);
					return eitfileE_EncodingError;
				}
				memcpy(lpNewExtendedEventDesc->bText, &(lpData[dwCurrentBase+8]), lpNewExtendedEventDesc->dwTextLength);
				lpNewExtendedEventDesc->lpText = lpNewExtendedEventDesc->bText;
				(*lpBytesConsumed) = 8+lpNewExtendedEventDesc->dwTextLength;
			}
			(*lpOut) = (struct eitfileDescriptor*)lpNewExtendedEventDesc;
			return eitfileE_Ok;

		case eitfileDescriptorType_ContentDescriptor:
			/*
				Allows additional classifications
			*/
			if(bDescriptorLength % 2 != 0) {
				return eitfileE_EncodingError;
			}
			lpNewContentDescriptor = (struct eitfileDescriptor_ContentDescriptor*)malloc(sizeof(struct eitfileDescriptor_ContentDescriptor)+bDescriptorLength/2);
			if(lpNewContentDescriptor == NULL) {
				return eitfileE_OutOfMemory;
			}
			lpNewContentDescriptor->base.descriptorType = eitfileDescriptorType_ContentDescriptor;
			lpNewContentDescriptor->base.descriptorList.lpNext = NULL;
			lpNewContentDescriptor->base.descriptorList.lpPrev = NULL;
			lpNewContentDescriptor->dwClassCount = bDescriptorLength/2;
			for(i = 0; i < bDescriptorLength/2; i=i+1) {
				lpNewContentDescriptor->bClasses[i] = lpData[dwCurrentBase+2+2*i];
			}

			(*lpBytesConsumed) = 2+bDescriptorLength;
			(*lpOut) = (struct eitfileDescriptor*)lpNewContentDescriptor;
			return eitfileE_Ok;

		case eitfileDescriptorType_LinkageDescriptor:
		case eitfileDescriptorType_TimeShiftedEventDescriptor:
		case eitfileDescriptorType_ComponentDescriptor:
		case eitfileDescriptorType_CAIdentifierDescriptor:
		case eitfileDescriptorType_ParentalRatingDescriptor:
		case eitfileDescriptorType_TelephoneDescriptor:
		case eitfileDescriptorType_MultiligualComponentDescriptor:
		case eitfileDescriptorType_PrivateDataSpecifier:
		case eitfileDescriptorType_ShortSmoothingBufferDescriptor:
		case eitfileDescriptorType_DataBroadcastDescriptor:
		case eitfileDescriptorType_PDCDescriptor:
		case eitfileDescriptorType_TVAIDDescriptor:
		case eitfileDescriptorType_ContentIdentifierDescriptor:
		case eitfileDescriptorType_XAITLocationDescriptor:
		case eitfileDescriptorType_FTAContentManagementDescriptor:
		case eitfileDescriptorType_ExtensionDescriptor:
			(*lpOut) = NULL;
			(*lpBytesConsumed) = 2+bDescriptorLength;
			return eitfileE_Ok;
		default:
			(*lpOut) = NULL;
			(*lpBytesConsumed) = 2+bDescriptorLength;
			return eitfileE_UnknownDescriptorId;
	}
}

enum eitfileError eitfileParse_EIT_Event(
	unsigned char* lpData,
	unsigned long int dwBufferSize,
	unsigned long int dwEventBase,

	unsigned long int* lpBytesConsumed,
	struct eitfileEITEvent_Entry** lpEventEntryOut
) {
	enum eitfileError e;

	struct eitfileEITEvent_Entry* lpNewEvent = NULL;
	struct eitfileDescriptor* lpNextChildTable = NULL;
	struct eitfileDescriptor* lpTempTable;
	unsigned long int mjd;

	unsigned long int dwY;
	unsigned long int dwM;
	unsigned long int dwD;

	unsigned long int dwDescriptorsLoopLength;
	unsigned long int dwDescriptorBase;
	unsigned long int dwConsume;

	if(lpBytesConsumed != NULL) { (*lpBytesConsumed) = 0; }
	if(lpEventEntryOut != NULL) { (*lpEventEntryOut) = NULL; }

	if((lpData == NULL) || (dwBufferSize == 0) || (dwEventBase >= dwBufferSize) || ((dwBufferSize - dwEventBase) < 12) || (lpEventEntryOut == NULL)) {
		return eitfileE_InvalidParam;
	}

	/* Create base structure */
	lpNewEvent = (struct eitfileEITEvent_Entry*)malloc(sizeof(struct eitfileEITEvent_Entry));
	if(lpNewEvent == NULL) {
		return eitfileE_OutOfMemory;
	}

	lpNewEvent->flags = 0;
	lpNewEvent->descriptorList.lpFirst = NULL;
	lpNewEvent->descriptorList.lpLast = NULL;

	/* Decode */
	lpNewEvent->eventId = ((((uint16_t)(lpData[dwEventBase])) << 8) & 0xFF00) |
		((((uint16_t)(lpData[dwEventBase+1]))) & 0x00FF);
	/*
		40 bit start time:
			16 bit givint 16 LSB of MJD
			24 bits coded as 6 digits in BCD
	*/
	mjd = ((((uint16_t)(lpData[dwEventBase+2])) << 8) & 0xFF00) |
		((((uint16_t)(lpData[dwEventBase+3]))) & 0x00FF);
	dwY = (unsigned long int)((((double)mjd) - 15078.2) / 365.25);
	dwM = (unsigned long int)(((double)mjd - 14956.1 - floor((double)dwY * 365.25)) / 30.6001);
	dwD = mjd - 14956 - (unsigned long int)((double)dwY * 365.25) - (unsigned long int)((double)dwM * 30.6001);
	if((dwM == 14) || (dwM == 15)) {
		lpNewEvent->startTime.year = 1900 + dwY + 1;
		lpNewEvent->startTime.month = dwM - 13;
		lpNewEvent->startTime.day = dwD;
	} else {
		lpNewEvent->startTime.year = 1900 + dwY;
		lpNewEvent->startTime.month = dwM - 1;
		lpNewEvent->startTime.day = dwD;
	}
	lpNewEvent->startTime.hour = ((lpData[dwEventBase+4] & 0xF0) >> 4) * 10 + (lpData[dwEventBase+4] & 0x0F);
	lpNewEvent->startTime.minute = ((lpData[dwEventBase+5] & 0xF0) >> 4) * 10 + (lpData[dwEventBase+5] & 0x0F);
	lpNewEvent->startTime.second = ((lpData[dwEventBase+6] & 0xF0) >> 4) * 10 + (lpData[dwEventBase+6] & 0x0F);

	lpNewEvent->duration.hour = ((lpData[dwEventBase+7] & 0xF0) >> 4) * 10 + (lpData[dwEventBase+7] & 0x0F);
	lpNewEvent->duration.minute = ((lpData[dwEventBase+8] & 0xF0) >> 4) * 10 + (lpData[dwEventBase+8] & 0x0F);
	lpNewEvent->duration.second = ((lpData[dwEventBase+9] & 0xF0) >> 4) * 10 + (lpData[dwEventBase+9] & 0x0F);
	lpNewEvent->durationSeconds = lpNewEvent->duration.hour*3600 + lpNewEvent->duration.minute*60 + lpNewEvent->duration.second;

	switch((lpData[dwEventBase+10] & 0xE0) >> 5) {
		case eitRunningStatus_Undefined:			lpNewEvent->runningStatus = eitRunningStatus_Undefined; 		break;
		case eitRunningStatus_NotRunning:			lpNewEvent->runningStatus = eitRunningStatus_NotRunning; 		break;
		case eitRunningStatus_StartsFewSeconds:		lpNewEvent->runningStatus = eitRunningStatus_StartsFewSeconds; 	break;
		case eitRunningStatus_Pausing:				lpNewEvent->runningStatus = eitRunningStatus_Pausing; 			break;
		case eitRunningStatus_Running:				lpNewEvent->runningStatus = eitRunningStatus_Running; 			break;
		case eitRunningStatus_ServiceOffAir:		lpNewEvent->runningStatus = eitRunningStatus_ServiceOffAir; 	break;
		default:
			free(lpNewEvent);
			return eitfileE_EncodingError;
	}

	if((lpData[dwEventBase+10] & 0x01) != 0) { lpNewEvent->flags = lpNewEvent->flags | eitfileEITEvent_Entry__FLAGS__FREE_CA_MODE; }

	dwDescriptorsLoopLength = ((((uint16_t)(lpData[dwEventBase+10])) << 8) & 0x0F00) |
		((((uint16_t)(lpData[dwEventBase+11]))) & 0x00FF);

	if(lpBytesConsumed != NULL) { (*lpBytesConsumed) = 12; }
	dwDescriptorBase = 12;

	/* Process descriptors */
	while(dwDescriptorsLoopLength > 0) {
		e = eitfileParse_Descriptor(
			lpData,
			dwBufferSize,
			dwEventBase + dwDescriptorBase,
			&dwConsume,
			&lpNextChildTable
		);
		if(e != eitfileE_Ok) {
			lpNextChildTable = lpNewEvent->descriptorList.lpFirst;
			while(lpNextChildTable != NULL) {
				lpTempTable = lpNextChildTable->descriptorList.lpNext;
				eitfileReleaseDescriptor(lpNextChildTable);
				lpNextChildTable = lpTempTable;
			}
			free(lpNewEvent);
			return e;
		}

		dwDescriptorBase = dwDescriptorBase + dwConsume;
		dwDescriptorsLoopLength = dwDescriptorsLoopLength - dwConsume;
		if(lpBytesConsumed != NULL) { (*lpBytesConsumed) = (*lpBytesConsumed) + dwConsume; }

		if(lpNextChildTable != NULL) {
			/* Insert into our linked list */
			lpNextChildTable->descriptorList.lpNext = NULL;
			lpNextChildTable->descriptorList.lpPrev = lpNewEvent->descriptorList.lpLast;
			if(lpNewEvent->descriptorList.lpLast == NULL) {
				lpNewEvent->descriptorList.lpFirst = lpNextChildTable;
			} else {
				lpNewEvent->descriptorList.lpLast->descriptorList.lpNext = lpNextChildTable;
			}
			lpNewEvent->descriptorList.lpLast = lpNextChildTable;
		}
	}

	(*lpEventEntryOut) = lpNewEvent;
	return eitfileE_Ok;
}


struct eitfileContentCategoryEntry eitfileContentCategory[] = {
	{ 0x00, "undefined content" },
	{ 0x10, "movie/drama general" },
	{ 0x11, "detective/thriller" },
	{ 0x12, "ddventure/western/war" },
	{ 0x13, "science fiction/fantasy/horror" },
	{ 0x14, "comedy" },
	{ 0x15, "soap/melodrama/folkloric" },
	{ 0x16, "romance" },
	{ 0x17, "serious/classical/religious/historical movie/drama" },
	{ 0x20, "news/current affairs (general)" },
	{ 0x21, "news/weather report" },
	{ 0x22, "news magazine" },
	{ 0x23, "documentary" },
	{ 0x24, "discussion/interview/debate" },
	{ 0x30, "show/game show (general)" },
	{ 0x31, "game show/quiz/contest" },
	{ 0x32, "variety show" },
	{ 0x33, "talk show" },
	{ 0x40, "sports (general)" },
	{ 0x41, "special events (Olympic Games, World Cup, etc.)" },
	{ 0x42, "sports magazines" },
	{ 0x43, "football/soccer" },
	{ 0x44, "tennis/squash" },
	{ 0x45, "team sports (excluding football)" },
	{ 0x46, "athletics" },
	{ 0x47, "motor sport" },
	{ 0x48, "water sport" },
	{ 0x49, "winter sports" },
	{ 0x4A, "equestrian" },
	{ 0x4B, "martial sports" },
	{ 0x50, "children's/youth programmes (general)" },
	{ 0x51, "pre-school children's programmes" },
	{ 0x52, "entertainment programmes for 6 to14" },
	{ 0x53, "entertainment programmes for 10 to 16" },
	{ 0x54, "informational/educational/school programmes" },
	{ 0x55, "cartoons/puppets" },
	{ 0x60, "music/ballet/dance (general)" },
	{ 0x61, "rock/pop" },
	{ 0x62, "serious music/classical music" },
	{ 0x63, "folk/traditional music" },
	{ 0x64, "jazz" },
	{ 0x65, "musical/opera" },
	{ 0x66, "ballet" },
	{ 0x70, "arts/culture (without music, general)" },
	{ 0x71, "performing arts" },
	{ 0x72, "fine arts" },
	{ 0x73, "religion" },
	{ 0x74, "popular culture/traditional arts" },
	{ 0x75, "literature" },
	{ 0x76, "film/cinema" },
	{ 0x77, "experimental film/video" },
	{ 0x78, "broadcasting/press" },
	{ 0x79, "new media" },
	{ 0x7A, "arts/culture magazines" },
	{ 0x7B, "fashion" },
	{ 0x80, "social/political issues/economics (general)" },
	{ 0x81, "magazines/reports/documentary" },
	{ 0x82, "economics/social advisory" },
	{ 0x83, "remarkable people" },
	{ 0x90, "education/science/factual topics (general)" },
	{ 0x91, "nature/animals/environment" },
	{ 0x92, "technology/natural sciences" },
	{ 0x93, "medicine/physiology/psychology" },
	{ 0x94, "foreign countries/expeditions" },
	{ 0x95, "social/spiritual sciences" },
	{ 0x96, "further education" },
	{ 0x97, "languages" },
	{ 0xA0, "leisure hobbies (general)" },
	{ 0xA1, "tourism/travel" },
	{ 0xA2, "handicraft" },
	{ 0xA3, "motoring" },
	{ 0xA4, "fitness and health" },
	{ 0xA5, "cooking" },
	{ 0xA6, "advertisement/shopping" },
	{ 0xA7, "gardening" },
	{ 0xB0, "original language" },
	{ 0xB1, "black and white" },
	{ 0xB2, "unpublished" },
	{ 0xB3, "live broadcast" }
};


#ifdef __cplusplus
	} /* extern "C" { */
#endif
