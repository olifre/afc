// SPDX-License-Identifier: GPL-3.0-or-later

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <cstring>
#include <sstream>
#include <limits.h>
#include <time.h>
#include <getopt.h>

#include <zlib.h>

bool checkCRC(FILE *fSource, const char *baseName, size_t bytesTotal, size_t maxStrLen, bool colour, bool isTTY) {
	clock_t init, final;
	init=clock();

	size_t bufferSize = 1048576;
	unsigned char *sBuf = new unsigned char[bufferSize];

	size_t bytesRead = 0;
	size_t bytesReadTotal = 0;

	int cPerc=0;
	uLong outCRC = crc32(0L, Z_NULL, 0);
	while ((bytesRead = fread(sBuf, sizeof(char), bufferSize, fSource))) {
		outCRC = crc32(outCRC, sBuf, bytesRead);
		bytesReadTotal += bytesRead;
		if (isTTY && ((int)(1.*bytesReadTotal/bytesTotal*10.) > cPerc)) {
			final=clock()-init;

			double exec_time = 1.*final / CLOCKS_PER_SEC;
			cPerc=1.*bytesReadTotal/bytesTotal*10.;

			char format[16];
			sprintf(format, "\r%%%zus [", maxStrLen);
			printf(format, baseName);
			int dashC;
			for (dashC=0; dashC<cPerc; dashC++) {
				printf("#");
			}
			for (; dashC<10; dashC++) {
				printf(" ");
			}
			printf("] (%.2f s)", exec_time);
			fflush(stdout);
		}
	}
	delete [] sBuf;

	bool upper_case=true;
	bool CRC_OK=false;

	// Check whether we have an uppercase or lowercase match, and store foundPos.
	char crc_str[9];
	const char *foundPos;
	sprintf(crc_str, "%08lX", outCRC);
	if ((foundPos=strstr(baseName, crc_str)) != NULL) {
		CRC_OK=true;
		upper_case=true;
	} else {
		sprintf(crc_str, "%08lx", outCRC);
		if ((foundPos=strstr(baseName, crc_str)) != NULL) {
			CRC_OK=true,
			upper_case=false;
		} else {
			CRC_OK=false;
		}
	}

	final=clock()-init;
	double exec_time = 1.*final / CLOCKS_PER_SEC;

	char format[100];
	char OK_string[100];
	char ERROR_string[100];

	int colourLen;
	if (colour) {
		colourLen=16;
		sprintf(OK_string, "%c[%d;%dmOK%c[%dm",27,1,32,27,0);
		sprintf(ERROR_string, "%c[%d;%dmERROR%c[%dm",27,1,31,27,0);
	} else {
		colourLen=5;
		sprintf(OK_string, "OK");
		sprintf(ERROR_string, "ERROR");
	}

	const char *baseNameFinal;
	char* baseNameCCRC = new char[strlen(baseName)+12];
	int toPad=maxStrLen;
	if (CRC_OK && colour) {
		if (foundPos-baseName > INT_MAX) {
			delete [] baseNameCCRC;
			std::cerr << "Something very wrong here!!!" << std::endl;
			exit(1);
		}
		if ((baseName-foundPos-8) > INT_MAX) {
			delete [] baseNameCCRC;
			std::cerr << "Something very wrong here!!!" << std::endl;
			exit(1);
		}
		int preCRC=foundPos-baseName;
		int aftCRC=(strrchr(baseName,'\0')-foundPos-8);
		sprintf(baseNameCCRC, "%*.*s%c[%d;%dm%8.8s%c[%dm%*.*s", preCRC, preCRC, baseName, 27,1,32, foundPos, 27,0, aftCRC, aftCRC, foundPos+8);
		baseNameFinal=baseNameCCRC;
		toPad+=11;
	} else {
		baseNameFinal=baseName;
	}

	if (isTTY) {
		printf("\r");
	}
	if (colour) {
		sprintf(format, "%%%ds [ %c[%d;%dm%%08%s%c[%dm ] %%%ds (%%.2f s)\r\n", toPad, 27,1, CRC_OK ? 32 : 31, upper_case ? "X" : "x", 27, 0, colourLen);
	} else {
		sprintf(format, "%%%ds [ %%08%s ] %%%ds (%%.2f s)\r\n", toPad, upper_case ? "X" : "x", colourLen);
	}

	printf(format, baseNameFinal, outCRC, CRC_OK ? OK_string : ERROR_string, exec_time);

	fflush(stdout);

	delete [] baseNameCCRC;

	return CRC_OK;
}

void printHelp(const char* progName) {
	std::cout << "/------------------------------------------------------------------------------\\" << std::endl;
	std::cout << "|                         ***** Anime file checker *****                       |" << std::endl;
	std::cout << "|------------------------------------------------------------------------------|" << std::endl;
	std::stringstream cmd;
	cmd << "Calculates CRC32 of given files and looks for matching string in filename. ";
	std::cout << "| " << cmd.str() << std::setw(80-2-cmd.str().length())                    << " |" << std::endl;
	cmd.str(std::string());
	cmd << "Uses width corresponding to longest filename, so please resize accordingly!";
	std::cout << "| " << cmd.str() << std::setw(80-2-cmd.str().length())                    << " |" << std::endl;
	std::cout << "|------------------------------------------------------------------------------|" << std::endl;
	cmd.str(std::string());
	cmd << "Usage: " << progName << " [options] <filename(s)>";
	std::cout << "| " << cmd.str() << std::setw(80-2-cmd.str().length())                    << " |" << std::endl;
	cmd.str(std::string());
	cmd << " Possible options: ";
	std::cout << "| " << cmd.str() << std::setw(80-2-cmd.str().length())                    << " |" << std::endl;
	cmd.str(std::string());
	cmd << "  -C        force colour even when output is not a TTY.";
	std::cout << "| " << cmd.str() << std::setw(80-2-cmd.str().length())                    << " |" << std::endl;
	cmd.str(std::string());
	cmd << "  -D        disable colour even when output is a TTY.";
	std::cout << "| " << cmd.str() << std::setw(80-2-cmd.str().length())                    << " |" << std::endl;
	cmd.str(std::string());
	cmd << "  -h,--help show this help text.";
	std::cout << "| " << cmd.str() << std::setw(80-2-cmd.str().length())                    << " |" << std::endl;
	std::cout << "|------------------------------------------------------------------------------|" << std::endl;
	cmd.str(std::string());
	cmd << "Exit codes:";
	std::cout << "| " << cmd.str() << std::setw(80-2-cmd.str().length())                    << " |" << std::endl;
	cmd.str(std::string());
	cmd << " - 1 if any file could not be accessed.";
	std::cout << "| " << cmd.str() << std::setw(80-2-cmd.str().length())                    << " |" << std::endl;
	cmd.str(std::string());
	cmd << " - 2 if any CRC error occured.";
	std::cout << "| " << cmd.str() << std::setw(80-2-cmd.str().length())                    << " |" << std::endl;
	std::cout <<"\\------------------------------------------------------------------------------/" << std::endl;
}

namespace longParameters {
        enum _longParameters {
		NONE,
		HELP,
	};
}

const struct option long_options[] = {
	{"help",	no_argument,	nullptr,	longParameters::HELP},
	{nullptr,	0,		nullptr,	longParameters::NONE},
};

int main(int argc, char **argv) {

	if (argc == 1) {
		printHelp(argv[0]);
		std::cerr << "No filenames were given!" << std::endl;
		exit(0);
	}

	bool useColour=true;
	bool isTTY = isatty(fileno(stdout));
	if (!isTTY) {
		useColour=false;
	}

	{
		int opt = 0;
		int option_index = 0;
		while ((opt=getopt_long(argc, argv, "+hCD", long_options, &option_index)) != -1) {
			switch (opt) {
				case 'h':
				case longParameters::HELP:
					printHelp(argv[0]);
					exit(0);
				case 'C':
					useColour=true;
					break;
				case 'D':
					useColour=false;
					break;
			}
		}
	}

	size_t maxStrLen=0;
	for (int i=optind; i<argc; i++) {
		char *fileName=argv[i];
		char *baseName = strdup(fileName);
		if (strlen(baseName) > maxStrLen) {
			maxStrLen=strlen(baseName);
		}
		free(baseName);
	}

	int filesOK=0;
	int filesToCheck=argc-optind;
	int filesReadError=0;

	for (int i=optind; i<argc; i++) {
		char *fileName=argv[i];
		char *baseName = strdup(fileName);

		struct stat statbuf;
		if (stat(fileName, &statbuf) == -1) {
			/* check the value of errno */
			std::cerr << "Error stating file " << fileName << ": " << strerror(errno) << ", skipping that!" << std::endl;
			filesReadError++;
			continue;
		}
		if (!S_ISREG(statbuf.st_mode)) {
			// Not a regular file, skip that.
			continue;
		}
		size_t bytesTotal = statbuf.st_size;

		FILE *fSource = fopen(fileName, "rb");
		if (fSource == NULL) {
			std::cerr << "Error opening " << fileName << "!" << std::endl;
			filesReadError++;
			continue;
		}

		bool crc_ok=checkCRC(fSource, basename(baseName), bytesTotal, maxStrLen, useColour, isTTY);
		if (crc_ok) {
			filesOK++;
		}

		free(baseName);
		fclose(fSource);
	}

	std::cout << filesToCheck << " files checked, "
	          << filesOK << " files ok, "
	          << filesReadError << " files could not be read."
	          << std::endl;

	int exitCode = 0;

	if (filesReadError > 0) {
		exitCode = 1;
	} else if (filesOK < filesToCheck) {
		exitCode = 2;
	}

	return exitCode;
}
