//===- SystemUtils.h - Utilities to do low-level system stuff --*- C++ -*--===//
//
// This file contains functions used to do a variety of low-level, often
// system-specific, tasks.
//
//===----------------------------------------------------------------------===//

#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H

#include <string>

/// isExecutableFile - This function returns true if the filename specified
/// exists and is executable.
///
bool isExecutableFile(const std::string &ExeFileName);

// FindExecutable - Find a named executable, giving the argv[0] of bugpoint.
// This assumes the executable is in the same directory as bugpoint itself.
// If the executable cannot be found, return an empty string.
//
std::string FindExecutable(const std::string &ExeName,
			   const std::string &BugPointPath);

/// removeFile - Delete the specified file
///
void removeFile(const std::string &Filename);

/// getUniqueFilename - Return a filename with the specified prefix.  If the
/// file does not exist yet, return it, otherwise add a suffix to make it
/// unique.
///
std::string getUniqueFilename(const std::string &FilenameBase);

/// RunProgramWithTimeout - This function executes the specified program, with
/// the specified null-terminated argument array, with the stdin/out/err fd's
/// redirected, with a timeout specified on the commandline.  This terminates
/// the calling program if there is an error executing the specified program.
/// It returns the return value of the program, or -1 if a timeout is detected.
///
int RunProgramWithTimeout(const std::string &ProgramPath, const char **Args,
			  const std::string &StdInFile = "",
			  const std::string &StdOutFile = "",
			  const std::string &StdErrFile = "");

#endif
