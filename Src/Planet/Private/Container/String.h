#pragma once

#include "../Platform/Platform.h"
#include <cstdarg>

// TODO: Rewrite this in terms of some array class
class String
{
public:

	// Constructor and Destructor
	String();
	String(const String& other);
	String(String&& other) = default;

	template<typename CharType>
	String(const CharType* charArray)
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

	String(unsigned char c);

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
		mData[mLength] = c;
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

public:
	// Member functions

	inline int Length() const { return mLength; }

	inline void Empty() { ResizeTo(0); }

	void Replace(int startIdx, int endIdx, const String& string);

private:

	void ResizeTo(int NewSize);

private:

	TCHAR* mData = nullptr;

	// length of the underlying string, one less than the length of the data array
	int mLength = 0;
};