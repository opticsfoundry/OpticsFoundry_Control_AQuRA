#pragma once
class PixelFont
{
public:
	unsigned char *Pixels;
	unsigned int Length;
private:
	unsigned int GetCharacterTableEntry(unsigned char character);
public:
	PixelFont(const CString text);
	~PixelFont(void);
};

