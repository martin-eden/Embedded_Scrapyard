// Assign standard pipes (stdin, stdout, stderr) to Serial

/*
  After setting pipes all functions dealing with standard I/O
  will start working.

  Formatted printing printf() worth rescue.

    InstallStandardStreams();
    printf("Heya!\n");
*/

/*
  Wrapped by: Martin Eden
  Last mod.: 2024-05-09
*/

#pragma once

void InstallStandardStreams();

/*
  2024-05-09
*/
