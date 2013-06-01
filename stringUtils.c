/*
****************************COPYRIGHTS AND LICENSE****************************

Copyright (c) 2013, Mhd Adel Al Qodamni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:


Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/* Begin doesMatchDelim */
/* Returns 1 if the ch matches at least one of the delim chars
the delim is a string of characters */
static int doesMatchDelim(char ch, char *delim)
{
	char *ptr = delim;
	if(ch == '\0')
		return 0;
	while(*ptr)
	{
		if(ch == *ptr)
			return 1;
		ptr++;
	}
	return 0;
}
/* End doesMatchDelim  */

/* begin countWords  */
/*This function should count how many words exist in a string
where a word is specified as the collection of graphed characters delimited
by the delim paramter */
int countWords(char *string, char *delim)
{
	int count = 0;
	char *str = string;
	if(!doesMatchDelim(*str, delim))
		count++;	
	while(*str)
        {
		if(doesMatchDelim(*str, delim)){/*reached a delim*/
			while(doesMatchDelim(*str, delim))/*keep ignoring them*/
				str++;
			if(*str == '\0')/*return if string is over*/
				return count;	
			count++;/*no more blank and string didn't end=new word*/
		}
		str++;
	}
	return count;
}
/* end countWords  */

/* beging split */
char** split(char *text, char *delim)
{
	char *state = text;/* Used to store where's the next word to be read */
	char *prevState; /* Stores where last word began */
	int wordSize;/* the size of each word in the text string */
	size_t i, j;/* loop counters */
	char **strings;
	size_t wordsCount = countWords(text, delim);
	if(wordsCount == 0)
		return NULL;
	/* +1 because the last word is NULL */
	if((strings = malloc((wordsCount+1)*sizeof(char*))) == NULL)
		return NULL;
	/* For each word in the strings array
		-read the text till you reach a delim and count its length
		-allocate a word for the text you read
		-copy the word to the array strings
		-save the state of where we stopped
		-repeat */
	for(i = 0; i < wordsCount; i++)
	{
		/* ignore leading delims */
		while(doesMatchDelim(*state, delim) && *state)
			++state;/* move the state to the next word */
		wordSize = 0;
		prevState = state;
		while(!doesMatchDelim(*state, delim) && *state)
		{
			++wordSize;
			++state;
		}/*now state points to the beginning of the first delim */
		strings[i] = malloc(wordSize * sizeof(char) + 1);/*+1 for \0 */
		if(strings[i] == NULL)
		{
			for(j = 0; j < i; j++)
				free(strings[j]);
		}
		memcpy(strings[i], prevState, wordSize);	
	}
	strings[wordsCount] = NULL;
	return strings;
}
/* end split */

