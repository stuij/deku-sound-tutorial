// ----- Change log -----
// 10/08/04 - Changed extern sample data declarations to s8[] 
//			  instead of s8* (compiler didn't like the pointers). 
//			  SAMPLE_HEADER now stores a pointer to smpData instead of an 
//			  index, so dSmpDataTable was no longer used, so I removed it.
// ----------------------

#include <dirent.h>
#include <stdio.h>  // for file IO
#include <stdlib.h> // for stuff
#include <string.h>
#include <termios.h>
#include <unistd.h>

    // These are just to decide what size of pointer arrays to make at the start
	// The only reason not to make them huge is so we don't waste too much 
	// memory on unused pointers, but you can make them as big as you want
#define MAX_SONGS			1024
#define MAX_SFX				16384
#define MAX_SAMPLE_DATAS	16384

	// Define the flag to search for in the MOD name
#define MOD_SFX_FLAG		'*'

	// Invalid index for sample headers to use
#define INVALID_SMP_DATA	0xffffffff

	// For patterns to specify that there is no note. We have 5 octaves, 
	// so only notes 0-59 are used, and 63 is the highest that still fits 
	// in the same number of bits
#define MOD_NO_NOTE			63
	// Only 31 samples, which we refer to as 0-30
#define MOD_NO_SAMPLE		31

#define FALSE 0
#define TRUE 1

// emulate getch() from the DOS conio.h header
int getch(void) {
  struct termios oldattr, newattr;
  int ch;
  tcgetattr(STDIN_FILENO, &oldattr);
  newattr = oldattr;
  newattr.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
  return ch;
}

    // Define our assert. The Cleanup function in here frees up any 
	// dynamic memory claimed so far. There are probably better ways 
	// to do it, but this is pretty easy.
#define ASSERT(cond) \
	if((cond) == FALSE) { \
		fprintf(stdout, "\nASSERT FAILED\n\t%s\n\tLine %i\n\n", __FILE__, __LINE__); \
		Cleanup(); \
		getch(); \
		exit(1); \
	}

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;

typedef struct _SAMPLE_HEADER
{
	char	name[22];
	u16		length;
	u8		finetune;	 // This will be explained when writing the music player
	u8		vol;
	u16		loopStart;
	u16		loopLength;

	u32		smpDataIdx;  // Index into global table

} SAMPLE_HEADER;

typedef struct _MOD_HEADER
{
	char			name[20];
	SAMPLE_HEADER	sample[31];
	u8				order[128];
	u8				**pattern;
	u8				orderCount;
	u8				patternCount;

} MOD_HEADER;

typedef struct _SAMPLE_DATA
{
	s8  *data;
	s32 length;
} SAMPLE_DATA;

	// Handy to have all the globals bunched up together
typedef struct _GLOBAL_STRUCT
{
	char			**fileTable;
	MOD_HEADER		*modHeader;
	SAMPLE_HEADER	**sfx;
	SAMPLE_DATA		*smpData;
	s32				fileCount;
	s32				modCount;
	s32				sfxCount;
	s32				smpDataCount;

} GLOBAL_STRUCT;

// Prototypes for functions

void GeneratePeriodTable();
u32 GetFileList			(char **fileTable, u32 maxFiles);
void LoadMod			(MOD_HEADER *modHeader, char *fileName);
void LoadSamples		(MOD_HEADER *modHeader, FILE *modFile);
void LoadOrders			(MOD_HEADER *modHeader, FILE *modFile);
void LoadPatterns		(MOD_HEADER *modHeader, FILE *modFile);
void LoadSampleDatas	(MOD_HEADER *modHeader, FILE *modFile);
void WriteCFile			(const char *fileName);
void WriteAsmFile		(const char *fileName);
void WriteHeaderFile	(const char *fileName);
void Cleanup			();



const u16 octave1Period[12] = {
		// These periods are for one octave below middle
	856,808,762,720,678,640,604,570,538,508,480,453		// C-1 to B-1
};
	// 5 octaves' worth of space, call GeneratePeriodTable to fill it
	// Used during pattern conversion
u16 periodTable[12*5];

GLOBAL_STRUCT globals;


int main(int argc, char **argv)
{
	s32 i, curFile;
	char dirName[256], oldDir[256];
	int result;

		// Initialize the globals
	memset( &globals, 0, sizeof(GLOBAL_STRUCT) );
	GeneratePeriodTable();

		// die if we didn't get 1 argument (arg 0 is just the name of the 
		// program, arg 1 is the first real argument, so we should have 2 total)
	if( argc != 2 )
	{
		fprintf(stdout, "Usage: converter [folder]\nPress any key to continue.\n");
		getch();
		exit(1);
	}
	strcpy( dirName, argv[1] );

		// Remember the old working directory
	getcwd(oldDir, 256);

		// Set current working directory to the argument
	result = chdir(dirName);
	if(result != 0)
	{
		fprintf(stderr, "Folder does not exist\n");
		Cleanup();
		exit(1);
	}

	globals.fileTable = new char*[MAX_SONGS];
	ASSERT( globals.fileTable != NULL );
	memset( globals.fileTable, 0, MAX_SONGS*sizeof(char*) );

	globals.fileCount = GetFileList(globals.fileTable, MAX_SONGS);

	globals.modHeader = new MOD_HEADER[globals.fileCount];
	ASSERT( globals.modHeader != NULL );
	memset( globals.modHeader, 0, globals.fileCount*sizeof(MOD_HEADER) );

	globals.sfx = new SAMPLE_HEADER*[MAX_SFX];
	ASSERT( globals.sfx != NULL );
	memset( globals.sfx, 0, MAX_SFX*sizeof(SAMPLE_HEADER*) );

	globals.smpData = new SAMPLE_DATA[MAX_SAMPLE_DATAS];
	ASSERT( globals.smpData != NULL );
	memset( globals.smpData, 0, MAX_SAMPLE_DATAS*sizeof(SAMPLE_DATA) );

	for(curFile = 0; curFile < globals.fileCount; curFile++)
	{
			// Use modCount here instead of curFile, because SFX modules are 
			// not saved in the modHeader array, so they can get out of whack
		LoadMod( &globals.modHeader[globals.modCount], globals.fileTable[curFile] );

			// Search name for * flag
		if( strchr(globals.modHeader[globals.modCount].name, MOD_SFX_FLAG) )
		{
				// SFX module. Keep samples and delete the song.
			fprintf(stdout, "Loading %s as SFX\n", globals.fileTable[curFile]);

				// Copy all used samples from the current song
			for(i = 0; i < 31; i++)
			{
				if(globals.modHeader[globals.modCount].sample[i].length > 0)
				{
					globals.sfx[globals.sfxCount] = new SAMPLE_HEADER;
					*globals.sfx[globals.sfxCount] = globals.modHeader[globals.modCount].sample[i];
					globals.sfxCount++;
				}
			}

				// Delete any dynamic memory used by the song

			if(globals.modHeader[globals.modCount].pattern != NULL)
			{
					// If this song's array of patterns has been 
					// allocated, go through and delete them
				for(i = 0; i < globals.modHeader[globals.modCount].patternCount; i++)
				{
					if(globals.modHeader[globals.modCount].pattern[i] != NULL)
					{
						delete[] globals.modHeader[globals.modCount].pattern[i];
						globals.modHeader[globals.modCount].pattern[i] = NULL;
					}
				}

					// And delete the pointers
				delete[] globals.modHeader[globals.modCount].pattern;
				globals.modHeader[globals.modCount].pattern = NULL;
			}

			// Don't increment modCount. Let the next song use this slot.
		}
		else
		{
				// Regular song, so keep it and increment the count
			globals.modCount++;
		}
	}

	fprintf(stdout, "Writing C file...\n");
	WriteCFile		("SndData.c");

	fprintf(stdout, "Writing ASM file...\n");
	WriteAsmFile	("SndData.s");

	fprintf(stdout, "Writing Header file...\n");
	WriteHeaderFile	("SndData.h");

		// Restore old working directory before exiting
	chdir(oldDir);

	fprintf(stdout, "Success! Press any key to continue.\n");
	getch();

	return 0;

}	// main

void GeneratePeriodTable()
{
	u8 octave, note;

	for(octave = 0; octave < 5; octave++)
	{
		for(note = 0; note < 12; note++)
		{
				// *2 to get us into octave 0, then divide by 2 for each octave up from there
			periodTable[octave*12 + note] = (octave1Period[note]*2) >> octave;
		}
	}

}	// GeneratePeriodTable


int ends_with_mod(const char *txt) {
  int txt_len = strlen(txt);
  if (txt_len > 4) {
    if ((*(txt + txt_len - 4) == '.') &&
        ((*(txt + txt_len - 3) == 'm') || (*(txt + txt_len - 3) == 'M')) &&
        ((*(txt + txt_len - 2) == 'o') || (*(txt + txt_len - 2) == 'O')) &&
        ((*(txt + txt_len - 1) == 'd') || (*(txt + txt_len - 1) == 'D'))) {
      return 1;
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////
// GetFileList: Searches current directory for files of specified type
// fileTable is a pre-allocated array of maxFiles char*'s.
// Returns the number of files loaded.
////////////////////////////////////////////////////////////////////
u32 GetFileList(char **fileTable, u32 maxFiles) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("getcwd() error");
    return 0;
  }
  printf("Current working dir: %s\n", cwd);

  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(cwd)) == NULL) {
    printf("Unable to open directory %s...\n", cwd);
    return 0;
  }

  u32 curFile = 0;
  while ((ent = readdir(dir)) != NULL) {
    printf("%s\n", ent->d_name);
    if (ends_with_mod(ent->d_name)) {
      fileTable[curFile] = new char[strlen(ent->d_name) + 1];
      ASSERT(fileTable[curFile] != NULL); // Handle running out of memory
      strcpy(fileTable[curFile], ent->d_name);

      curFile++;
    }
  }
  closedir(dir);

  // curFile also happens to be the number of files loaded
  return curFile;
} // GetFileList

void LoadMod(MOD_HEADER *modHeader, char *fileName)
{
	FILE *modFile;
	char sig[4];

		// Open the file for reading as binary data
	modFile = fopen(fileName, "rb");
	if(modFile == NULL)
	{
		fprintf(stderr, "%s does not exist\n", fileName);
		Cleanup();
		exit(1);
	}

		// Validate file
	fseek(modFile, 1080, SEEK_SET);
	fread(sig, 4, 1, modFile);

	if( memcmp(sig, "M.K.", 4) && memcmp(sig, "4CHN", 4) )
	{
			// Neither of our accepted sigs, so die
		fprintf(stderr, "%s is not a 4-channel MOD file\n", fileName);
		Cleanup();
		exit(1);
	}

		// Good file, so go back to the start and load everything
	fseek(modFile, 0, SEEK_SET);

	fread(modHeader->name, 20, 1, modFile);
	modHeader->name[19] = '\0';		// Chop the last char to make sure string is NULL-terminated

	LoadSamples		( modHeader, modFile );
	LoadOrders		( modHeader, modFile );
	LoadPatterns	( modHeader, modFile );
	LoadSampleDatas	( modHeader, modFile );

	fclose(modFile);

}	// LoadMod

void LoadSamples(MOD_HEADER *modHeader, FILE *modFile)
{
	s32 i;

	for(i = 0; i < 31; i++)
	{
		SAMPLE_HEADER *smp = &modHeader->sample[i];

		fread(smp, 30, 1, modFile);
		smp->smpDataIdx = INVALID_SMP_DATA;   // Nothing for now, will load later

		smp->length =     ( ((smp->length & 0xff) << 8) | 
							 (smp->length >> 8) );

		smp->loopStart =  ( ((smp->loopStart & 0xff) << 8) | 
							 (smp->loopStart >> 8) );

		smp->loopLength = ( ((smp->loopLength & 0xff) << 8) | 
							 (smp->loopLength >> 8) );
	}

}	// LoadSamples

void LoadOrders(MOD_HEADER *modHeader, FILE *modFile)
{
	u32 trash;

	fread(&modHeader->orderCount, 1, 1, modFile);
	fread(&trash, 1, 1, modFile);	// Unused byte

		// Read the orders
	fread(modHeader->order, 128, 1, modFile);

		// Back to sig, already handled it so jump over
	fread(&trash, 4, 1, modFile);

}	// LoadOrders

void LoadPatterns(MOD_HEADER *modHeader, FILE *modFile)
{
	s32 i;
	u8 highestOrder = 0;
	u8 curPattern, row, column;

	for(i = 0; i < modHeader->orderCount; i++)
	{
		if(modHeader->order[i] > highestOrder)
			highestOrder = modHeader->order[i];
	}

		// +1 because pattern 0 is included
	modHeader->patternCount = highestOrder + 1;

		// Allocate the pointers for the patterns
	modHeader->pattern = new u8*[modHeader->patternCount];
	ASSERT( modHeader->pattern );   // handle running out of memory
	   // Initialize the memory to 0
	memset( modHeader->pattern, 0, modHeader->patternCount*sizeof(u8*) );

		// And load them
	for(curPattern = 0; curPattern < modHeader->patternCount; curPattern++)
	{
			// Allocate the memory for the current pattern (they are always 1K)
		modHeader->pattern[curPattern] = new u8[1024];
		ASSERT( modHeader->pattern[curPattern] );			// handle running out of memory
		memset( modHeader->pattern[curPattern], 0, 1024);	// initialize to 0

		for(row = 0; row < 64; row++)
		{
			for(column = 0; column < 4; column++)
			{
				u8 cell[4];
				u8 sample;
				u16 period;
				u8 effect;
				u8 param;
				u8 closestNote;

				fread(cell, 4, 1, modFile);

				sample = (cell[0] & 0xF0) | (cell[2] >> 4);
				period = cell[1] | ((cell[0] & 0xF) << 8);
				effect = cell[2] & 0xF;
				param  = cell[3];

				if(period == 0)
				{
						// Period 0 is no note
					closestNote = MOD_NO_NOTE;
				}
				else
				{
						// Now we'll loop through our note period table and find the closest match
					u16 closestDist = 0xffff;   // make sure the first comparison sets the closest note
					closestNote = 0;			// But still initialize note to prevent compiler warning

					for(i = 0; i < 12*5; i++)   // 5 octaves, 12 notes each
					{
						u16 newDist = abs( period - periodTable[i] );
						if(newDist < closestDist)
						{
							closestNote = (u8)i;
							closestDist = newDist;
						}
					}
				}

				if(sample == 0)
				{
					sample = MOD_NO_SAMPLE;
				}
				else
				{
					sample -= 1;
				}

					// Now that we have our note, we can store the data in our new pattern
					// Calculate the address of the cell to output to
					// rowOffset = row * 4 columns per row * 4 bytes per cell
					// columnOffset = column * 4 bytes per cell
				u8 *outCell = &modHeader->pattern[curPattern][row*4*4 + column*4];
				outCell[0] = closestNote;
				outCell[1] = sample;
				outCell[2] = effect;
				outCell[3] = param;
			}
		}
	}

}	// LoadPatterns

void LoadSampleDatas(MOD_HEADER *modHeader, FILE *modFile)
{
	s32 i, curSmp;

	for(curSmp = 0; curSmp < 31; curSmp++)
	{
		if(modHeader->sample[curSmp].length > 0)
		{
			SAMPLE_DATA tempSmpData;

				// length is still stored as half
			tempSmpData.length = modHeader->sample[curSmp].length*2;

			tempSmpData.data = new s8[tempSmpData.length];
			ASSERT(tempSmpData.data != NULL);

			fread(tempSmpData.data, tempSmpData.length, 1, modFile);

			for(i = 0; i < globals.smpDataCount; i++)
			{
				if(tempSmpData.length == globals.smpData[i].length)
				{
					if( !memcmp(tempSmpData.data, globals.smpData[i].data, tempSmpData.length) )
					{
							// Use existing sample data
						modHeader->sample[curSmp].smpDataIdx = i;
						break;
					}
				}
			}

			if(modHeader->sample[curSmp].smpDataIdx == INVALID_SMP_DATA)
			{
					// Never found an existing sample data, so add the 
					// new one to the end of the list and use it
				globals.smpData[globals.smpDataCount] = tempSmpData;
				modHeader->sample[curSmp].smpDataIdx = globals.smpDataCount;
				globals.smpDataCount++;
			}
			else
			{
					// Did find existing sample data, delete the temporary one
				delete[] tempSmpData.data;
				tempSmpData.data = NULL;
			}
		}
	}

}	// LoadSampleDatas

void WriteCFile(const char *fileName)
{
	s32 i, curMod;
	FILE *outFile;

		// Open file for writing text
	outFile = fopen(fileName, "wt");
	if(outFile == NULL)
	{
		fprintf(stderr, "Failed to create %s\n", fileName);
		Cleanup();
		exit(1);
	}

		// Includes
	fprintf(outFile, "#include \"../Gba.h\"\n");
	fprintf(outFile, "#include \"../Sound.h\"\n");

		// Externs for patterns
	for(curMod = 0; curMod < globals.modCount; curMod++)
	{
		for(i = 0; i < globals.modHeader[curMod].patternCount; i++)
		{
			fprintf(outFile, "\nextern const u8 dMod%iPattern%i[];", curMod, i);
		}
		fprintf(outFile, "\n");
	}

		// Externs for sample datas
	for(i = 0; i < globals.smpDataCount; i++)
	{
		fprintf(outFile, "\nextern const s8 dSmpData%i[];", i);
	}
	fprintf(outFile, "\n");

		// MOD data
	for(curMod = 0; curMod < globals.modCount; curMod++)
	{
		s32 sampleCount;

			// Find the last sample non-zero length sample, or stop at 0 if none are used
		for(i = 30; i > 0 && globals.modHeader[curMod].sample[i].length == 0; i--)
		{}

			// Count is one more than highest used
		sampleCount = i + 1;

			// Print the sample table
		fprintf(outFile, "\nconst SAMPLE_HEADER dMod%iSmpTable[] = {", curMod);
		for(i = 0; i < sampleCount; i++)
		{
			fprintf(outFile, "\n\t{ %i, %i, %i, %i, %i, dSmpData%i }, ", 
				globals.modHeader[curMod].sample[i].length, 
				globals.modHeader[curMod].sample[i].finetune, 
				globals.modHeader[curMod].sample[i].vol, 
				globals.modHeader[curMod].sample[i].loopStart, 
				globals.modHeader[curMod].sample[i].loopLength, 
				globals.modHeader[curMod].sample[i].smpDataIdx);
		}
		fprintf(outFile, "\n};\n");

			// Print the pattern table
		fprintf(outFile, "\nconst u8 *dMod%iPatternTable[] = {", curMod);
		for(i = 0; i < globals.modHeader[curMod].patternCount; i++)
		{
			if((i & 7) == 0)
				fprintf(outFile, "\n\t");

			fprintf(outFile, "dMod%iPattern%i, ", curMod, i);
		}
		fprintf(outFile, "\n};\n");

			// Print the pattern table
		fprintf(outFile, "\nconst u8 dMod%iOrderTable[] = {", curMod);
		for(i = 0; i < globals.modHeader[curMod].orderCount; i++)
		{
			if((i & 15) == 0)
				fprintf(outFile, "\n\t");

			fprintf(outFile, "%i, ", globals.modHeader[curMod].order[i]);
		}
		fprintf(outFile, "\n};\n");
	}

	fprintf(outFile, "\nconst MOD_HEADER dModTable[] = {");
	for(curMod = 0; curMod < globals.modCount; curMod++)
	{
		fprintf(outFile, "\n\t{ dMod%iSmpTable, ", curMod);
		fprintf(outFile, "dMod%iOrderTable, ", curMod);
		fprintf(outFile, "dMod%iPatternTable, ", curMod);
		fprintf(outFile, "%i, ", globals.modHeader[curMod].orderCount);
		fprintf(outFile, "%i }, ", globals.modHeader[curMod].patternCount);
	}
	fprintf(outFile, "\n};\n");

		// SFX headers, same as MOD sample tables
	fprintf(outFile, "\nconst SAMPLE_HEADER dSfxTable[] = {");
	if(globals.sfxCount == 0)
	{
			// This is just incase none of the songs are flagged for SFX.
			// Just put something in the table so it doesn't cause an error.
		fprintf(outFile, "\n\t{ 0, 0, 0, 0, 0, 0 }\t// Placeholder, no SFX specified");
	}
	else
	{
		for(i = 0; i < globals.sfxCount; i++)
		{
			fprintf(outFile, "\n\t{ %i, %i, %i, %i, %i, dSmpData%i }, ", 
				globals.sfx[i]->length, 
				globals.sfx[i]->finetune, 
				globals.sfx[i]->vol, 
				globals.sfx[i]->loopStart, 
				globals.sfx[i]->loopLength, 
				globals.sfx[i]->smpDataIdx);
		}
	}
	fprintf(outFile, "\n};\n");

	fclose(outFile);

}	// WriteCFile

void WriteAsmFile	(const char *fileName)
{
	s32 i, curMod, curPattern, curSmpData;
	FILE *outFile;

		// Open file for writing text
	outFile = fopen(fileName, "wt");
	if(outFile == NULL)
	{
		fprintf(stderr, "Failed to create %s\n", fileName);
		Cleanup();
		exit(1);
	}

		// Put everything in .rodata
	fprintf(outFile, ".section .rodata\n");

		// Patterns
	for(curMod = 0; curMod < globals.modCount; curMod++)
	{
		for(curPattern = 0; curPattern < globals.modHeader[curMod].patternCount; curPattern++)
		{
				// Declare the identifier
			fprintf(outFile, "\n.global dMod%iPattern%i", curMod, curPattern);
			fprintf(outFile, "\n.align 2");
			fprintf(outFile, "\ndMod%iPattern%i:", curMod, curPattern);

				// Print the data with 16 bytes per row
			for(i = 0; i < 1024; i++)
			{
				if((i & 15) == 0)
					fprintf(outFile, "\n.byte ");
				else
					fprintf(outFile, ", ");

				fprintf(outFile, "%i", globals.modHeader[curMod].pattern[curPattern][i]);
			}

			fprintf(outFile, "\n");
		}
	}

		// Sample datas
	for(curSmpData = 0; curSmpData < globals.smpDataCount; curSmpData++)
	{
			// Declare the identifier
		fprintf(outFile, "\n.global dSmpData%i", curSmpData);
		fprintf(outFile, "\n.align 2");
		fprintf(outFile, "\ndSmpData%i:", curSmpData);

			// Print the data with 16 bytes per row
		for(i = 0; i < globals.smpData[curSmpData].length; i++)
		{
			if((i & 15) == 0)
				fprintf(outFile, "\n.byte ");
			else
				fprintf(outFile, ", ");

			fprintf(outFile, "%i", globals.smpData[curSmpData].data[i]);
		}

		fprintf(outFile, "\n");
	}
}	// WriteAsmFile

void WriteHeaderFile(const char *fileName)
{
	s32 i;
	FILE *outFile;

		// Open file for writing text
	outFile = fopen(fileName, "wt");
	if(outFile == NULL)
	{
		fprintf(stderr, "Failed to create %s\n", fileName);
		Cleanup();
		exit(1);
	}

		// MOD defines
	for(i = 0; i < globals.modCount; i++)
	{
		fprintf(outFile, "\n#define MOD_%s %i", globals.modHeader[i].name, i);
	}
	fprintf(outFile, "\n");

		// SFX defines
	for(i = 0; i < globals.sfxCount; i++)
	{
		fprintf(outFile, "\n#define SFX_%s %i", globals.sfx[i]->name, i);
	}
	fprintf(outFile, "\n");

	fclose(outFile);

}	// WriteHeaderFile

void Cleanup()
{
	s32 i, curMod;

		// Check if the array of pointers has been allocated yet
	if(globals.fileTable != NULL)
	{
			// If so, delete any strings that exist
		for(i = 0; i < globals.fileCount; i++)
		{
			if(globals.fileTable[i] != NULL)
			{
				delete[] globals.fileTable[i];
				globals.fileTable[i] = NULL;
			}
		}

			// Delete the array itself
		delete[] globals.fileTable;
		globals.fileTable = NULL;
	}

		// Do MOD headers
	if(globals.modHeader != NULL)
	{
		for(curMod = 0; curMod < globals.modCount; curMod++)
		{
			if(globals.modHeader[curMod].pattern != NULL)
			{
					// If this header's array of patterns has been 
					// allocated, go through and delete them
				for(i = 0; i < globals.modHeader[curMod].patternCount; i++)
				{
					if(globals.modHeader[curMod].pattern[i] != NULL)
					{
						delete[] globals.modHeader[curMod].pattern[i];
						globals.modHeader[curMod].pattern[i] = NULL;
					}
				}

					// And delete the pointers
				delete[] globals.modHeader[curMod].pattern;
				globals.modHeader[curMod].pattern = NULL;
			}
		}

			// Delete the whole table of headers
		delete[] globals.modHeader;
		globals.modHeader = NULL;
	}

		// Kill the SFX headers
	if(globals.sfx != NULL)
	{
		for(i = 0; i < globals.sfxCount; i++)
		{
			if(globals.sfx[i] != NULL)
			{
				delete[] globals.sfx[i];
				globals.sfx[i] = NULL;
			}
		}

			// And the pointers
		delete[] globals.sfx;
		globals.sfx = NULL;
	}

		// Kill the sample datas
	if(globals.smpData != NULL)
	{
		for(i = 0; i < globals.smpDataCount; i++)
		{
			if(globals.smpData[i].data != NULL)
			{
				delete[] globals.smpData[i].data;
				globals.smpData[i].data = NULL;
			}
		}

		delete[] globals.smpData;
		globals.smpData = NULL;
	}

}	// Cleanup
