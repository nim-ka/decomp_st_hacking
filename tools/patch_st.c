#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define ST_INJECT_RAM_ADDR 0x80400000

#define ST_RDRAM_OFFSET 0x1B0
#define ST_RDRAM(addr) (((addr) & ~0xF0000000) + ST_RDRAM_OFFSET)
#define RDRAM_ST(addr) (((addr) | 0x80000000) - ST_RDRAM_OFFSET)

#define ROM_RDRAM(addr) (((addr) - rom_entry + 0x1000) & ~0x80000000)

#define CLEANUP_AND_EXIT(code) exitCode = (code); goto cleanup

#define MAX_HOOKS 1024

struct HookInfo {
	bool began;
	char destName[256];
	unsigned long int destAddr;
	char srcName[256];
	unsigned long int srcAddr;
	unsigned long int destSize;
};

struct HookInfo hooks[MAX_HOOKS] = { 0 };

unsigned long int rom_entry;

char *progName = NULL;

static const struct option cli_options[] = {
	{ "help",      no_argument,          NULL, 'h' },
	{ "baserom",   required_argument,    NULL, 'b' },
	{ "rom",       required_argument,    NULL, 'r' },
	{ "in",        required_argument,    NULL, 'i' },
	{ "out",       required_argument,    NULL, 'o' },
	{ "hooks",     required_argument,    NULL, 'x' },
	{ "map",       required_argument,    NULL, 'm' },
	{ NULL,        no_argument,          NULL, 0   }
};

void usage(void) {
	fprintf(stderr,
		"Usage:\n"
		"\t%s [--help/-h]\n"
		"\t%s [--baserom/-b baserom.z64] [--rom/-r rom.z64] [--in/-i input.st] [--out/-o output.st] [--hooks/-x hooks.txt] [--map/-m map.map]\n"
		"\n"
		"This tool takes in two ROMs, an unmodified ROM and a newly built ROM that must only differ in src/custom (Expansion Pak RAM),\n"
		"and injects the changes in the new ROM and the hooks specified in the hook file into a given input savestate to give a new output savestate. The original input savestate file is not modified.\n"
		"The default base ROM file path is 'baserom.us.z64'. The default ROM file path is 'build/us/sm64.us.z64'. The default input savestate file path is 'basest.us.st'. The default output savestate file path is 'build/us/sm64.us.st'. The default hook file path is 'sm64_hooks.us.txt'. The default map file path is 'build/us/sm64.us.map'.\n"
		"\n"
		"The hook file format is as follows. Each non-empty line specifies a 'hook', which is a function in src/custom intended to overwrite a function somewhere else in the code. Each hook line consists of the name of the target function, then the name of the source function to be copied over, then the maximum copiable size of the target area, all space-separated.",
	progName, progName);
}

long int get_end_of_padded_file(FILE *file) {
	fseek(file, -1, SEEK_END);
	
	while (fgetc(file) == 0xFF) {
		fseek(file, -2, SEEK_CUR);
	}
	
	return ftell(file);
}

// Reverse endianness of a 4-byte buffer
void reverse_endianness(unsigned char *buf) {
	unsigned char byte0 = buf[0];
	unsigned char byte1 = buf[1];
	unsigned char byte2 = buf[2];
	unsigned char byte3 = buf[3];
	buf[0] = byte3;
	buf[1] = byte2;
	buf[2] = byte1;
	buf[3] = byte0;
}

int main(int argc, char **argv) {
	progName = argv[0];
	
	int exitCode = 0;
	
	unsigned char buf[4];
	
	char *baseromPath = "baserom.us.z64";
	char *romPath = "build/us/sm64.us.z64";
	char *inPath = "basest.us.st";
	char *outPath = "build/us/sm64.us.st";
	char *hooksPath = "sm64_hooks.us.txt";
	char *mapPath = "build/us/sm64.us.map";

	char curOpt;
	int optIndex;
	
	while ((curOpt = getopt_long(argc, argv, "hb:r:i:o:x:", cli_options, &optIndex)) != -1) {
		switch (curOpt) {
			case 'b':
				baseromPath = optarg;
				break;
			
			case 'r':
				romPath = optarg;
				break;
			
			case 'i':
				inPath = optarg;
				break;
			
			case 'o':
				outPath = optarg;
				break;
			
			case 'x':
				hooksPath = optarg;
				break;
			
			case 'm':
				mapPath = optarg;
				break;
			
			case 'h':
			case '?':
			default:
				usage();
				return 1;
		}
	}
	
	if (optind < argc) {
		fprintf(stderr, "%s: too many arguments\n", progName);
		usage();
		return 1;
	}
	
	printf(
		"Patching ST.\n"
		"Base ROM: %s\n"
		"ROM: %s\n"
		"Input ST: %s\n"
		"Output ST: %s\n"
		"Hook file: %s\n"
		"Map file: %s\n",
	baseromPath, romPath, inPath, outPath, hooksPath, mapPath);
	
	FILE *baseromFile = fopen(baseromPath, "rb");
	
	if (baseromFile == NULL) {
		fprintf(stderr, "%s: file %s could not be opened\n", progName, baseromPath);
		return 1;
	}
	
	FILE *romFile = fopen(romPath, "rb");
	
	if (romFile == NULL) {
		fprintf(stderr, "%s: file %s could not be opened\n", progName, romPath);
		fclose(baseromFile);
		return 1;
	}
	
	FILE *hooksFile = fopen(hooksPath, "r");
	
	if (hooksFile == NULL) {
		fprintf(stderr, "%s: file %s could not be opened\n", progName, hooksPath);
		fclose(baseromFile);
		fclose(romFile);
		return 1;
	}
	
	FILE *mapFile = fopen(mapPath, "r");
	
	if (mapFile == NULL) {
		fprintf(stderr, "%s: file %s could not be opened\n", progName, mapPath);
		fclose(baseromFile);
		fclose(romFile);
		fclose(hooksFile);
		return 1;
	}
	
	gzFile inFile = gzopen(inPath, "rb");
	
	if (inFile == Z_NULL) {
		fprintf(stderr, "%s: file %s could not be opened\n", progName, inPath);
		fclose(baseromFile);
		fclose(romFile);
		fclose(hooksFile);
		fclose(mapFile);
		return 1;
	}
	
	gzFile outFile = gzopen(outPath, "wb");
	
	if (outFile == Z_NULL) {
		fprintf(stderr, "%s: file %s could not be opened\n", progName, outPath);
		fclose(baseromFile);
		fclose(romFile);
		fclose(hooksFile);
		fclose(mapFile);
		gzclose(inFile);
		return 1;
	}
	
	fseek(romFile, 0x8, SEEK_SET);
	
	if (fread(buf, 1, 4, romFile) != 4) {
		fprintf(stderr, "%s: error reading from ROM file %s", progName, romPath);
		CLEANUP_AND_EXIT(1);
	}
	
	rom_entry =
		(buf[0] << 24) |
		(buf[1] << 16) |
		(buf[2] << 8) |
		buf[3];
	
	char lineBuf[256];
	char sizeBuf[256];
	
	int hookCount = 0;
	
	do {
		if (fgets(lineBuf, 256, hooksFile) == NULL) {
			if (feof(hooksFile)) {
				break;
			} else {
				fprintf(stderr, "%s: error reading from hook file %s\n", progName, hooksPath);
				CLEANUP_AND_EXIT(1);
			}
		}
		
		unsigned int len = strlen(lineBuf);
		
		if (len <= 1) {
			continue;
		}
		
		if (len >= 255) {
			fprintf(stderr, "%s: encountered a line longer than 256 characters in hook file %s\n", progName, hooksPath);
			CLEANUP_AND_EXIT(1);
		}
		
		if (hookCount == MAX_HOOKS) {
			fprintf(stderr, "%s: hook file has more than the maximum %d hooks\n", progName, MAX_HOOKS);
			CLEANUP_AND_EXIT(1);
		}
		
		if (sscanf(lineBuf, "%s %s %s\n", hooks[hookCount].destName, hooks[hookCount].srcName, sizeBuf) != 3) {
			fprintf(stderr,
				"%s: invalid line in hook file:\n"
				"\t%s",
			progName, lineBuf);
			CLEANUP_AND_EXIT(1);
		}
		
		unsigned long int size = strtoul(sizeBuf, NULL, 0);
		
		if (size == 0 || size % 4 != 0) {
			fprintf(stderr, "%s: invalid target size in hook file '%s'. The size must be a valid non-zero integer divisible by 4.\n", progName, sizeBuf);
			CLEANUP_AND_EXIT(1);
		}
		
		hooks[hookCount].destSize = size;
		
		hookCount++;
	} while (true);
	
	if (hookCount > 0) {
		do {
			if (fgets(lineBuf, 256, mapFile) == NULL) {
				if (feof(mapFile)) {
					break;
				} else {
					fprintf(stderr, "%s: error reading from map file %s\n", progName, mapPath);
					CLEANUP_AND_EXIT(1);
				}
			}
			
			unsigned int len = strlen(lineBuf);
			
			if (len <= 1 || len >= 255) {
				continue;
			}
			
			for (int i = 0; i < hookCount; i++) {
				char nameBuf[512];
				
				strcpy(nameBuf, hooks[i].destName);
				
				len = strlen(nameBuf);
				nameBuf[len] = '\n';
				nameBuf[len + 1] = '\0';
				
				if (strstr(lineBuf, nameBuf) != NULL) {
					unsigned long int addr = strtoul(lineBuf, NULL, 0);
					
					if (addr == 0) {
						fprintf(stderr,
							"%s: invalid address for hook target function %s in map file:\n"
							"\t%s",
						progName, hooks[i].destName, lineBuf);
						CLEANUP_AND_EXIT(1);
					}
					
					if (addr >= ST_INJECT_RAM_ADDR) {
						fprintf(stderr, "%s: hook target function %s must not come from custom segment\n", progName, hooks[i].destName);
						CLEANUP_AND_EXIT(1);
					}
					
					hooks[i].destAddr = addr;
					
					break;
				}
				
				strcpy(nameBuf, hooks[i].srcName);
				
				len = strlen(nameBuf);
				nameBuf[len] = '\n';
				nameBuf[len + 1] = '\0';
				
				if (strstr(lineBuf, nameBuf) != NULL) {
					unsigned long int addr = strtoul(lineBuf, NULL, 0);
					
					if (addr == 0) {
						fprintf(stderr,
							"%s: invalid address for hook source function %s in map file:\n"
							"\t%s",
						progName, hooks[i].srcName, lineBuf);
						CLEANUP_AND_EXIT(1);
					}
					
					if (addr < ST_INJECT_RAM_ADDR) {
						fprintf(stderr, "%s: hook source function %s must come from custom segment\n", progName, hooks[i].srcName);
						CLEANUP_AND_EXIT(1);
					}
					
					hooks[i].srcAddr = addr;
					
					break;
				}
			}
		} while (true);
		
		for (int i = 0; i < hookCount; i++) {
			if (hooks[i].destAddr == 0) {
				fprintf(stderr, "%s: no address found for hook target function %s in map file\n", progName, hooks[i].destName);
				CLEANUP_AND_EXIT(1);
			}
			
			if (hooks[i].srcAddr == 0) {
				fprintf(stderr, "%s: no address found for hook source function %s in map file\n", progName, hooks[i].srcName);
				CLEANUP_AND_EXIT(1);
			}
		}
		
		printf("Loaded hooks:\n");
		
		for (int i = 0; i < hookCount; i++) {
			printf("\t%s (0x%08lx) <-- %s (0x%08lx), max 0x%lx bytes\n", hooks[i].destName, hooks[i].destAddr, hooks[i].srcName, hooks[i].srcAddr, hooks[i].destSize);
		}
	}
	
	// Assert that all changes (besides checksum) in the new ROM occur in src/custom i.e. at the end of the ROM i.e. over all FF in the base ROM
	
	long int baseromEndOffset = get_end_of_padded_file(baseromFile);
	long int romEndOffset = get_end_of_padded_file(romFile);
	
	// Skip past ROM header
	fseek(baseromFile, 0x1000, SEEK_SET);
	fseek(romFile, 0x1000, SEEK_SET);
	
	long int offset;
	
	while ((offset = ftell(baseromFile)) < baseromEndOffset) {
		unsigned char ch1 = fgetc(baseromFile);
		unsigned char ch2 = fgetc(romFile);
		
		if (ch1 != ch2) {
			fprintf(stderr, "%s: found a difference between the base ROM and the new ROM outside of the expected area (byte at 0x%lx changed from %02x to %02x). Aborting.\n", progName, offset, ch1, ch2);
			CLEANUP_AND_EXIT(1);
		}
	}
	
	printf("Injecting from offset 0x%lx to offset 0x%lx\n", baseromEndOffset, romEndOffset);
	
	long int injectOffset = ST_RDRAM(ST_INJECT_RAM_ADDR);
	
	gzseek(inFile, 0, SEEK_SET);
	gzseek(outFile, 0, SEEK_SET);
	
	while (gztell(inFile) < injectOffset) {
		gzread(inFile, buf, 4);
		
		// Janky in-place hook writing
		
		unsigned int curPos = gztell(outFile);
		
		for (int i = 0; i < hookCount; i++) {
			// TODO: Make this not copy the entire dest size?
			unsigned long int destStart = hooks[i].destAddr;
			unsigned long int destEnd = hooks[i].destAddr + hooks[i].destSize;
			unsigned long int srcStart = hooks[i].srcAddr;
			
			if (destStart <= RDRAM_ST(curPos) && RDRAM_ST(curPos) < destEnd) {
				fseek(romFile, ROM_RDRAM(srcStart) + curPos - destStart, SEEK_SET);
				
				if (fread(buf, 1, 4, romFile) != 4) {
					fprintf(stderr, "%s: error reading from ROM file %s", progName, romPath);
					CLEANUP_AND_EXIT(1);
				}
				
				reverse_endianness(buf);
				
				if (!hooks[i].began) {
					printf("Began injecting hook #%d from 0x%08lx to 0x%08lx\n", i + 1, hooks[i].srcAddr, hooks[i].destAddr);
					hooks[i].began = true;
				}
			}
		}
		
		gzwrite(outFile, buf, 4);
	}
	
	fseek(romFile, baseromEndOffset, SEEK_SET);
	
	while (ftell(romFile) < romEndOffset) {
		if (fread(buf, 1, 4, romFile) != 4) {
			break;
		}
		
		reverse_endianness(buf);
		
		gzwrite(outFile, buf, 4);
		
		// Progress inFile as well
		gzseek(inFile, 4, SEEK_CUR);
	}
	
	while (gzread(inFile, buf, 4)) {
		gzwrite(outFile, buf, 4);
	}
	
cleanup:
	if (exitCode) {
		fprintf(stderr, "Savestate injection failed.\n");
	} else {
		printf("Savestate injection succeeded!\n");
	}
	
	fclose(baseromFile);
	fclose(romFile);
	fclose(hooksFile);
	fclose(mapFile);
	gzclose(inFile);
	gzclose(outFile);
	return exitCode;
}