/*
*The MIT License
*
*Copyright (c) 2018 Mike (Milad) Taghavi <mitghi[at]gmail/me.com>
*
*Permission is hereby granted, free of charge, to any person obtaining a copy
*of this software and associated documentation files (the "Software"), to deal
*in the Software without restriction, including without limitation the rights
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*copies of the Software, and to permit persons to whom the Software is
*furnished to do so, subject to the following conditions:
*
*The above copyright notice and this permission notice shall be included in
*all copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reader.h"

#define nil NULL
#define _cast

File*
NewFile(const string path)
{
	File* file = _cast (File*) malloc(sizeof(*file));
	if(file == nil)
	{
		return nil;
	}
	memset(file, 0x00, sizeof(File));
	asprintf(&file->path, "%s", path);
	return file;
};

bool
FileOpen(File* const file, const string mode)
{
	if(file->isOpen)
	{
		return true;
	}
	file->fd = fopen(file->path, mode);
	file->isOpen = file->fd != nil;
	return file->isOpen;
};

bool
FileClose(File* const file)
{
	if(!file->isOpen)
	{
		return false;
	}
	int retcode = fclose(file->fd);
	file->isOpen = retcode == 0;
	return file->isOpen;
};

void
FileFree(File* file)
{
	free(file->path);
	free(file);
};

string
FileReadLine(File* const file)
{
	string line = nil;
	file->meta.lineposition = getline(&line, &file->meta.linecount, file->fd);
	if(file->meta.lineposition == -1)
	{
		free(line);
		return nil;
	}
	return line;
};

size_t
FileWriteLineC(File* const file, const string line)
{
	size_t llen = strlen(line);
	size_t wr = fwrite(line, sizeof(char), llen, file->fd);
	return wr;
};

size_t
FileWriteLine(File* const file, const string line, size_t s)
{
	size_t llen = strlen(line);
	size_t wr = fwrite(line, s, llen, file->fd);
	return wr;
};
