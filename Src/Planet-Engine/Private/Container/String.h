#pragma once

#include <cstdarg>

#include "../Platform/Platform.h"

// TODO: Rewrite this in terms of some array class
class String
{
 public:
    // Constructor and Destructor
    String()
    {
        Empty();
    }

    String(const String& other)
    {
        *this = other;
    }

    String(String&& other)
    {
        *this = other;
    }

    template<typename CharType>
    explicit String(const CharType* charArray)
    {
        int len = 0;
        const CharType* charPtr = charArray;
        while (*charPtr != 0)
        {
            ++len;
            ++charPtr;
        }

        ResizeTo(len);

        if (sizeof(TCHAR) == sizeof(CharType))
        {
            memcpy(mData, charArray, (len + 1) * sizeof(TCHAR));
        }
        else
        {
            for (int i = 0; i <= len; ++i)
            {
                mData[i] = (TCHAR)charArray[i];
            }
        }
    }

    explicit String(unsigned char c)
    {
        ResizeTo(1);
        mData[0] = c;
    }

    ~String();


    // Operators

    const TCHAR* operator*() const
    {
        return mData;
    }

    TCHAR& operator[](int idx)
    {
        return mData[idx];
    }

    const TCHAR& operator[](int idx) const
    {
        return mData[idx];
    }

    void operator+=(TCHAR c)
    {
        ResizeTo(mLength + 1);
        mData[mLength-1] = c;
    }

    void operator+=(const String& other)
    {
        ResizeTo(mLength + other.Length());
        memcpy(mData + (mLength - other.Length()), other.mData, other.Length());
    }

    String& operator=(String&& other)
    {
        mData = other.mData;
        mLength = other.mLength;

        other.mData = nullptr;
        other.ResizeTo(0);
        return *this;
    }

    String& operator=(const String& other)
    {
        ResizeTo(other.Length());
        memcpy(mData, other.mData, other.Length() + 1 * sizeof(TCHAR));
        return *this;
    }

 public:
    // statics:

    static inline String Printf(TCHAR* Format)
    {
        return String{ Format };
    }

    template<typename... Args>
    static inline String Printf(TCHAR* Format, Args... args)
    {
        return PrintfInternal(Format, args...);
    }

    static String PrintfInternal(TCHAR* Format, ...);

    static String FromFloat(float value);
    static String FromInt(int value);

 public:
    // Member functions

    inline int Length() const { return mLength; }

    inline void Empty() { ResizeTo(0); }

    void Replace(int startIdx, int endIdx, const String& string);

    void Reverse();

 private:
    void ResizeTo(int NewSize);

 private:
    TCHAR* mData = nullptr;

    // length of the underlying string, one less than the length of the data array
    int mLength = 0;
};