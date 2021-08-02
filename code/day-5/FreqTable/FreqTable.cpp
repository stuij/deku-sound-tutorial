// Generates a table of frequencies for each MOD note

#include <stdio.h>  // for file IO
#include <stdlib.h> // for stuff
#include <termios.h>
#include <unistd.h>

#define AMIGA_VAL 3579545

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;

const u16 periodTable[16][12] = {
    // Finetune 0
    {856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453}, // C-1 to B-1
                                                                  // Finetune 1
    {850, 802, 757, 715, 674, 637, 601, 567, 535, 505, 477, 450}, // C-1 to B-1
                                                                  // Finetune 2
    {844, 796, 752, 709, 670, 632, 597, 563, 532, 502, 474, 447}, // ...
                                                                  // Finetune 3
    {838, 791, 746, 704, 665, 628, 592, 559, 528, 498, 470, 444},
    // Finetune 4
    {832, 785, 741, 699, 660, 623, 588, 555, 524, 495, 467, 441},
    // Finetune 5
    {826, 779, 736, 694, 655, 619, 584, 551, 520, 491, 463, 437},
    // Finetune 6
    {820, 774, 730, 689, 651, 614, 580, 547, 516, 487, 460, 434},
    // Finetune 7
    {814, 768, 725, 684, 646, 610, 575, 543, 513, 484, 457, 431},
    // Finetune -8 (actually 0x8)
    {907, 856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480},
    // Finetune -7 (actually 0x9)
    {900, 850, 802, 757, 715, 675, 636, 601, 567, 535, 505, 477},
    // Finetune -6 (actually 0xA)
    {894, 844, 796, 752, 709, 670, 632, 597, 563, 532, 502, 474},
    // Finetune -5 (actually 0xB)
    {887, 838, 791, 746, 704, 665, 628, 592, 559, 528, 498, 470},
    // Finetune -4 (actually 0xC)
    {881, 832, 785, 741, 699, 660, 623, 588, 555, 524, 494, 467},
    // Finetune -3
    {875, 826, 779, 736, 694, 655, 619, 584, 551, 520, 491, 463},
    // Finetune -2
    {868, 820, 774, 730, 689, 651, 614, 580, 547, 516, 487, 460},
    // Finetune -1
    {862, 814, 768, 725, 684, 646, 610, 575, 543, 513, 484, 457},
};

int main(int argc, char **argv) {
  FILE *file;
  s32 i;
  u8 finetune, note, octave;
  // This is the table of frequencies in Hz for each note at each finetune level
  // in each octave. 16 finetune levels * 12 notes per octave * 5 octaves
  u16 freqTable[16 * 12 * 5];

  for (finetune = 0; finetune < 16; finetune++) {
    for (octave = 0; octave < 5; octave++) {
      for (note = 0; note < 12; note++) {
        // Calculate the period of this note in this octave. First multiply by 2
        // to get into octave 0, then divide by 2 for each octave up from there
        u16 tempPeriod = (periodTable[finetune][note] * 2) >> octave;
        u32 tempFreq = (AMIGA_VAL / tempPeriod);

        // Clip to 16 bits, because only the highest notes on the
        // highest finetunes will overflow it, and they're not worth
        // using u32's and doubling the size of the table
        if (tempFreq > 65535)
          tempFreq = 65535;

        // And plug it into our period->Hz formula
        freqTable[finetune * 12 * 5 + octave * 12 + note] = (u16)tempFreq;
      }
    }
  }

  if (argc < 2) {
    fprintf(stderr, "Usage:\n freqtable [outFile]\n");
    return 1;
  }

  file = fopen(argv[1], "wt");
  if (!file) {
    fprintf(stderr, "Usage:\n freqtable [outFile]\n");
    return 1;
  }

  fprintf(file, "const u16 noteFreqTable[] =\n{");
  for (i = 0; i < 16 * 5 * 12; i++) {
    // Print finetune header every set of octaves
    if (!(i % (12 * 5)))
      fprintf(file, "\n\t\t// Finetune %i", i / (12 * 5));

    // Print newline every set of notes
    if (!(i % 12))
      fprintf(file, "\n\t");

    // Print the note frequency
    fprintf(file, "%5i, ", freqTable[i]);

    // Print octave info after every set of notes
    if (!((i + 1) % 12) && i != 0)
      fprintf(file, "\t// C%i-B%i", i / 12 % 5, i / 12 % 5);
  }
  fprintf(file, "\n};\n");

  fprintf(stdout, "Success!\n");
  return 0;
}
