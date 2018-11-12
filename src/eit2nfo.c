#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef WIN32
	#include <dirent.h>
#else
	#include <windows.h>
#endif

#include "../include/eitfile.h"

#ifdef __cplusplus
	extern "C" {
#endif

static uint32_t mappingISO8859_9[256] = {
	0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x000F,
	0x0010,0x0011,0x0012,0x0013,0x0014,0x0015,0x0016,0x0017,0x0018,0x0019,0x001A,0x001B,0x001C,0x001D,0x001E,0x001F,
	0x0020,0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,0x0028,0x0029,0x002A,0x002B,0x002C,0x002D,0x002E,0x002F,
	0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,0x0038,0x0039,0x003A,0x003B,0x003C,0x003D,0x003E,0x003F,
	0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,0x0048,0x0049,0x004A,0x004B,0x004C,0x004D,0x004E,0x004F,
	0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,0x0058,0x0059,0x005A,0x005B,0x005C,0x005D,0x005E,0x005F,
	0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,0x006E,0x006F,
	0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007A,0x007B,0x007C,0x007D,0x007E,0x007F,
	0x0080,0x0081,0x0082,0x0083,0x0084,0x0085,0x0086,0x0087,0x0088,0x0089,0x008A,0x008B,0x008C,0x008D,0x008E,0x008F,
	0x0090,0x0091,0x0092,0x0093,0x0094,0x0095,0x0096,0x0097,0x0098,0x0099,0x009A,0x009B,0x009C,0x009D,0x009E,0x009F,
	0x00A0,0x00A1,0x00A2,0x00A3,0x00A4,0x00A5,0x00A6,0x00A7,0x00A8,0x00A9,0x00AA,0x00AB,0x00AC,0x00AD,0x00AE,0x00AF,
	0x00B0,0x00B1,0x00B2,0x00B3,0x00B4,0x00B5,0x00B6,0x00B7,0x00B8,0x00B9,0x00BA,0x00BB,0x00BC,0x00BD,0x00BE,0x00BF,
	0x00C0,0x00C1,0x00C2,0x00C3,0x00C4,0x00C5,0x00C6,0x00C7,0x00C8,0x00C9,0x00CA,0x00CB,0x00CC,0x00CD,0x00CE,0x00CF,
	0x011E,0x00D1,0x00D2,0x00D3,0x00D4,0x00D5,0x00D6,0x00D7,0x00D8,0x00D9,0x00DA,0x00DB,0x00DC,0x0130,0x015E,0x00DF,
	0x00E0,0x00E1,0x00E2,0x00E3,0x00E4,0x00E5,0x00E6,0x00E7,0x00E8,0x00E9,0x00EA,0x00EB,0x00EC,0x00ED,0x00EE,0x00EF,
	0x011F,0x00F1,0x00F2,0x00F3,0x00F4,0x00F5,0x00F6,0x00F7,0x00F8,0x00F9,0x00FA,0x00FB,0x00FC,0x0131,0x015F,0x00FF
};

static unsigned long int eitGetStringBytelength(
	unsigned char* lpString,
	unsigned long int dwStringLen
) {
	unsigned long int dwResult;
	unsigned long int i;
	uint32_t* lpMappingTable_SingleByte = NULL;
	/*
		Check if we have a selection of character table
		as the first byte
	*/
	if(lpString[0] > 0x20) {
		/* Use default character table */
		dwResult = 0;
		for(i = 1; i < dwStringLen; i=i+1) {
			if((lpString[i] >= 0x80) && (lpString[i] <= 0x9F) && (lpString[i] != 0x8A)) { continue; } /* Skip control characters */
			if(lpString[i] == 0x8A) { dwResult = dwResult + 1; continue; } /* CRLF */
			dwResult = dwResult + 1;
		}
		return dwResult + 1;
	}
	if(lpString[0] == 0x15) {
		/* UTF-8 */
		dwResult = 0;
		for(i = 1; i < dwStringLen; i=i+1) {
			if((lpString[i] >= 0x80) && (lpString[i] <= 0x9F) && (lpString[i] != 0x8A)) { continue; } /* Skip control characters */
			if(lpString[i] == 0x8A) { dwResult = dwResult + 1; continue; } /* CRLF */
			dwResult = dwResult + 1;
		}
		return dwResult + 1;
	}

	if(lpString[0] == 0x05) { lpMappingTable_SingleByte = mappingISO8859_9; }





	if(lpMappingTable_SingleByte != NULL) {
		/* We have a single byte -> UTF8 Mapping */
		dwResult = 0;
		for(i = 1; i < dwStringLen; i=i+1) {
			if((lpString[i] >= 0x80) && (lpString[i] <= 0x9F) && (lpString[i] != 0x8A)) { continue; } /* Skip control characters */
			if(lpString[i] == 0x8A) { dwResult = dwResult + 1; continue; } /* CRLF */

			if(lpMappingTable_SingleByte[lpString[i]] < 0x7F) { dwResult = dwResult + 1; }
			else if(lpMappingTable_SingleByte[lpString[i]] < 0x7FF) { dwResult = dwResult + 2; }
			else if(lpMappingTable_SingleByte[lpString[i]] < 0xFFF) { dwResult = dwResult + 3; }
			else { dwResult = dwResult + 4; }
		}
		return dwResult + 1;
	}
	return 0;
}

static int eitCopyString(
	unsigned char* lpString,
	unsigned long int dwStringLenIn,

	unsigned char* lpBufferOut,
	unsigned long int dwBufferOutLen
) {
	unsigned long int i;
	unsigned long int j = 0;
	uint32_t* lpMappingTable_SingleByte = NULL;

	/* Write a NUL terminated UTF-8 string into the output buffer */
	if(dwBufferOutLen < eitGetStringBytelength(lpString, dwStringLenIn)) {
		return 255;
	}

	if(lpString[0] > 0x20) {
		/* Use default character table */
		for(i = 1; i < dwStringLenIn; i=i+1) {
			if((lpString[i] >= 0x80) && (lpString[i] <= 0x9F) && (lpString[i] != 0x8A)) { continue; } /* Skip control characters */
			if(lpString[i] == 0x8A) { lpBufferOut[j] = '\n'; j=j+1; continue; } /* CRLF */
			lpBufferOut[j] = lpString[i];
			j = j + 1;
		}
		lpBufferOut[j] = 0x00;
		return 0;
	}
	if(lpString[0] == 0x15) {
		/* UTF-8 */
		for(i = 1; i < dwStringLenIn; i=i+1) {
			if((lpString[i] >= 0x80) && (lpString[i] <= 0x9F) && (lpString[i] != 0x8A)) { continue; } /* Skip control characters */
			if(lpString[i] == 0x8A) { lpBufferOut[j] = '\n'; j=j+1; continue; } /* CRLF */
			lpBufferOut[j] = lpString[i];
			j = j + 1;
		}
		lpBufferOut[j] = 0x00;
		return 0;
	}

	if(lpString[0] == 0x05) { lpMappingTable_SingleByte = mappingISO8859_9; }

	if(lpMappingTable_SingleByte != NULL) {
		/* We have a single byte -> UTF8 Mapping */
		for(i = 1; i < dwStringLenIn; i=i+1) {
			if((lpString[i] >= 0x80) && (lpString[i] <= 0x9F) && (lpString[i] != 0x8A)) { continue; } /* Skip control characters */
			if(lpString[i] == 0x8A) { lpBufferOut[j] = '\n'; j = j + 1; } /* CRLF */

			if(lpMappingTable_SingleByte[lpString[i]] < 0x7F) {
				lpBufferOut[j] = (unsigned char)(lpMappingTable_SingleByte[lpString[i]]);
				j = j + 1;
			} else if(lpMappingTable_SingleByte[lpString[i]] < 0x7FF) {
				lpBufferOut[j+1] = (unsigned char)((lpMappingTable_SingleByte[lpString[i]]) & 0x0000003F) | 0x80;
				lpBufferOut[j] = (unsigned char)(((lpMappingTable_SingleByte[lpString[i]]) >> 6) & 0x0000001F) | 0xC0;
				j = j + 2;
			} else if(lpMappingTable_SingleByte[lpString[i]] < 0xFFF) {
				lpBufferOut[j+2] = (unsigned char)((lpMappingTable_SingleByte[lpString[i]]) & 0x0000003F) | 0x80;
				lpBufferOut[j+1] = (unsigned char)(((lpMappingTable_SingleByte[lpString[i]]) >> 6) & 0x0000003F) | 0x80;
				lpBufferOut[j] = (unsigned char)(((lpMappingTable_SingleByte[lpString[i]]) >> 12) & 0x0000000F) | 0xE0;
				j = j + 3;
			} else {
				lpBufferOut[j+3] = (unsigned char)((lpMappingTable_SingleByte[lpString[i]]) & 0x0000003F) | 0x80;
				lpBufferOut[j+2] = (unsigned char)(((lpMappingTable_SingleByte[lpString[i]]) >> 6) & 0x0000003F) | 0x80;
				lpBufferOut[j+1] = (unsigned char)(((lpMappingTable_SingleByte[lpString[i]]) >> 12) & 0x0000003F) | 0x80;
				lpBufferOut[j] = (unsigned char)(((lpMappingTable_SingleByte[lpString[i]]) >> 18) & 0x00000007) | 0xF0;
				j = j + 4;
			}
		}
		lpBufferOut[j] = 0x00;
		return 0;
	}
	return -1;
}

static int processEITFile_CreateNFO(
	struct eitfileEITEvent_Entry* lpEIT,
	char* lpNFOFilename
) {
	FILE* fHandleOut;
	struct eitfileDescriptor* lpCurDesc;
	unsigned long int dwLen, dwLen2, j;
	unsigned long int dwTitleLen;
	unsigned long int dwOutlineLen;

	unsigned char* lpTitle = NULL;
	unsigned char* lpOutline = NULL;

	/*
		Check if we have located an title (normally found in
		an "short" description element
	*/
	lpCurDesc = lpEIT->descriptorList.lpFirst;
	while(lpCurDesc != NULL) {
		if(lpCurDesc->descriptorType == eitfileDescriptorType_ShortEventDescriptor) {
			if(lpTitle != NULL) {
				printf("%s:%u [WARN] Duplicate short descriptor found (possibly unsupported multi-language?)\n", __FILE__, __LINE__);
			} else {
				dwLen = eitGetStringBytelength(((struct eitfileDescriptor_ShortEventDescriptor*)lpCurDesc)->bNameAndTextData, ((struct eitfileDescriptor_ShortEventDescriptor*)lpCurDesc)->dwEventNameLength);
				if(dwLen <= 0) {
					printf("%s:%u [WARN] Failed to decode short event descriptor - unknown encoding?)\n", __FILE__, __LINE__);
					continue;
				}
				lpTitle = (unsigned char*)malloc(dwLen);
				dwTitleLen = dwLen-1;
				if(lpTitle == NULL) {
					printf("%s:%u [ERROR] Out of memory\n", __FILE__, __LINE__);
					return 2;
				}
				if(eitCopyString(((struct eitfileDescriptor_ShortEventDescriptor*)lpCurDesc)->bNameAndTextData, ((struct eitfileDescriptor_ShortEventDescriptor*)lpCurDesc)->dwEventNameLength, lpTitle, dwLen) != 0) {
					printf("%s:%u [ERROR] String conversion problem\n", __FILE__, __LINE__);
					free(lpTitle);
					return 255;
				}
			}
		}
		lpCurDesc = lpCurDesc->descriptorList.lpNext;
	}

	/*
		Check if we have an outline or additional information.
		This may be scattered over multiple elements ...
	*/
	dwLen = 0;
	lpCurDesc = lpEIT->descriptorList.lpFirst;
	while(lpCurDesc != NULL) {
		if(lpCurDesc->descriptorType == eitfileDescriptorType_ExtendedEventDescriptor) {
			/*
				Currently we do NOT reorder them if they are not in order but simply concat
				all non itemized texts
			*/
			dwLen2 = eitGetStringBytelength(((struct eitfileDescriptor_ExtendedEventDescriptor*)lpCurDesc)->lpText, ((struct eitfileDescriptor_ExtendedEventDescriptor*)lpCurDesc)->dwTextLength);
			/*
				if(dwLen2 < 0) {
					printf("%s:%u [WARN] Failed to decode extended event descriptor - ignoring part\n", __FILE__, __LINE__);
					continue;
				}
			*/
			dwLen = dwLen + dwLen2;
		}
		lpCurDesc = lpCurDesc->descriptorList.lpNext;
	}
	if(dwLen > 0) {
		j = 0;
		lpOutline = (unsigned char*)malloc(dwLen);
		dwOutlineLen = dwLen - 1;
		if(lpOutline == NULL) {
			printf("%s:%u [ERROR] Out of memory\n", __FILE__, __LINE__);
			return 2;
		}
		lpCurDesc = lpEIT->descriptorList.lpFirst;
		while(lpCurDesc != NULL) {
			if(lpCurDesc->descriptorType == eitfileDescriptorType_ExtendedEventDescriptor) {
				dwLen2 = eitGetStringBytelength(((struct eitfileDescriptor_ExtendedEventDescriptor*)lpCurDesc)->lpText, ((struct eitfileDescriptor_ExtendedEventDescriptor*)lpCurDesc)->dwTextLength);
				if(dwLen2 <= 0) { continue; }
				if(eitCopyString(((struct eitfileDescriptor_ExtendedEventDescriptor*)lpCurDesc)->lpText, ((struct eitfileDescriptor_ExtendedEventDescriptor*)lpCurDesc)->dwTextLength, &(lpOutline[j]), dwLen-j) != 0) {
					printf("%s:%u [ERROR] String conversion problem\n", __FILE__, __LINE__);
					return 255;
				}
				j = j + dwLen2 - 1;
			}
			lpCurDesc = lpCurDesc->descriptorList.lpNext;
		}
	}

	/* If we have at least an title we generate the NFO */
	if(lpTitle != NULL) {
		fHandleOut = fopen(lpNFOFilename, "wb");
		if(!fHandleOut) {
			printf("%s:%u [ERROR] Failed to create or overwrite %s\n", __FILE__, __LINE__, lpNFOFilename);
			if(lpTitle != NULL) 	{ free(lpTitle); 	}
			if(lpOutline != NULL) 	{ free(lpOutline); 	}
			return 1;
		}

		if(fwrite("<movie>\n", strlen("<movie>\n"), 1, fHandleOut) != 1) {
			fclose(fHandleOut);
			printf("%s:%u [ERROR] Failed write into %s\n", __FILE__, __LINE__, lpNFOFilename);
			if(lpTitle != NULL) 	{ free(lpTitle); 	}
			if(lpOutline != NULL) 	{ free(lpOutline); 	}
			return 1;
		}
		if(lpTitle != NULL) {
			if(fwrite("\t<title>", strlen("\t<title>"), 1, fHandleOut) != 1) {
				fclose(fHandleOut);
				printf("%s:%u [ERROR] Failed write into %s\n", __FILE__, __LINE__, lpNFOFilename);
				if(lpTitle != NULL) 	{ free(lpTitle); 	}
				if(lpOutline != NULL) 	{ free(lpOutline); 	}
				return 1;
			}
			if(fwrite(lpTitle, dwTitleLen, 1, fHandleOut) != 1) {
				fclose(fHandleOut);
				printf("%s:%u [ERROR] Failed write into %s\n", __FILE__, __LINE__, lpNFOFilename);
				if(lpTitle != NULL) 	{ free(lpTitle); 	}
				if(lpOutline != NULL) 	{ free(lpOutline); 	}
				return 1;
			}
			if(fwrite("</title>\n", strlen("</title>\n"), 1, fHandleOut) != 1) {
				fclose(fHandleOut);
				printf("%s:%u [ERROR] Failed write into %s\n", __FILE__, __LINE__, lpNFOFilename);
				if(lpTitle != NULL) 	{ free(lpTitle); 	}
				if(lpOutline != NULL) 	{ free(lpOutline); 	}
				return 1;
			}
		}
		if(lpOutline != NULL) {
			if(fwrite("\t<outline>", strlen("\t<outline>"), 1, fHandleOut) != 1) {
				fclose(fHandleOut);
				printf("%s:%u [ERROR] Failed write into %s\n", __FILE__, __LINE__, lpNFOFilename);
				if(lpTitle != NULL) 	{ free(lpTitle); 	}
				if(lpOutline != NULL) 	{ free(lpOutline); 	}
				return 1;
			}
			if(fwrite(lpOutline, dwOutlineLen, 1, fHandleOut) != 1) {
				fclose(fHandleOut);
				printf("%s:%u [ERROR] Failed write into %s\n", __FILE__, __LINE__, lpNFOFilename);
				if(lpTitle != NULL) 	{ free(lpTitle); 	}
				if(lpOutline != NULL) 	{ free(lpOutline); 	}
				return 1;
			}
			if(fwrite("</outline>\n", strlen("</outline>\n"), 1, fHandleOut) != 1) {
				fclose(fHandleOut);
				printf("%s:%u [ERROR] Failed write into %s\n", __FILE__, __LINE__, lpNFOFilename);
				if(lpTitle != NULL) 	{ free(lpTitle); 	}
				if(lpOutline != NULL) 	{ free(lpOutline); 	}
				return 1;
			}
		}
		if(fwrite("</movie>\n", strlen("</movie>\n"), 1, fHandleOut) != 1) {
			fclose(fHandleOut);
			printf("%s:%u [ERROR] Failed write into %s\n", __FILE__, __LINE__, lpNFOFilename);
			if(lpTitle != NULL) 	{ free(lpTitle); 	}
			if(lpOutline != NULL) 	{ free(lpOutline); 	}
			return 1;
		}
		fclose(fHandleOut);
	}

	if(lpTitle != NULL) 	{ free(lpTitle); 	}
	if(lpOutline != NULL) 	{ free(lpOutline); 	}
	return 0;
}

static int processEITFile(
	char* lpFilename,
	char* lpNFOFilename
) {
	FILE* fHandle;
	long fileSize;
	unsigned char* lpFileBuffer;
	unsigned long int dwCurrentOffset;
	unsigned long int dwConsumed;
	struct eitfileEITEvent_Entry* lpNextEventEntry;
	enum eitfileError e;
	int r;

	fHandle = fopen(lpFilename, "rb");
	if(!fHandle) {
		printf("%s:%u [ERROR] Failed to open file\n", __FILE__, __LINE__);
		return 1;
	}

	/* Determine file size */
	fseek(fHandle, 0, SEEK_END);
	fileSize = ftell(fHandle);
	fseek(fHandle, 0, SEEK_SET);

	/* Allocate memory (we read the whole file into memory) */
	lpFileBuffer = (unsigned char*)malloc(sizeof(char)*fileSize);
	if(lpFileBuffer == NULL) {
		printf("%s:%u [ERROR] Out of memory\n", __FILE__, __LINE__);
		fclose(fHandle);
		return 2;
	}

	/* Read file into memory */
	if(fread(lpFileBuffer, 1, fileSize, fHandle) != fileSize) {
		printf("%s:%u [ERROR] Loading %s into memory failed\n", __FILE__, __LINE__, lpFilename);
		fclose(fHandle);
		return 3;
	}
	fclose(fHandle);

	/* Parse EIT file */
	dwCurrentOffset = 0;
	while(dwCurrentOffset < fileSize) {
		e = eitfileParse_EIT_Event(
			lpFileBuffer,
			fileSize,
			dwCurrentOffset,
			&dwConsumed,
			&lpNextEventEntry
		);
		if(e != eitfileE_Ok) {
			printf("%s:%u Parsing failed with code %u\n", __FILE__, __LINE__, e);
			break;
		}
		dwCurrentOffset = dwCurrentOffset + dwConsumed;

		/*
			Process event entry

			Currently we collect title and description
			as well as genre
		*/
		if(lpNextEventEntry != NULL) {
			r = processEITFile_CreateNFO(lpNextEventEntry, lpNFOFilename);
			eitfileReleaseEvent(lpNextEventEntry);

			/* Currently only process a single event as most software only writes a single one */
			free(lpFileBuffer);
			return r;
		}
	}
	free(lpFileBuffer);

	return 0;
}

static void printUsage(int argc, char* argv[]) {
	printf("%s usage:\n\n", argv[0]);
	printf("\tConvert a single EIT:\n\t\t%s [-o] filename.eit [filename.nfo]\n\n\t\t\tIf the NFO filename is not supplied the EIT file extension is replaced with NFO\n", argv[0]);
	printf("\tConvert all EITs in a directory tree\n\t\t%s [-o] path\n\n\t\t\tIf the NFO filename is not supplied the EIT file extension is replaced with NFO\n", argv[0]);
	printf("\n\tOptions:\n\t\t-o\tOverwrite existing NFOs even if the NFO is newer than the EIT\n");
}

static int directoryIterate(
	char* lpPath,
	int bOverwrite
) {
	struct stat					statSource;
	struct stat					statDestination;

	#ifndef WIN32
		DIR* lpDirectory;
		struct dirent* lpDirEntry;	/* Note: The man page means we are not allowed to free this structure */
		long int dwMaxPath;
		char* lpConcatFilename;
		char* lpRealPath;
		char* lpNFOFilename;
		int r;

		dwMaxPath = pathconf(lpPath, _PC_PATH_MAX)+1;
		lpConcatFilename = (char*)malloc(dwMaxPath);
		if(lpConcatFilename == NULL) {
			printf("%s:%u [ERROR] Out of memory allocating memory for filenames\n", __FILE__, __LINE__);
			return 2;
		}

		if((lpRealPath = realpath(lpPath, NULL)) == NULL) {
			printf("%s:%u [ERROR] Failed to resolve full path for %s\n", __FILE__, __LINE__, lpPath);
			free(lpConcatFilename);
			return 2;
		}

		lpDirectory = opendir(lpPath);
		if(lpDirectory == NULL) {
			printf("%s:%u [ERROR] Failed to open %s for iteration\n", __FILE__, __LINE__, lpPath);
			free(lpRealPath);
			free(lpConcatFilename);
			return 1;
		}

		for(;;) {
			lpDirEntry = readdir(lpDirectory);
			if(lpDirEntry == NULL) {
				break;
			}

			/* Do the same as for a single file - create the NFO filename, check modification timestamps */

			if(lpDirEntry->d_type == DT_DIR) {
				if((strcmp(lpDirEntry->d_name, ".") == 0) || (strcmp(lpDirEntry->d_name, "..") == 0)) {
					continue;
				}

				if((strlen(lpDirEntry->d_name)+strlen(lpRealPath)+2) > dwMaxPath) {
					printf("%s:%u [ERROR] Path for %s/%s too long - skipping\n", __FILE__, __LINE__, lpRealPath, lpDirEntry->d_name);
					continue;
				}

				memset(lpConcatFilename, 0, dwMaxPath);
				strcpy(lpConcatFilename, lpRealPath);
				strcat(lpConcatFilename, "/");
				strcat(lpConcatFilename, lpDirEntry->d_name);

				directoryIterate(lpConcatFilename, bOverwrite);
			} else if(lpDirEntry->d_type == DT_REG) {
				if((strlen(lpDirEntry->d_name)+strlen(lpRealPath)+2) > dwMaxPath) {
					printf("%s:%u [ERROR] Path for %s/%s too long - skipping\n", __FILE__, __LINE__, lpRealPath, lpDirEntry->d_name);
					continue;
				}

				memset(lpConcatFilename, 0, dwMaxPath);
				strcpy(lpConcatFilename, lpRealPath);
				strcat(lpConcatFilename, "/");
				strcat(lpConcatFilename, lpDirEntry->d_name);

				if(strlen(lpConcatFilename) < 4) {
					continue;
				}
				if(strcmp(&(lpConcatFilename[strlen(lpConcatFilename)-4]), ".eit") != 0) {
					continue;
				}

				lpNFOFilename = malloc(strlen(lpConcatFilename)+1);
				if(lpNFOFilename == NULL) {
					printf("%s:%u [ERROR] Out of memory\n", __FILE__, __LINE__);
					free(lpConcatFilename);
					free(lpRealPath);
					closedir(lpDirectory);
					return 1;
				}
				strcpy(lpNFOFilename, lpConcatFilename);
				lpNFOFilename[strlen(lpNFOFilename)-3] = 'n';
				lpNFOFilename[strlen(lpNFOFilename)-2] = 'f';
				lpNFOFilename[strlen(lpNFOFilename)-1] = 'o';
				lpNFOFilename[strlen(lpNFOFilename)] = 0x00;

				/* Stat both */
				if(stat(lpConcatFilename, &statSource) != 0) {
					printf("%s:%u [ERROR] Failed to stat file %s - skipping\n", __FILE__, __LINE__, lpConcatFilename);
					continue;
				}
				if(stat(lpNFOFilename, &statDestination) == 0) {
					/* Maybe we skip because it's newer */
					if((statDestination.st_mtime >= statSource.st_mtime) && (bOverwrite == 0)) {
						free(lpNFOFilename);
						continue;
					}
				}

				if((r = processEITFile(lpConcatFilename, lpNFOFilename)) != 0) {
					printf("%s:%u [ERROR] Code: %u, failed to transform %s -> %s\n", __FILE__, __LINE__, r, lpConcatFilename, lpNFOFilename);
				}
				free(lpNFOFilename);
			}
		}

		free(lpRealPath);
		free(lpConcatFilename);
		closedir(lpDirectory);
		return 0;
	#else
		HANDLE 				hFind;
		WIN32_FIND_DATAA 	fData;
		char				lpDirectoryPath[MAX_PATH+3];
		char				lpConcatName[MAX_PATH+3];
		char				lpNFOFilename[MAX_PATH+3];
		int 				r;

		if(strlen(lpPath) > (MAX_PATH-3)) {
			printf("%s:%u [WARNING] Path %s too long - ignoring\n", __FILE__, __LINE__, lpPath);
			return 1;
		}

		memset(lpDirectoryPath, 0, MAX_PATH+1);
		memset(lpConcatName, 0, MAX_PATH+1);

		GetFullPathName(lpPath, MAX_PATH, lpDirectoryPath, NULL);
		if(strlen(lpDirectoryPath) > (MAX_PATH-3)) {
			printf("%s:%u [WARNING] Path %s too long - ignoring\n", __FILE__, __LINE__, lpDirectoryPath);
			return 1;
		}
		strcpy(lpConcatName, lpDirectoryPath);
		if(lpConcatName[strlen(lpConcatName)-1] != '\\') {
			strcat(lpConcatName, "\\*");
		} else {
			strcat(lpConcatName, "*");
		}
		hFind = FindFirstFile(lpConcatName, &fData);
		if(hFind == INVALID_HANDLE_VALUE) {
			printf("%s:%u [WARNING] Failed to iterate over %s - ignoring (Error %u)\n", __FILE__, __LINE__, lpDirectoryPath, (unsigned int)GetLastError());
			return 1;
		}

		do {
			if((strcmp(fData.cFileName, ".") != 0) && (strcmp(fData.cFileName, "..") != 0)) {
				if((strlen(fData.cFileName) + strlen(lpDirectoryPath)) > MAX_PATH) {
					FindClose(hFind);
					printf("%s:%u [WARNING] Path %s%s too long - ignoring\n", __FILE__, __LINE__, lpDirectoryPath, fData.cFileName);
					return 1;
				}
				memset(lpConcatName, 0, MAX_PATH+1);
				strcpy(lpConcatName, lpDirectoryPath);
				strcat(lpConcatName, fData.cFileName);

				if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					strcat(lpConcatName, "\\");
					directoryIterate(lpConcatName, bOverwrite);
				} else {
					if(strlen(lpConcatName) < 4) {
						continue;
					}

					if(strcmp(&(lpConcatName[strlen(lpConcatName)-4]), ".eit") != 0) {
						continue;
					}

					/*
						We have found an EIT file name. Generate the NFO filename
						and check if it does exist / is older than the EIT
					*/
					strcpy(lpNFOFilename, lpConcatName);
					lpNFOFilename[strlen(lpConcatName)-3] = 'n';
					lpNFOFilename[strlen(lpConcatName)-2] = 'f';
					lpNFOFilename[strlen(lpConcatName)-1] = 'o';

					/* Stat both */
					if(stat(lpConcatName, &statSource) != 0) {
						printf("%s:%u [ERROR] Failed to stat file %s - skipping\n", __FILE__, __LINE__, lpConcatName);
						continue;
					}
					if(stat(lpNFOFilename, &statDestination) == 0) {
						/* Maybe we skip because it's newer */
						if((statDestination.st_mtime >= statSource.st_mtime) && (bOverwrite == 0)) {
							continue;
						}
					}

					if((r = processEITFile(lpConcatName, lpNFOFilename)) != 0) {
						printf("%s:%u [ERROR] Code: %u, failed to transform %s -> %s\n", __FILE__, __LINE__, r, lpConcatName, lpNFOFilename);
					}
				}
			}
		} while(FindNextFile(hFind, &fData) != 0);
		FindClose(hFind);
		return 0;
	#endif
}

int main(int argc, char* argv[]) {
	char* lpPath 				= NULL;
	char* lpOriginalFile 		= NULL;
	char* lpDestinationFile 	= NULL;
	int flagOverwrite 			= 0;

	unsigned long int 			i;

	struct stat					statSource;
	struct stat					statDestination;

	if(argc < 2) { printUsage(argc, argv); return 1; }

	/*
		Process command line options
	*/
	for(i = 1; i < argc; i=i+1) {
		if(strcmp(argv[i], "-o") == 0) {
			flagOverwrite = 1;
			continue;
		}

		/* No known option, try as first or second filename / pathname */
		if((lpOriginalFile == NULL) && (lpPath == NULL)) {
			if(stat(argv[i], &statSource) != 0) {
				printf("%s:%u [ERROR] Failed to stat file or directory %s\n", __FILE__, __LINE__, argv[i]);
				return 2;
			}
			if(S_ISDIR(statSource.st_mode)) {
				lpPath = argv[i];
			} else {
				lpOriginalFile = argv[i];
			}
			continue;
		}
		if(lpPath != NULL) {
			printf("%s:%u [ERROR]. Source was an directory (%s). Destination cannot be specified in this case\n", __FILE__, __LINE__, lpPath);
			return 1;
		} else if(lpDestinationFile != NULL) {
			printf("%s:%u [ERROR] Excessive argument %s\n", __FILE__, __LINE__, argv[i]);
			return 1;
		}
		lpDestinationFile = argv[i];
	}
	if((lpOriginalFile != NULL) && (lpDestinationFile == NULL)) {
		/* generate destination name ... only works if original file extension is ".eit" */
		if(strlen(lpOriginalFile) < 4) {
			printf("%s:%u [ERROR] Can only autogenerate target filename for files with .eit extension, not for %s\n", __FILE__, __LINE__, lpOriginalFile);
			return 1;
		}
		if(strcmp(&(lpOriginalFile[strlen(lpOriginalFile)-4]), ".eit") != 0) {
			printf("%s:%u [ERROR] Can only autogenerate target filename for files with .eit extension, not for %s\n", __FILE__, __LINE__, lpOriginalFile);
			return 1;
		}
		lpDestinationFile = (char*)malloc(strlen(lpOriginalFile)+1);
		if(lpDestinationFile == NULL) {
			printf("%s:%u [ERROR] Out of memory\n", __FILE__, __LINE__);
			return 2;
		}
		memcpy(lpDestinationFile, lpOriginalFile, strlen(lpOriginalFile));
		lpDestinationFile[strlen(lpOriginalFile)-3] = 'n';
		lpDestinationFile[strlen(lpOriginalFile)-2] = 'f';
		lpDestinationFile[strlen(lpOriginalFile)-1] = 'o';
		lpDestinationFile[strlen(lpOriginalFile)] = 0x00;
	}

	if((lpOriginalFile != NULL) && (lpDestinationFile != NULL)) {
		/* Check if both files exist. Stat for original has already happened */
		if(stat(lpDestinationFile, &statDestination) != 0) {
			/* If stat fails we simply try to overwrite */
			return processEITFile(lpOriginalFile, lpDestinationFile);
		}

		/*
			If destination is NEWER or SAME age as source don't overwrite
			except the flag has been specified
		*/
		if((statDestination.st_mtime < statSource.st_mtime) || (flagOverwrite != 0)) {
			return processEITFile(lpOriginalFile, lpDestinationFile);
		}
		return 0;
	}

	/* Else we have specified a full path and should iterate over all EIT's in them ... */
	return directoryIterate(lpPath, flagOverwrite);
}


#ifdef __cplusplus
	} /* extern "C" { */
#endif
