

#include "stdafx.h"
#include "SSLApplication.h"

extern CService theService;

#define DIRNAME "Z:\\Develop\\gov\\"
#define TARNAME "Z:\\Develop\\gov\\gh\\"
// #define HOSTNAME "http://192.168.1.21"

// #define DETECTCR	"\r\n"
#define DETECTCR	"\n"

#ifdef	USE_ARGCARGV
	extern int Argc;
	extern TCHAR** Argv;
#endif	USE_ARGCARGV

#ifdef FILETRANSLATE_APPLICATION

char CLIENT_CONTEXT[]	= "(Client:Context:";
char SERVER_CONTEXT[]	= "(Server:Context:";
char CONTEXT_END[]		= ")\r\n\r\n\r\n";
int  CONTEXT_LENGTH		= 16;
int  CONTEXT_END_LEN	= 7;

#define	MAX_BUFFER 300000
#define MAX_LINES 5000
#define MAX_PAIR 1000
#define MAX_TEAMS 100
#define HASH_GROUP	1

typedef struct HashTeam
{
	long size;
	long filesize;
	unsigned long* hash;
	char* buffer;
	long place;
	char fname[MAX_PATH+8];
	long fnsize;
}HashTeam;

typedef struct EqualTeam
{
	long size;
	long team[MAX_TEAMS];
}EqualTeam;


#define GetBuffer theService.pStartApplication->pInterApp->GetApplicationBuffer
#define GetContext theService.pStartApplication->pInter->GetProtocolContext

#define FreeBuffer(mBuffer) theService.pStartApplication->pInterApp->FreeApplicationBuffer(mBuffer)
#define FreeContext(mContext) theService.pStartApplication->pInter->FreeProtocolContext(mContext)

void RemoveMark(CListItem* tBuffer, CListItem* sBuffer, long size);
int ReplaceKeyword(char* sour, int ssize, char* key, int ksize, char* repl, int rsize, char* desc);
long GetFileLines(char* &buffer, long start, long end);
long WriteFileLines(long team, long start, long end, unsigned char* &file);

unsigned long* memlong(unsigned long* buffer, long size, unsigned long var);
long CreateLineHash(unsigned long* hash, char* buffer, long size) /* return lines */;
long CreateHashGroup(unsigned long* hash, unsigned long *hashgroup, long size, long group);
long ReadFile2Hash(HashTeam* nowteam, long buffersize);
long ReadFile2Hash(unsigned long* hash, char* buffer, long buffersize, char* filename);
long CmpHashTeam(long num, HashTeam* team, EqualTeam* et);
void DisplayEqualTeam(int lines, int num, EqualTeam* et);
long AddtoTeam(char* filename, CContextItem* forList);
long CmpFileGroup(char* path, char* filewild);
long CmpFileSequ(char* path, char* filesequ, int len);

void RemoveMark(CListItem* tBuffer, CListItem* sBuffer, long size)
{
	long ret_err = 0x01;																											//
	char *sPointer = REAL_BUFFER(sBuffer), *sStart;
	char *tPointer = REAL_BUFFER(tBuffer);
	char *sEnd = sPointer + size;
	long copySize = 0, totalCopySize = 0;

	sStart = sPointer;
	while (sPointer < sEnd && sStart)
	{
		sStart = memstr(sPointer, sEnd-sPointer, CLIENT_CONTEXT, CONTEXT_LENGTH);
		if (sStart)
		{
			copySize = sStart - sPointer;
			memcpy(tPointer, sPointer, copySize);
			tPointer  += copySize;
			totalCopySize += copySize;
			sPointer = memstr(sStart, 100, CONTEXT_END, CONTEXT_END_LEN);
			if (!sPointer) break;
			sPointer += CONTEXT_END_LEN;
		}
	}

	sStart = sPointer;
	while (sPointer < sEnd && sStart)
	{
		sStart = memstr(sPointer, sEnd-sPointer, SERVER_CONTEXT, CONTEXT_LENGTH);
		if (sStart)
		{
			copySize = sStart - sPointer;
			memcpy(tPointer, sPointer, copySize);
			tPointer  += copySize;
			totalCopySize += copySize;
			sPointer = memstr(sStart, 100, CONTEXT_END, CONTEXT_END_LEN);
			if (!sPointer) break;
			sPointer += CONTEXT_END_LEN;
		}
	}

	tBuffer->NProcessSize = totalCopySize;
}
char aa[]="\r\nab\r\nabcde\r\nabcdefg\r\nabcde";
unsigned long ab[20];

HashTeam useHash[MAX_TEAMS];
EqualTeam useEqual[MAX_PAIR*2];
unsigned char cmpTotal[500000];
unsigned char* tpointer = cmpTotal;

int ReplaceKeyword(char* sour, int ssize, char* key, int ksize, char* repl, int rsize, char* desc)
{
	char* send = sour + ssize;
	char* nowplace = sour, *nextplace;
	char* nowdesc = desc;

	do 
	{
		nextplace = memstr(nowplace, send-nowplace, key, ksize);
		if (nextplace)
		{
			memcpy(nowdesc, nowplace, nextplace-nowplace);
			nowdesc += (nextplace-nowplace);
			memcpy(nowdesc, repl, rsize);
			nowdesc += rsize;

			nowplace = nextplace + ksize;
		}
		else break;

	} while (TRUE);

	memcpy(nowdesc, nowplace, send-nowplace);
	nowdesc += (send-nowplace);
	return (nowdesc-desc);
}

void ReplaceKeywords(HashTeam* nowteam)
{
	char* nowstart = nowteam->buffer;
// 	int nowsize = nowteam->filesize;
	char* nowcr = memstr(nowstart, nowteam->filesize, "\r\n", 2);
	char* nowkey = (char*)memchr(nowstart, '?', nowcr-nowstart)+1;
	char* nextkey;
	char* keyval;
	CListItem* mBuffer;
	int newsize;

	if (!nowkey) return;

	do 
	{
		nextkey = (char*)memchr(nowkey, '&', nowcr-nowkey);
		if (!nextkey) nextkey = nowcr;
		keyval = (char*)memchr(nowkey, '=', nextkey-nowkey);

		if (!keyval)
			break;

		mBuffer = GetBuffer();
// 		newsize = ReplaceKeyword(nowstart, nowteam->filesize, nowkey, nextkey-nowkey, nowkey, keyval-nowkey, REAL_BUFFER(mBuffer));
		newsize = ReplaceKeyword(nowcr, nowteam->filesize-(nowcr-nowstart), nowkey, nextkey-nowkey, nowkey, keyval-nowkey, REAL_BUFFER(mBuffer));

		nowteam->filesize = newsize+(nowcr-nowstart);
		nowkey = nextkey + 1;
		memcpy(nowcr, REAL_BUFFER(mBuffer), newsize);
		FreeBuffer(mBuffer);

	}while(nextkey != nowcr);
}

long GetFileLines(char* &buffer, long start, long end)
{
	char *sstart = buffer-sizeof(DETECTCR)+1, *send;
	int i;

	for (i=0; i<start; i++)
	{
		sstart = memstr(sstart+sizeof(DETECTCR)-1, MAX_BUFFER, NASZ(DETECTCR) );
		if (!sstart) return 0;
	}

	send = sstart;
	for (i=0; i<end-start+1; i++)
	{
		send = memstr(send+sizeof(DETECTCR)-1, MAX_BUFFER, NASZ(DETECTCR));
		if (!send) return 0;
	}

	buffer = sstart+sizeof(DETECTCR)-1;
	return (send-sstart);
}

long WriteFileLines(long team, long start, long end, unsigned char* &total)
{
	HashTeam* nowhash = useHash+team;
	char* buf = nowhash->buffer;
	int size;

	size = GetFileLines(buf, start, end);
	if (!size) return -1;

	memcpy(total, buf, size);
	total += size;

	return 0;
}

long CreateLineHash(unsigned long* hash, char* buffer, long size)		// return lines
{
	char* lStart = buffer, *lEnd;
	char* bEnd = buffer + size;
	long nowLine = 0, totalLine;
	unsigned long tempsum, next;
	unsigned long* nowhash = hash;
	int i, j;

	*((unsigned long*)bEnd) = 0;

	do 
	{
		lEnd = memstr(lStart, bEnd-lStart, NASZ(DETECTCR));
		if (!lEnd)
			break;

		if (memstr(lStart, lEnd-lStart, NASZ("--------------")) == lStart) tempsum = TO_INT('-','-','-','-');
		else if (memstr(lStart, lEnd-lStart, NASZ("Cookie: JSESSIONID")) == lStart) tempsum = TO_INT('C','o','o','k');
		else if (memstr(lStart, lEnd-lStart, NASZ("Date: ")) == lStart) tempsum = TO_INT('D','a','t','e');
		else
		{
			tempsum = 0;
			j = ((lEnd-lStart)>>2);
			for (i=0; i<=j; i++)		// a little more than one line
			{
				next = *((unsigned long*)(lStart));
				tempsum += next;
				if (tempsum & 1)
					tempsum = 0x80000000 + (tempsum >> 1);
				else 
					tempsum = (tempsum >> 1);
				lStart+=4;
	// printf("0x%08x  ", tempsum);
			}
		}
		*nowhash++ = tempsum;
		lStart = lEnd+sizeof(DETECTCR)-1;
		nowLine++;
	} while (lStart<bEnd);

	totalLine = nowLine;
// 	if (totalLine<HASH_GROUP) return 0;
// 	printf("\r\n");

	return nowLine;
}

long CreateHashGroup(unsigned long* hash, unsigned long *hashgroup, long size, long group)
{
	unsigned long tempsum = 0;
	unsigned long* nowhash = hash;
	int i;
	for (i=0; i<group; i++) tempsum += *(hash+i);

	for (i=0; i<size-group+1; i++)
	{
		*hashgroup++ = tempsum;
		tempsum -= *nowhash;
		tempsum += *(nowhash+group);
		nowhash++;
	}
	return size-group+1;
}

unsigned long* memlong(unsigned long* buffer, long size, unsigned long var)
{
	for (int i=0; i<size; i++)
	{
		if (*buffer == var) return buffer;
		else buffer++;
	}
	return 0;
}

long ReadFile2Hash(HashTeam* nowteam, long buffersize)
{
	HANDLE hRead;
	long ret;
	DWORD fileread;

	hRead = CreateFile(nowteam->fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);																			//
	if (hRead == INVALID_HANDLE_VALUE) return -1;
	ret = ReadFile(hRead, nowteam->buffer, buffersize, &fileread, NULL);										//
	nowteam->filesize = fileread;

	if (!ret) return 0;				//
	CloseHandle(hRead);
	return CreateLineHash(nowteam->hash, nowteam->buffer, fileread);
}


long ReadFile2Hash(unsigned long* hash, char* buffer, long buffersize, char* filename)
{
	HANDLE hRead;
	long ret;
	DWORD fileread;

	hRead = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);																			//
	if (hRead == INVALID_HANDLE_VALUE) return -1;

	ret = ReadFile(hRead, buffer, buffersize, &fileread, NULL);										//

	if (!ret) return 0;				//
	CloseHandle(hRead);
	return CreateLineHash(hash, buffer, fileread);
}




long CmpHashTeam(long num, HashTeam* team, EqualTeam* et)
{
	int i, j, k;
// 	int model;
	HashTeam* nowteam = team;
	HashTeam* modelteam;
	EqualTeam* nowet = et;
	unsigned long* point;
	long ssize;
	int issame;

// 	et->size = team->size;
	if (num<=1) return 0;

	ssize = MAX_LINES;
printf("          ");
	nowteam = team;
	for (i=0; i<num; i++)
	{
printf("%9d,",nowteam->size);
		if ( nowteam->size < ssize )
		{
			ssize = nowteam->size;
			modelteam = nowteam;
		}
		nowteam++;
	}
printf("\r\n");

	for (i=ssize; i>HASH_GROUP; i--)
	{
		nowteam = team;
		for (j=0; j<num; j++)
		{
			CreateHashGroup(nowteam->hash, nowteam->hash+MAX_LINES, nowteam->size, i);
			nowteam++;
		}
		for (j=0; j<ssize-1; j++)
		{
			nowteam = team;
			issame = 1;
			for (k=0; k<num; k++)
			{
				if (nowteam != modelteam)
				{
					point = memlong(nowteam->hash+MAX_LINES, nowteam->size-i+1, *(modelteam->hash+(MAX_LINES+j)) );
					if (!point)
					{
						issame = 0;
						break;
					}
					else
					{
						nowteam->place = (point-nowteam->hash)-MAX_LINES;
					}
				}
				else nowteam->place = j;

				nowteam ++;
			}
			if (issame)
			{
				nowteam = team;
				nowet->size = i;
				for (k=0; k<num; k++)
				{
					nowet->team[k] = nowteam->place;
					nowteam++;
				}
				memset(modelteam->hash+j, 0, i*4);
				nowet ++;

				j += (i-1);			//	skip same lines
			}
		}
	}
	return nowet-et;
}

void DisplayEqualTeam(int lines, int num, EqualTeam* et)
{
	int i,j;
	int minval = -1, nowmin, nowval;
	int minplace;
	EqualTeam* nowet;
	EqualTeam* descet = et+MAX_PAIR;

// nowet = et;
// for (i=0; i<lines; i++)
// {
// 	printf("size:%5d, ", nowet->size);
// 	for (j=0; j<num; j++) printf("%4d-%4d ", nowet->team[j], nowet->team[j]+nowet->size-1);
// 	nowet++;
// 	printf("\r\n");
// }

	for (i=0; i<lines; i++)
	{
		nowet = et;
		nowmin = MAX_LINES;
		for (j=0; j<lines; j++)
		{
			nowval = nowet->team[0];
			if (nowval > minval && nowval < nowmin)
			{
				nowmin = nowval;
				minplace = j;
			}
			nowet++;
		}
		minval = nowmin;
		descet->size = (et+minplace)->size;
		for (j=0; j<num; j++) descet->team[j] = (et+minplace)->team[j];
		descet++;
	}

	descet = et+MAX_PAIR;
	int totalsize = 0;
	for (i=0; i<lines; i++)
	{
		printf("lines:%5d:", descet->size);
		totalsize += descet->size;
		for (j=0; j<num; j++)
		{
			printf("%4d-%4d,", descet->team[j], descet->team[j]+descet->size-1);
		}
		descet++;
		printf("\r\n");
	}
	printf("total same lines:%d\r\n", totalsize);

long nowlines[MAX_TEAMS];
long endline;
	for (i=0; i<MAX_TEAMS; i++) nowlines[i] = 0;

	nowet = et+MAX_PAIR;
	for (i=0; i<lines; i++)
	{
		for (j=0; j<num; j++)
		{
			tpointer = AddString(tpointer, NASZ("||||||||||||||"));
			tpointer = AddString(tpointer, (useHash+j)->fname, (useHash+j)->fnsize);
			tpointer = AddString(tpointer, NASZ("\r\n\r\n\r\n"));
			endline = nowet->team[j]-1;

			tpointer = AddString(tpointer, NASZ("||||||||||||||Between"));
			tpointer = AddInt(tpointer, nowlines[j], 5);
			tpointer = AddString(tpointer, NASZ("/"));
			tpointer = AddInt(tpointer, endline, 5);
			tpointer = AddString(tpointer, NASZ("---------------------------------------------------------------------------------------------------\r\n"));

			if (nowlines[j]>endline) 
			{
				tpointer = AddString(tpointer, NASZ("is Empty\r\n"));
			}
			else
			{
				WriteFileLines(j, nowlines[j], endline, tpointer);
			}
			nowlines[j] = endline+nowet->size+1;
		}

		tpointer = AddString(tpointer, NASZ("\r\n\r\n||||||||||||||Same Lines, "));
		tpointer = AddInt(tpointer, nowet->size, 5);
		tpointer = AddString(tpointer, NASZ(":for "));
		for (j=0; j<num; j++)
		{
			tpointer = AddInt(tpointer, nowet->team[j], 5);
			tpointer = AddString(tpointer, NASZ("/"));
			tpointer = AddInt(tpointer, nowet->team[j]+nowet->size-1, 5);
			tpointer = AddString(tpointer, NASZ(",  "));

		}
		tpointer = AddString(tpointer, NASZ("-------------------------------------------------------------------\r\n"));
		WriteFileLines(0, nowet->team[0], nowet->team[0]+nowet->size-1, tpointer);

		nowet++;
		tpointer = AddString(tpointer, NASZ("\r\n"));
		tpointer = AddString(tpointer, NASZ("||||||||||||||---------------------------------------------------------------------------------------------------------------------\r\n\r\n\r\n"));

		*tpointer = 0;
		printf("%s", cmpTotal);
		tpointer = cmpTotal;

	}
}


long AddtoTeam(char* filename, CContextItem* forList)
{
	static int teamNum = 0;
	CListItem* mBuffer, *mHash;
	long lines;

	mBuffer = GetBuffer();
	AddtoContentList(forList, mBuffer, NULL);
	mHash = GetBuffer();
	AddtoContentList(forList, mHash, NULL);

	useHash[teamNum].hash = REAL_ULBUFFER(mHash);
	useHash[teamNum].buffer = REAL_BUFFER(mBuffer);
	strcpy_s(useHash[teamNum].fname, MAX_PAIR, filename);
	useHash[teamNum].fnsize = strlen(filename);
	lines = ReadFile2Hash(&useHash[teamNum], mBuffer->BufferType->BufferSize);
// 	lines = ReadFile2Hash(REAL_ULBUFFER(mHash), REAL_BUFFER(mBuffer), mBuffer->BufferType->BufferSize, filename);
	useHash[teamNum].size = lines;

//	ReplaceKeywords(&useHash[teamNum]);		//	Not a good way, not use it

	return ++teamNum;
}

long CmpFileGroup(char* path, char* filewild)
{
	CFileContext* mContext;
	WIN32_FIND_DATA findLoop;
	HANDLE hFindLoop;
	int etnum, teamnum = 0;
	BOOL haveLoop = TRUE;

	mContext = (CFileContext*)GetContext();

	strcpy_s(mContext->fileName, MAX_PATH, path);
	strcat_s(mContext->fileName, MAX_PATH, filewild);

	mContext->fileNameLength = strlen(mContext->fileName);
	hFindLoop = FindFirstFile(mContext->fileName, &findLoop);

	while (hFindLoop != INVALID_HANDLE_VALUE && haveLoop)
	{
// 		ret_err = 0x30;
// 		modelSequ = GetInt(findLoop.cFileName, FileSequenceLength);
// 		if (haveRead[modelSequ] == HAVE_READ)
// 		{
// 			haveLoop = FindNextFile(hFindLoop, &findLoop);
// 			continue;
// 		}
// 		haveRead[modelSequ] = HAVE_READ;

		strcpy_s(mContext->fileName, MAX_PATH, path);
		strcat_s(mContext->fileName, MAX_PATH, findLoop.cFileName);
printf("%s\r\n", mContext->fileName);
		teamnum = AddtoTeam(mContext->fileName, mContext);

		if (teamnum>=MAX_TEAMS)
		{
			printf("Too many files\r\n");
			exit(0);
		}
		haveLoop = FindNextFile(hFindLoop, &findLoop);
	}
	FindClose(hFindLoop);

	if (teamnum)
	{
		etnum = CmpHashTeam(teamnum, useHash, useEqual);
		DisplayEqualTeam(etnum, teamnum, useEqual);
	}

	FreeContext(mContext);
	return 0;
}

long CmpFileSequ(char* path, char* filesequ, int len)
{
	HANDLE hFindSequ;
	WIN32_FIND_DATA findSequ;
	char filename[MAX_PATH*2];

	strcpy_s(filename, MAX_PATH, path);
	strcat_s(filename, MAX_PATH, filesequ);
	strcat_s(filename, MAX_PATH, "*.txt");
	hFindSequ = FindFirstFile(filename, &findSequ);

	if (hFindSequ != INVALID_HANDLE_VALUE)
	{
		strcpy_s(filename, MAX_PATH, "?????");
		strcat_s(filename, MAX_PATH, &findSequ.cFileName[5]);
		if (len)
		{
			filename[len] = 0;
			strcat_s(filename, MAX_PATH, "*.txt");
		}
		CmpFileGroup(path, filename);
	}
	FindClose(hFindSequ);
	return 0;
}

// long CSystemApplication::InitProcess(void)
// {
// 	long ret_err = 0x01;
// 
// 	__TRY
// 
// 	ret_err = 0x10;
// 	if (!SystemAddResource<CNormalBuffer>("FOR_BUFFER", 100, PAGE_SIZE, TRUE, 0)) break;
// 	if (!SystemAddResource<CHTTPContext>("HTTP", 10, CACHE_SIZE, TRUE, TIMEOUT_TCP)) break;
// 
// 	pInter = new CInterProtocol();
// 	pInter->SetAction(Resoucce, NULL, "HTTP");
// 	RegisterProtocol(pInter, PROTOCOL_INTER, 0);			// set NULL in SetAction, 
// 
// 	pInterApp = new CInterApplication();
// 	pInterApp->SetAction(Resoucce, "FOR_BUFFER");
// 	RegisterApplication(pInterApp, APPLICATION_INTER, FLAG_PROCESS_ACCEPT );
// 
// #ifdef USE_SERVICE
// // 	CmpFileSequ("50459");
// // 	CmpFileSequ("10196");
// // 	CmpFileSequ("50358");
// // 	CmpFileSequ("10413");
//  	CmpFileSequ("51151");
// #endif USE_SERVICE
// 
// #ifdef USE_ARGCARGV
// 	if (Argc == 3) CmpFileSequ(*(Argv+1), *(Argv+2), 0);		//real post
// 	if (Argc == 4) CmpFileSequ(*(Argv+1), *(Argv+2), atoi(*(Argv+3)));		//real post
// #endif	USE_ARGCARGV
// 
// 	exit(0);
//  	__CATCH(MODULE_APPLICATION, "FileTranslate")
// }

// 	This for remove same result file
#define HAVE_READ 1
// long CSystemApplication::InitProcess(void)
// {
// 	long ret;
// 	long ret_err = 0x01;
// 	long modelSequ, fileSequ;
// 	DWORD fileread;
// 	WIN32_FIND_DATA findLoop, findSame;
// 	HANDLE hFindLoop, hFindSame, hRead;
// 	BOOL haveLoop = TRUE, haveSame = TRUE;
// 	CFileContext* mContext, *modelContext;
// 	CListItem* mBuffer, *sBuffer;;
// 
// 	char* haveRead;
//  	char* modelBuffer, *modelStart;
// 	char* fileBuffer, *fileStart;
// 
// 	__TRY
// 
// 	ret_err = 0x10;
// 	if (!SystemAddResource<CNormalBuffer>("FOR_BUFFER", 5, PAGE_SIZE, TRUE, 0)) break;
// 	if (!SystemAddResource<CHTTPContext>("HTTP", 10, CACHE_SIZE, TRUE, TIMEOUT_TCP)) break;
// 
// 	pInter = new CInterProtocol();
// 	pInter->SetAction(Resoucce, NULL, "HTTP");
// 	RegisterProtocol(pInter, PROTOCOL_INTER, 0);			// set NULL in SetAction, 
// 
// 	pInterApp = new CInterApplication();
// 	pInterApp->SetAction(Resoucce, "FOR_BUFFER");
// 	RegisterApplication(pInterApp, APPLICATION_INTER, FLAG_PROCESS_ACCEPT );
// 
// 	haveRead = (char*)malloc(65536+16);
// 	if (!haveRead) break;
// 	memset(haveRead, 0, 65536);
// 
// 	mContext = (CFileContext*)pInter->GetProtocolContext();
// 	if (!mContext) break;
// 	mContext->PApplication = pInterApp;
// 
// 	modelContext = (CFileContext*)pInter->GetDuplicateContext(mContext);
// 	if (!modelContext) break;
// 
// 	mBuffer = pInterApp->GetApplicationBuffer();
// 	if (!mBuffer) break;
// 	modelBuffer = REAL_BUFFER(mBuffer);
// 
// 	sBuffer = pInterApp->GetApplicationBuffer();
// 	if (!sBuffer) break;
// 	fileBuffer = REAL_BUFFER(sBuffer);
// 
// 	strcpy_s(mContext->fileName, MAX_PATH, TARNAME);
// 	strcat_s(mContext->fileName, MAX_PATH, "*.txt");
// 	mContext->fileNameLength = sizeof(TARNAME);
// 	hFindLoop = FindFirstFile(mContext->fileName, &findLoop);
// 
// 	while (hFindLoop != INVALID_HANDLE_VALUE && haveLoop)
// 	{
// 		ret_err = 0x30;
// 		modelSequ = GetInt(findLoop.cFileName, FileSequenceLength);
// 		if (haveRead[modelSequ] == HAVE_READ)
// 		{
// 			haveLoop = FindNextFile(hFindLoop, &findLoop);
// 			continue;
// 		}
// 		haveRead[modelSequ] = HAVE_READ;
// 
// 		strcpy_s(mContext->fileName, MAX_PATH, TARNAME);
// 		strcat_s(mContext->fileName, MAX_PATH, findLoop.cFileName);
// 
// 		ret_err = 0x40;
// 		hRead = CreateFile(mContext->fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);																			//
// 		if (hRead == INVALID_HANDLE_VALUE) break;
// 
// 		ret_err = 0x50;
// 		ret = ReadFile(hRead, modelBuffer, mBuffer->BufferType->BufferSize, &fileread, NULL);										//
// 		if (!ret) break;				//
// // 		printf("Read file %s, size:%d\r\n", ((CFileContext*)(mContext))->fileName, fileread);
// 		CloseHandle(hRead);
// 
// 		modelStart = memstr(modelBuffer, fileread, "\r\n\r\n", 4);
// 		if (!modelStart) break;
// 		modelStart = memstr(modelStart+4, fileread, "\r\n\r\n", 4);
// 		if (!modelStart) break;
// 		if (modelStart > modelBuffer+fileread) break;
// 		mBuffer->NProcessSize = fileread-(modelStart-modelBuffer);
// 
// printf("%d: ", modelSequ);
// 
// 		strcpy_s(modelContext->fileName, MAX_PATH, TARNAME);
// 		strcat_s(modelContext->fileName, MAX_PATH, "?????");
// 		strcat_s(modelContext->fileName, MAX_PATH, findLoop.cFileName+FileSequenceLength);
// 		hFindSame = FindFirstFile(modelContext->fileName, &findSame);
// 
// 		while (hFindSame != INVALID_HANDLE_VALUE && haveSame)
// 		{
// 			fileSequ = GetInt(findSame.cFileName, FileSequenceLength);
// 			if (fileSequ != modelSequ)
// 			{
// 				haveRead[fileSequ] = HAVE_READ;
// 
// 				strcpy_s(modelContext->fileName, MAX_PATH, TARNAME);
// 				strcat_s(modelContext->fileName, MAX_PATH, findSame.cFileName);
// 
// 				ret_err = 0x40;
// 				hRead = CreateFile(modelContext->fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);																			//
// 				if (hRead == INVALID_HANDLE_VALUE) break;
// 
// 				ret_err = 0x50;
// 				ret = ReadFile(hRead, fileBuffer, sBuffer->BufferType->BufferSize, &fileread, NULL);										//
// 				if (!ret) break;				//
// // 				printf("Read file %s, size:%d\r\n", ((CFileContext*)(modelContext))->fileName, fileread);
// 				CloseHandle(hRead);
// 
// 				fileStart = memstr(fileBuffer, fileread, "\r\n\r\n", 4);
// 				if (!fileStart) break;
// 				fileStart = memstr(fileStart+4, fileread, "\r\n\r\n", 4);
// 				if (!fileStart) break;
// 				if (fileStart > fileBuffer+fileread) break;
// 				sBuffer->NProcessSize = fileread-(fileStart-fileBuffer);
// 
// 				if (mBuffer->NProcessSize == sBuffer->NProcessSize)
// 				{
// // 					if (!memcmp(modelStart, fileStart, mBuffer->NProcessSize)) printf("%d,", fileSequ);
// hRead = CreateFile(modelContext->fileName, GENERIC_WRITE, FILE_SHARE_READ,	NULL, CREATE_ALWAYS, NULL, NULL);				//	to delete file
// CloseHandle(hRead);
// 				}
// 			}
// 			haveSame = FindNextFile(hFindSame, &findSame);
// 		}
// printf("\r\n");
// 		FindClose(hFindSame);
// 		haveSame = TRUE;
// 
// 		haveLoop = FindNextFile(hFindLoop, &findLoop);
// 	}
// 
// 	FindClose(hFindLoop);
// 
// 	__CATCH(MODULE_APPLICATION, "FileTranslate")
// }


// 	This for change filename and remove 304,404, no return file
long CSystemApplication::InitProcess(void)
{
	long ret;
	long ret_err = 0x01;
	DWORD fileread, filewrite;
	WIN32_FIND_DATA findLoop;
	HANDLE hFindLoop, hRead, hWrite;
	BOOL haveLoop = TRUE;
	CFileContext* mContext, *modelContext, *writeContext;
	CListItem* mBuffer, *tBuffer;
	char* modelBuffer, *modelStart;

	__TRY

	ret_err = 0x10;
	if (!SystemAddResource<CNormalBuffer>("FOR_BUFFER", 5, PAGE_SIZE, TRUE, 0)) break;
	if (!SystemAddResource<CHTTPContext>("HTTP", 10, CACHE_SIZE, TRUE, TIMEOUT_TCP)) break;

	pInter = new CInterProtocol();
	pInter->SetAction(Resoucce, NULL, "HTTP");
	RegisterProtocol(pInter, PROTOCOL_INTER, 0);			// set NULL in SetAction, 

	pInterApp = new CInterApplication();
	pInterApp->SetAction(Resoucce, "FOR_BUFFER");
	RegisterApplication(pInterApp, APPLICATION_INTER, FLAG_PROCESS_ACCEPT );

// 	strcpy_s(dirName, MAX_PATH, DIRNAME);
// 	strcat_s(dirName, MAX_PATH, "*.log");

	mContext = (CFileContext*)pInter->GetProtocolContext();
	if (!mContext) break;
	mContext->PApplication = pInterApp;

	modelContext = (CFileContext*)pInter->GetDuplicateContext(mContext);
	if (!modelContext) break;

// #ifdef	USE_ARGCARGV
// 	char* argv1 = *(Argv+1);
// 	char* argv2 = *(Argv+2)
// 	if (Argc == 3 && *(argv1+strlen(argv1)-1) == '\\' && *(argv2+strlen(argv2)-1) == '\\')
// 	{
// 		strcpy_s(modelContext->fileName, MAX_PATH, *(Argv+1));
// 		strcat_s(modelContext->fileName, MAX_PATH, "*.txt");
// 		modelContext->fileNameLength = strlen(argv1);
// 	}
// 	else
// 	{
// 		printf("Remove sour desc:\r\nExample: remove c:\\abc\\ d:\\cba\\")
// 		printf("\r\nRemove 304 and 404 for *.txt file in given dirctiory. \r\n");
// 		exit(0);
// 	}
// #endif	USE_ARGCARGV

#ifdef	USE_SERVICE
	strcpy_s(modelContext->fileName, MAX_PATH, DIRNAME);
	strcat_s(modelContext->fileName, MAX_PATH, "*.txt");
	modelContext->fileNameLength = sizeof(DIRNAME);
#endif	USE_SERVICE

	hFindLoop = FindFirstFile(modelContext->fileName, &findLoop);

	writeContext = (CFileContext*)pInter->GetDuplicateContext(mContext);
	if (!writeContext) break;


	mBuffer = pInterApp->GetApplicationBuffer();
	if (!mBuffer) break;
	modelBuffer = REAL_BUFFER(mBuffer);

	while (hFindLoop != INVALID_HANDLE_VALUE && haveLoop)
	{
		ret_err = 0x30;
		strcpy_s(mContext->fileName, MAX_PATH, DIRNAME);
		strcat_s(mContext->fileName, MAX_PATH, findLoop.cFileName);
		if (findLoop.nFileSizeHigh || findLoop.nFileSizeLow > 10*1024*1024) break;

		ret_err = 0x40;
		hRead = CreateFile(mContext->fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);																			//
		if (hRead == INVALID_HANDLE_VALUE) break;

		ret_err = 0x50;
		ret = ReadFile(hRead, modelBuffer, mBuffer->BufferType->BufferSize, &fileread, NULL);										//
		if (!ret) break;				//
		printf("Read file %s, size:%d\r\n", ((CFileContext*)(mContext))->fileName, fileread);
		CloseHandle(hRead);

		ret_err = 0x60;
		modelStart = memstr(modelBuffer, fileread, "\r\n\r\n", 4);
// 		if (!modelStart) break;

		if (modelStart && strncmp(modelStart+4, "HTTP/1.1 304", 12) && strncmp(modelStart+4, "HTTP/1.1 404", 12) && (modelStart-modelBuffer < fileread-10))
		{
			ret_err = 0x70;
			tBuffer = pInterApp->GetApplicationBuffer();
			if (!tBuffer) break;
			
	//		RemoveMark(tBuffer, mBuffer, fileread);
			memcpy(REAL_BUFFER(tBuffer), REAL_BUFFER(mBuffer), fileread);

			if (AnalysisProxyURL(mContext, mBuffer, modelStart-modelBuffer+4)) break;
			modelBuffer = REAL_BUFFER(mBuffer);

			ret_err = 0x70;
			if (GetURL2Filename(mContext, modelContext, mBuffer, fileread)) break;
		
			ret_err = 0x80;
			strcpy_s(writeContext->fileName, MAX_PATH, TARNAME);
			memcpy((writeContext->fileName)+sizeof(TARNAME)-1,
				mContext->fileName+sizeof(DIRNAME)-1, mContext->fileNameLength-sizeof(DIRNAME)+1);
			writeContext->fileNameLength = mContext->fileNameLength-sizeof(DIRNAME)+sizeof(TARNAME);
			*(writeContext->fileName+writeContext->fileNameLength) = 0;

			ret_err = 0x90;
			hWrite = CreateFile(writeContext->fileName, GENERIC_WRITE, FILE_SHARE_READ,	NULL, CREATE_ALWAYS, NULL, NULL);				//
			if (hWrite == INVALID_HANDLE_VALUE) break;
			printf("Write file %s\r\n", writeContext->fileName);

			ret_err = 0xa0;
			ret = WriteFile(hWrite, REAL_BUFFER(tBuffer), fileread/*tBuffer->NProcessSize*/, &filewrite, NULL);
			pInterApp->FreeApplicationBuffer(tBuffer);
		}

		haveLoop = FindNextFile(hFindLoop, &findLoop);
	}

	FindClose(hFindLoop);

	__CATCH(MODULE_APPLICATION, "FileTranslate")
}

#endif FILETRANSLATE_APPLICATION
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 