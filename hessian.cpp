
#include "stdafx.h"

#define		HESSIAN_METHOD_END		0x7f7a007a		/*z*/
#define		HESSIAN_OBJECT_END		0x7f6f006f		/*o*/
#define		HESSIAN_STRING_CONTINUE	0x7f520052		/*R*/
#define		HESSIAN_STRING_END		0x7f530053		/*S*/
#define		HESSIAN_IS_TYPE			0x7f740074		/*t*/

#define		HESSIAN_REF_MAX			32

#define		HESSIAN_RETURN_VALUE	0x7f07f07f
#define		HESSIAN_ADD_REF			0x7f07e07e

#define		HessianReadNext			code = (unsigned char)*source;	result = HessianFunction[code](source, souend, target, ref)

union int64_union
{
	__int64	int64_sum;
	unsigned char int64_sep[8];
};

union int32_union
{
	long	int32_sum;
	unsigned char int32_sep[8];
};

typedef struct HessianRef
{
	int		namelen;
	char	name[NORMAL_CHAR];
	int		fieldNum;
}HessianRef;

typedef struct HessianRefs
{
	int		refNumber;
	int		shouldAdd;
	HessianRef ref[HESSIAN_REF_MAX];
}HessianRefs;

typedef long (*ProcessOneHessian)(char* &, char*, char* &, HessianRefs&);																//

long HessianString(char* &source, int len, char* &target, HessianRefs& ref);		
long HessianSystemString(char* &source, char* souend, char* &target, HessianRefs& ref);	//	for Method, List, Map name
long HessianSmallString(char* &source, char* souend, char* &target, HessianRefs& ref);		/*0x00-0x1f*/
long HessianChunkString(char* &source, char* souend, char* &target, HessianRefs& ref);		/*R*/
long HessianChunkEndString(char* &source, char* souend, char* &target, HessianRefs& ref);	/*S*/

long HessianSmallNumber(char* &source, char* souend, char* &target, HessianRefs& ref);		/*n*/
long HessianLargeNumber(char* &source, char* souend, char* &target, HessianRefs& ref);		/*l*/

long HessianSmallInt(char* &source, char* souend, char* &target, HessianRefs& ref);		/*0x80-0xbf*/
long HessianMiddleInt(char* &source, char* souend, char* &target, HessianRefs& ref);		/*0xc0-0xcf*/
long HessianLargeInt(char* &source, char* souend, char* &target, HessianRefs& ref);		/*0xd0-0xd7*/
long HessianHugeInt(char* &source, char* souend, char* &target, HessianRefs& ref);			/*I*/

long HessianSmallLong(char* &source, char* souend, char* &target, HessianRefs& ref);		/*0xd8-0xef*/
long HessianMiddleLong(char* &source, char* souend, char* &target, HessianRefs& ref);		/*0xf0-0xff*/
long HessianLargeLong(char* &source, char* souend, char* &target, HessianRefs& ref);		/*0x38-0x3f*/
long HessianHugeLong(char* &source, char* souend, char* &target, HessianRefs& ref);		/*Y*/
long HessianFullLong(char* &source, char* souend, char* &target, HessianRefs& ref);		/*L*/

long HessianSmallBinary(char* &source, char* souend, char* &target, HessianRefs& ref);		/*0x20-0x2f*/
long HessianTrue(char* &source, char* souend, char* &target, HessianRefs& ref);			/*T*/
long HessianFalse(char* &source, char* souend, char* &target, HessianRefs& ref);			/*F*/

long HessianCallStart(char* &source, char* souend, char* &target, HessianRefs& ref);		/*c*/
long HessianRelayStart(char* &source, char* souend, char* &target, HessianRefs& ref);		/*r*/
long HessianMethodStart(char* &source, char* souend, char* &target, HessianRefs& ref);		/*m*/
long HessianMapStart(char* &source, char* souend, char* &target, HessianRefs& ref);		/*M*/
long HessianObjectStart(char* &source, char* souend, char* &target, HessianRefs& ref);		/*O*/
long HessianListStart(char* &source, char* souend, char* &target, HessianRefs& ref);		/*V*/
long HessianType(char* &source, char* souend, char* &target, HessianRefs& ref);			/*t*/

long HessianObjectEnd(char* &source, char* souend, char* &target, HessianRefs& ref);		/*o*/
long HessianEnd(char* &source, char* souend, char* &target, HessianRefs& ref);				/*z*/
long HessianNull(char* &source, char* souend, char* &target, HessianRefs& ref);			/*N*/

long HessianNone(char* &source, char* souend, char* &target, HessianRefs& ref);

long HessianDecode(char* source, long slen, char* target, long &tlen);
long HessianEncode(char* source, long slen, char* target, long &tlen);


ProcessOneHessian HessianFunction[ASCII_NUMBER] = {
	HessianSmallString,			HessianSmallString,			HessianSmallString,			HessianSmallString,
	HessianSmallString,			HessianSmallString,			HessianSmallString,			HessianSmallString,
	HessianSmallString,			HessianSmallString,			HessianSmallString,			HessianSmallString,
	HessianSmallString,			HessianSmallString,			HessianSmallString,			HessianSmallString,

	HessianSmallString,			HessianSmallString,			HessianSmallString,			HessianSmallString,
	HessianSmallString,			HessianSmallString,			HessianSmallString,			HessianSmallString,
	HessianSmallString,			HessianSmallString,			HessianSmallString,			HessianSmallString,
	HessianSmallString,			HessianSmallString,			HessianSmallString,			HessianSmallString,

	HessianSmallBinary,	/*0x20*/HessianSmallBinary,			HessianSmallBinary,			HessianSmallBinary,
	HessianSmallBinary,			HessianSmallBinary,			HessianSmallBinary,			HessianSmallBinary,
	HessianSmallBinary,			HessianSmallBinary,			HessianSmallBinary,			HessianSmallBinary,
	HessianSmallBinary,			HessianSmallBinary,			HessianSmallBinary,			HessianSmallBinary,	/*0x2f*/

	HessianNone,				HessianNone,				HessianNone,				HessianNone,
	HessianNone,				HessianNone,				HessianNone,				HessianNone,
	HessianLargeLong,	/*0x38*/HessianLargeLong,			HessianLargeLong,			HessianLargeLong,
	HessianLargeLong,			HessianLargeLong,			HessianLargeLong,			HessianLargeLong,	/*0x3f*/

	HessianNone,				HessianNone,				HessianNone,				HessianNone,
	HessianNone,				HessianNone,				HessianFalse,		/*F*/	HessianNone,
	HessianNone,				HessianHugeInt,		/*I*/	HessianNone,				HessianNone,
	HessianFullLong,	/*L*/	HessianMapStart,	/*M*/	HessianNull,		/*N*/	HessianObjectStart,	/*O*/

	HessianNone,				HessianNone,				HessianChunkString,	/*R*/	HessianChunkEndString,/*S*/
	HessianTrue,		/*T*/	HessianNone,				HessianListStart,	/*V*/	HessianNone,
	HessianNone,				HessianHugeLong,	/*Y*/	HessianNone,				HessianNone,
	HessianNone,				HessianNone,				HessianNone,				HessianNone,

	HessianNone,				HessianNone,				HessianNone,				HessianCallStart,	/*c*/
	HessianFullLong,	/*d*/	HessianNone,				HessianNone,				HessianNone,
	HessianNone,				HessianNone,				HessianNone,				HessianNone,
	HessianLargeNumber,	/*l*/	HessianMethodStart,	/*m*/	HessianSmallNumber,	/*n*/	HessianObjectEnd,	/*o*/

	HessianNone,				HessianNone,				HessianRelayStart,	/*r*/	HessianNone,
	HessianType,		/*t*/	HessianNone,				HessianNone,				HessianNone,
	HessianNone,				HessianNone,				HessianEnd,			/*z*/	HessianNone,
	HessianNone,				HessianNone,				HessianNone,				HessianNone,

	HessianSmallInt,	/*0x80*/HessianSmallInt,			HessianSmallInt,			HessianSmallInt,
	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,
	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,
	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,

	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,
	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,
	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,
	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,

	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,
	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,
	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,
	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,

	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,
	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,
	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,
	HessianSmallInt,			HessianSmallInt,			HessianSmallInt,			HessianSmallInt,	/*0xbf*/

	HessianMiddleInt,	/*0xc0*/HessianMiddleInt,			HessianMiddleInt,			HessianMiddleInt,
	HessianMiddleInt,			HessianMiddleInt,			HessianMiddleInt,			HessianMiddleInt,
	HessianMiddleInt,			HessianMiddleInt,			HessianMiddleInt,			HessianMiddleInt,
	HessianMiddleInt,			HessianMiddleInt,			HessianMiddleInt,			HessianMiddleInt,	/*0xcf*/

	HessianLargeInt,	/*0xb0*/HessianLargeInt,			HessianLargeInt,			HessianLargeInt,
	HessianLargeInt,			HessianLargeInt,			HessianLargeInt,			HessianLargeInt,	/*0xb7*/
	HessianSmallLong,	/*0xd8*/HessianSmallLong,			HessianSmallLong,			HessianSmallLong,
	HessianSmallLong,			HessianSmallLong,			HessianSmallLong,			HessianSmallLong,

	HessianSmallLong,			HessianSmallLong,			HessianSmallLong,			HessianSmallLong,
	HessianSmallLong,			HessianSmallLong,			HessianSmallLong,			HessianSmallLong,
	HessianSmallLong,			HessianSmallLong,			HessianSmallLong,			HessianSmallLong,
	HessianSmallLong,			HessianSmallLong,			HessianSmallLong,			HessianSmallLong,	/*0xef*/

	HessianMiddleLong,	/*0xf0*/HessianMiddleLong,			HessianMiddleLong,			HessianMiddleLong,
	HessianMiddleLong,			HessianMiddleLong,			HessianMiddleLong,			HessianMiddleLong,
	HessianMiddleLong,			HessianMiddleLong,			HessianMiddleLong,			HessianMiddleLong,
	HessianMiddleLong,			HessianMiddleLong,			HessianMiddleLong,			HessianMiddleLong,	/*0xff*/
};

long HessianString(char* &source, int len, char* &target, HessianRefs& ref)
{
	char i;
	char* start = source;
	HessianRef *oneref;

	for (i=0; i<len; i++)
	{
		if ((*source & 0xf0) == 0xf0) source += 4;
		else if ((*source & 0xe0) == 0xe0) source += 3;
		else if ((*source & 0xc0) == 0xc0) source += 2;
		else if ((*source & 0x80) == 0) source += 1;
		else return VALUE_NOT_FOUND;
	}
	len = source - start;
	memcpy(target, start, len);
	target += len;
	if (ref.shouldAdd == HESSIAN_ADD_REF)
	{
		oneref = &(ref.ref[ref.refNumber]);
		oneref->namelen = len;
		memcpy(oneref->name, start, len);
	}
	return 0;
}

long HessianSystemString(char* &source, char* souend, char* &target, HessianRefs& ref)			//	for Method, List, Map name
{
	unsigned char val1 = *((unsigned char*)(source++));
	unsigned char val2 = *((unsigned char*)(source++));
	int len = (val1 << 8) + val2;
	if (souend-source < len) return VALUE_NOT_FOUND;
	return HessianString(source, len, target, ref);
}

long HessianSmallString(char* &source, char* souend, char* &target, HessianRefs& ref)		/*0x00-0x1f*/
{
	int len = (int)(*source++);
	return HessianString(source, len, target, ref);
}

long HessianChunkString(char* &source, char* souend, char* &target, HessianRefs& ref)		/*R*/
{
	unsigned char code;
	int result;
	source++;

	result = HessianSystemString(source, souend, target, ref);
	if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;

	HessianReadNext;
	if (result == HESSIAN_STRING_END) return HESSIAN_STRING_END;
	else if (result == HESSIAN_STRING_CONTINUE) return HESSIAN_STRING_CONTINUE;
	else return VALUE_NOT_FOUND;
}

long HessianChunkEndString(char* &source, char* souend, char* &target, HessianRefs& ref)		/*S*/
{
	int result;

	source++;
	result = HessianSystemString(source, souend, target, ref);
	if (result != VALUE_NOT_FOUND) return HESSIAN_STRING_END;
	else return VALUE_NOT_FOUND;
}


long HessianSmallNumber(char* &source, char* souend, char* &target, HessianRefs& ref)		/*n*/
{
	source++;
	char val = *(source++);
	MyString(NASZ("(Number:"), target);
	Myitoa(val, target);
	MyString(NASZ(")"), target);
	return 0;
}

long HessianLargeNumber(char* &source, char* souend, char* &target, HessianRefs& ref)		/*l*/
{
	MyString(NASZ("(Number:"), target);
	HessianHugeInt(source, souend, target, ref);
	MyString(NASZ(")"), target);
	return 0;
}

long HessianSmallInt(char* &source, char* souend, char* &target, HessianRefs& ref)			/*0x80-0xbf*/
{
	char val = *(source++) - 0x90;
	Myitoa(val, target);

	if (ref.shouldAdd == HESSIAN_ADD_REF) ref.ref[ref.refNumber].fieldNum = val;
	if (ref.shouldAdd == HESSIAN_RETURN_VALUE) return (long)val;
	return 0;
}

long HessianMiddleInt(char* &source, char* souend, char* &target, HessianRefs& ref)			/*0xc0-0xcf*/
{
	unsigned char val1 = *((unsigned char*)(source++));
	unsigned char val2 = *((unsigned char*)(source++));
	long val = (val1 << 8) + val2 - 0xc800;
	Myitoa(val, target);

	if (ref.shouldAdd == HESSIAN_ADD_REF) ref.ref[ref.refNumber].fieldNum = val;
	if (ref.shouldAdd == HESSIAN_RETURN_VALUE) return val;
	return 0;
}

long HessianLargeInt(char* &source, char* souend, char* &target, HessianRefs& ref)			/*0xd0-0xd7*/
{
	int32_union int32_val;

	int32_val.int32_sep[3] = 0;
	int32_val.int32_sep[2] = *((unsigned char*)(source++));
	int32_val.int32_sep[1] = *((unsigned char*)(source++));
	int32_val.int32_sep[0] = *((unsigned char*)(source++));
	int32_val.int32_sum -= 0xd40000;
	Myitoa(int32_val.int32_sum, target);

	if (ref.shouldAdd == HESSIAN_ADD_REF) ref.ref[ref.refNumber].fieldNum = int32_val.int32_sum;
	if (ref.shouldAdd == HESSIAN_RETURN_VALUE) return int32_val.int32_sum;
	return 0;
}

long HessianHugeInt(char* &source, char* souend, char* &target, HessianRefs& ref)			/*I*/
{
	int32_union int32_val;

	source++;
	int32_val.int32_sep[3] = *((unsigned char*)(source++));
	int32_val.int32_sep[2] = *((unsigned char*)(source++));
	int32_val.int32_sep[1] = *((unsigned char*)(source++));
	int32_val.int32_sep[0] = *((unsigned char*)(source++));
	Myitoa(int32_val.int32_sum, target);

	if (ref.shouldAdd == HESSIAN_ADD_REF) ref.ref[ref.refNumber].fieldNum = int32_val.int32_sum;
	if (ref.shouldAdd == HESSIAN_RETURN_VALUE) return int32_val.int32_sum;
	return 0;
}

long HessianSmallLong(char* &source, char* souend, char* &target, HessianRefs& ref)			/*0xd8-0xef*/
{
	char val = *(source++) - 0xe0;
	Myitoa(val, target);

	if (ref.shouldAdd == HESSIAN_ADD_REF) ref.ref[ref.refNumber].fieldNum = val;
	if (ref.shouldAdd == HESSIAN_RETURN_VALUE) return (long)val;
	return 0;
}

long HessianMiddleLong(char* &source, char* souend, char* &target, HessianRefs& ref)			/*0xf0-0xff*/
{
	unsigned char val1 = *((unsigned char*)(source++));
	unsigned char val2 = *((unsigned char*)(source++));
	long val = (val1 << 8) + val2 - 0xf800;
	Myitoa(val, target);

	if (ref.shouldAdd == HESSIAN_ADD_REF) ref.ref[ref.refNumber].fieldNum = val;
	if (ref.shouldAdd == HESSIAN_RETURN_VALUE) return val;
	return 0;
}

long HessianLargeLong(char* &source, char* souend, char* &target, HessianRefs& ref)			/*0x38-0x3f*/
{
	int32_union int32_val;

	int32_val.int32_sep[3] = 0;
	int32_val.int32_sep[2] = *((unsigned char*)(source++));
	int32_val.int32_sep[1] = *((unsigned char*)(source++));
	int32_val.int32_sep[0] = *((unsigned char*)(source++));
	int32_val.int32_sum -= 0x3b0000;
	Myitoa(int32_val.int32_sum, target);

	if (ref.shouldAdd == HESSIAN_ADD_REF) ref.ref[ref.refNumber].fieldNum = int32_val.int32_sum;
	if (ref.shouldAdd == HESSIAN_RETURN_VALUE) return int32_val.int32_sum;
	return 0;
}

long HessianHugeLong(char* &source, char* souend, char* &target, HessianRefs& ref)			/*Y*/
{
	int32_union int32_val;

	source++;
	int32_val.int32_sep[3] = *((unsigned char*)(source++));
	int32_val.int32_sep[2] = *((unsigned char*)(source++));
	int32_val.int32_sep[1] = *((unsigned char*)(source++));
	int32_val.int32_sep[0] = *((unsigned char*)(source++));
	Myitoa(int32_val.int32_sum, target);

	if (ref.shouldAdd == HESSIAN_ADD_REF) ref.ref[ref.refNumber].fieldNum = int32_val.int32_sum;
	if (ref.shouldAdd == HESSIAN_RETURN_VALUE) return int32_val.int32_sum;
	return 0;
}

long HessianFullLong(char* &source, char* souend, char* &target, HessianRefs& ref)			/*L*/	/*d*/
{
	int64_union	int64_val;

	source++;
	int64_val.int64_sep[7] = *((unsigned char*)(source++));
	int64_val.int64_sep[6] = *((unsigned char*)(source++));
	int64_val.int64_sep[5] = *((unsigned char*)(source++));
	int64_val.int64_sep[4] = *((unsigned char*)(source++));
	int64_val.int64_sep[3] = *((unsigned char*)(source++));
	int64_val.int64_sep[2] = *((unsigned char*)(source++));
	int64_val.int64_sep[1] = *((unsigned char*)(source++));
	int64_val.int64_sep[0] = *((unsigned char*)(source++));
	Myitoa(int64_val.int64_sum, target);
// 	if (ref.shouldAdd) ref.ref[ref.refNumber].fieldNum = val;
	return 0;
}

long HessianSmallBinary(char* &source, char* souend, char* &target, HessianRefs& ref)		/*0x20-0x2f*/
{	
	char len = *(source++) - 0x20;
	memcpy(target, source, len);
	source += len;
	target += len;
	return 0;
}

long HessianTrue(char* &source, char* souend, char* &target, HessianRefs& ref)				/*T*/
{
	source++;
	MyString(NASZ("TRUE"), target);
	return 0;
}

long HessianFalse(char* &source, char* souend, char* &target, HessianRefs& ref)				/*F*/
{
	source++;
	MyString(NASZ("FALSE"), target);
	return 0;
}

long HessianCallStart(char* &source, char* souend, char* &target, HessianRefs& ref)			/*c*/
{
	source++;
	MyString(NASZ("HessianCall Ver "), target);
	Myitoa(*source++, target);
	MyString(NASZ("."), target);
	Myitoa(*source++, target);
	MyString(NASZ(" "), target);
	return 0;
}

long HessianRelayStart(char* &source, char* souend, char* &target, HessianRefs& ref)			/*r*/
{
	source++;
	MyString(NASZ("HessianRelay Ver "), target);
	Myitoa(*source++, target);
	MyString(NASZ("."), target);
	Myitoa(*source++, target);
	MyString(NASZ(" "), target);
	return 0;
}


long HessianMethodStart(char* &source, char* souend, char* &target, HessianRefs& ref)		/*m*/
{
	unsigned char code;

	source++;
	int result = HessianSystemString(source, souend, target, ref);
	if (result) return VALUE_NOT_FOUND;
	MyString(NASZ("("), target);

	do 
	{
		HessianReadNext;
		if (result == HESSIAN_METHOD_END) break;
		if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;
		MyString(NASZ(","), target);
	} while (source < souend);

	if (result != HESSIAN_METHOD_END) return VALUE_NOT_FOUND;
	if (*(target-1) == ',') target--;
	MyString(NASZ(")"), target);

	return 0;
}

long HessianObjectStart(char* &source, char* souend, char* &target, HessianRefs& ref)		/*O*/
{
	unsigned char code;
	int result;
	HessianRef* oneref;

	source++;
	MyString(NASZ("Object["), target);

	oneref = &(ref.ref[ref.refNumber]);

	oneref->fieldNum = 0;
	oneref->namelen = 0;
	ref.shouldAdd = HESSIAN_ADD_REF;

	MyString(NASZ("Name="), target);
	HessianReadNext;
	if (!oneref->namelen) return VALUE_NOT_FOUND;

	MyString(NASZ(",FieldNum="), target);
	HessianReadNext;
	if (!oneref->fieldNum) return VALUE_NOT_FOUND;
	MyString(NASZ(","), target);

	ref.shouldAdd = 0;
	ref.refNumber++;

	do 
	{
		HessianReadNext;
		if (result == HESSIAN_OBJECT_END) break;
		if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;
		MyString(NASZ(","), target);
	} while (source < souend);

	if (result != HESSIAN_OBJECT_END) return VALUE_NOT_FOUND;
	if (*(target-1) == ',') target--;
	MyString(NASZ("]"), target);

// 	do 
// 	{
//	HessianReadNext;
// 		if (result == HESSIAN_METHOD_END) break;
// 		if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;
// 		MyString(NASZ(","), target);
// 	} while (source < souend);
// 
// 	if (result != HESSIAN_METHOD_END) return VALUE_NOT_FOUND;
// 	if (*(target-1) == ',') target--;
// 	MyString(NASZ("]"), target);

	if (result == HESSIAN_OBJECT_END) return HESSIAN_OBJECT_END;
	return 0;
}

long HessianMapStart(char* &source, char* souend, char* &target, HessianRefs& ref)			/*M*/
{
	unsigned char code;
	int result;

	source++;
	MyString(NASZ("Map["), target);
	do 
	{
		HessianReadNext;
		if (result == HESSIAN_METHOD_END) break;
		if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;

		else if (result != HESSIAN_IS_TYPE)
		{
			MyString(NASZ("="), target);
			HessianReadNext;
			if (result == HESSIAN_METHOD_END) break;
			if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;
			MyString(NASZ(","), target);
		}
	} while (source < souend);

	if (result != HESSIAN_METHOD_END) return VALUE_NOT_FOUND;
	if (*(target-1) == ',') target--;
	MyString(NASZ("]"), target);

	return 0;
}

long HessianListStart(char* &source, char* souend, char* &target, HessianRefs& ref)			/*V*/
{
	unsigned char code;
	int result;
	source++;
	MyString(NASZ("List["), target);
	int i, fNum;

	do 
	{
		HessianReadNext;
// 		if (result == HESSIAN_METHOD_END) break;
		if (result == HESSIAN_OBJECT_END) break;
		MyString(NASZ(","), target);
	} while (source < souend);

	if (result == HESSIAN_OBJECT_END)
	{
		MyString(NASZ("]=["), target);
		do 
		{

			ref.shouldAdd = HESSIAN_RETURN_VALUE;
			HessianReadNext;
			ref.shouldAdd = 0;

			if (result >= ref.refNumber) return VALUE_NOT_FOUND;
			fNum = ref.ref[result].fieldNum;
			MyString(NASZ("["), target);
			for (i=0; i<fNum; i++)
			{
				HessianReadNext;
				if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;
				MyString(NASZ(","), target);
			}

			HessianReadNext;
			if (result == HESSIAN_METHOD_END)
			{
				if (*(target-1) == ',') target--;
				MyString(NASZ("]]"), target);
				return HESSIAN_METHOD_END;
			}
			else if (result == HESSIAN_OBJECT_END)
			{
				if (*(target-1) == ',') target--;
				MyString(NASZ("],"), target);
			}
			else
			{
				return VALUE_NOT_FOUND;
			}

		} while (source < souend);
	}
// 	do 
// 	{
// 
// // 		if (result == HESSIAN_METHOD_END) break;
// 		if (result == HESSIAN_OBJECT_END) break;
// // 		if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;
// // 
// // 		else if (result != HESSIAN_IS_TYPE)
// // 		{
// // 			MyString(NASZ("="), target);
// // 			code = *source;
// // 			result = HessianFunction[code](source, souend, target);
// // 			if (result == HESSIAN_METHOD_END) break;
// // 			if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;
// 			MyString(NASZ(","), target);
// // 		}
// 	} while (source < souend);
// 
// 	}

	if (result != HESSIAN_METHOD_END) return VALUE_NOT_FOUND;

	if (*(target-1) == ',') target--;
	MyString(NASZ("]"), target);

	return 0;
}

long HessianType(char* &source, char* souend, char* &target, HessianRefs& ref)				/*t*/
{
	source++;
	MyString(NASZ("Type:"), target);

	int result = HessianSystemString(source, souend, target, ref);
	if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;
	MyString(NASZ(","), target);
	return HESSIAN_IS_TYPE;
}

long HessianObjectEnd(char* &source, char* souend, char* &target, HessianRefs& ref)			/*o*/
{
	source++;
	return HESSIAN_OBJECT_END;
}


long HessianEnd(char* &source, char* souend, char* &target, HessianRefs& ref)				/*z*/
{
	source++;
	return HESSIAN_METHOD_END;
}

long HessianNull(char* &source, char* souend, char* &target, HessianRefs& ref)				/*N*/
{
	source++;
	return 0;
}

long HessianNone( char* &source, char* souend, char* &target, HessianRefs& ref)
{
	__asm int 3
	return 0;
}

long HessianDecode(char* source, long slen, char* target, long &tlen)
{
	char* tstart = target;
	char* end = source+slen;
	unsigned char code;
	long result;
	HessianRefs ref;

	ref.refNumber = 0;
	ref.shouldAdd = 0;
	tlen = 0;
	do 
	{
		code = (unsigned char)*source;
		result = HessianFunction[code](source, end, target, ref);
		if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;
	} while (source <= end);
	tlen = target - tstart;

	return 0;
}

long HessianEncode(char* source, long slen, char* target, long &tlen)
{
	return 0;
}

#define BUFFERSIZE 2048*1024
char soubuffer[BUFFERSIZE];
char tarbuffer[BUFFERSIZE];

// char testbuffer[BUFFERSIZE] = {'R', 0, 2, 'a', 'b', 0x90, 0, 3, 'a', 'b', 'c', 'S', 0, 1, 'z'};

// int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
// {
// 	char* cpointer = soubuffer;
// 	long* lpointer;
// 	long numtran;
// 	long result;
// 
// //	miss r2.txt
// 	FILE*   fileHandle = fopen("z:\\develop\\r3.txt", "rb");
// 	int numread = fread( soubuffer, 1, BUFFERSIZE, fileHandle );
// 
// 	for (int i=0; i<numread; i++)
// 	{
// 		lpointer = (long*)cpointer;
// 		if (*lpointer != 0x0a0d0a0d) cpointer++;
// 		else
// 		{
// 			cpointer += 4;
// 			result = HessianDecode(cpointer, numread-(cpointer-soubuffer), tarbuffer, numtran);
// 			break;
// 		}
// 	}
// 
// 	FILE* filew = fopen("z:\\develop\\t.txt", "wb");
// 	fwrite(tarbuffer, 1, numtran, filew);
// 	fclose(filew);
// 
// 	int aaa = 1;
// 	return 0;
// 
// }