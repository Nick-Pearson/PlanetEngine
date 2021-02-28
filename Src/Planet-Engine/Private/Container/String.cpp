#include "String.h"

#include <stdio.h>

#include <string>
#include <cstdlib>

String::~String()
{
    free(mData);
}

#pragma optimize("", off)
String String::PrintfInternal(TCHAR* Format, ...)
{
    va_list args;
    va_start(args, Format);

    const int BufferSize = 256;
    TCHAR* Buff = new TCHAR[BufferSize];

    int result = vsnprintf(Buff, BufferSize - 1, Format, args);

    va_end(args);

    String str = String{ Buff };
    delete Buff;
    return str;
}

String String::FromFloat(float value)
{
    int wholePart = static_cast<int>(value);
    float decimalPart = value - static_cast<float>(wholePart);

    String str = String::FromInt(wholePart);

    int intDecimalPart = 0;

    const int maxPlaces = 8;
    for (int i = 0; i < maxPlaces; ++i)
    {
        decimalPart *= 10.0f;
        intDecimalPart *= 10;

        intDecimalPart += static_cast<int>(decimalPart);
    }

    str += '.';
    str += String::FromInt(intDecimalPart);

    return str;
}

String String::FromInt(int value)
{
    String outStr;

    if (value < 0)
        outStr += '-';

    int valCpy = value;
    do
    {
        outStr += (valCpy % 10) + '0';
        valCpy /= 10;
    } while (valCpy != 0);

    if (value < 0)
        outStr += '-';

    outStr.Reverse();

    return outStr;
}

void String::Replace(int startIdx, int endIdx, const String& string)
{
    int replacementLength = (endIdx - startIdx) + 1;
    int lengthDifference = string.Length() - replacementLength;

    if (lengthDifference > 0)
    {
        int copyLen = mLength - endIdx - 1;
        ResizeTo(mLength + lengthDifference);

        for (int i = 1; i <= copyLen; ++i)
        {
            mData[mLength - i] = mData[endIdx + copyLen + 1 - i];
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

void String::Reverse()
{
    int halfLen = static_cast<int>(mLength / 2.0f);
    for (int i = 0; i < halfLen; ++i)
    {
        TCHAR tmp = mData[i];
        mData[i] = mData[mLength - i - 1];
        mData[mLength - i - 1] = tmp;
    }
}
#pragma optimize("", on)

void String::ResizeTo(int NewLength)
{
    if (NewLength < 0) NewLength = 0;

    mData = reinterpret_cast<TCHAR*>(realloc(mData, NewLength + 1));
    mData[NewLength] = '\0';
    mLength = NewLength;
}