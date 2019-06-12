#include "String.h"
#include <cstdlib>

String::String()
{
	Empty();
}

String::String(const String& other)
{
	ResizeTo(other.Length());
	memcpy(mData, other.mData, (other.Length() + 1) * sizeof(TCHAR));
}

String::String(unsigned char c)
{
	ResizeTo(1);
	mData[0] = c;
}

String::~String()
{
	free(mData);
}

String String::PrintfInternal(TCHAR* Format, ...)
{
	va_list args;
	va_start(args, Format);

	String outputString{ Format };

	String formatString;
	int insertIdx = 0;
	for (int i = 0; i < outputString.Length(); ++i)
	{
		const TCHAR& c = outputString[i];

		if (formatString.Length() > 0)
		{
			formatString += c;

			if (c == 's')
			{
				outputString.Replace(insertIdx, i, va_arg(args, String));
			}
			else if (c == 'c')
			{
				outputString.Replace(insertIdx, i, String{ va_arg(args, unsigned char) });
			}
			else if (c == 'p')
			{
				const char* strptr = va_arg(args, const char*);
				outputString.Replace(insertIdx, i, String{ strptr });
			}
			
			formatString.Empty();
		}
		else if (c == '%')
		{
			formatString += c;
			insertIdx = i;
		}
	}

	va_end(args);

	return outputString;
}

void String::Replace(int startIdx, int endIdx, const String& string)
{
	int replacementLength = (endIdx - startIdx) + 1;
	int lengthDifference = string.Length() - replacementLength;

	if (lengthDifference > 0)
	{
		int copyLen = mLength - endIdx - 1;
		ResizeTo(mLength + lengthDifference);

		for(int i = 1; i <= copyLen; ++i)
		{
			mData[(mLength - copyLen - 1) + i] = mData[i + endIdx];
		}
	}

	memcpy(mData + startIdx, string.mData, string.Length() * sizeof(TCHAR));

	if (lengthDifference < 0)
	{
		int newLen = mLength + lengthDifference;
		for (int i = endIdx + lengthDifference + 1; i <= newLen; ++i)
		{
			mData[i] = mData[i - lengthDifference];
		}

		ResizeTo(newLen);
	}
}

void String::ResizeTo(int NewLength)
{
	if (NewLength < 0) NewLength = 0;

	mData = (TCHAR*)realloc(mData, NewLength + 1);
	mData[NewLength] = '\0';
	mLength = NewLength;
}