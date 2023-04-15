#include <atlstr.h>

class TextPrinter
{
private:
	const double rate = 0.2;
	double internalTime = 0;
	CString str;
	CString strCopy;
	CString current;
public:
	bool isHolding = false;

	TextPrinter(CString str)
	{
		this->str = str;
		strCopy = str;
	}

	void Update(double dt)
	{
		internalTime += dt;
		while (true)
		{
			if (strCopy.GetLength() == 0) break;
			CString s = strCopy[0];
			current += s;
			strCopy.Delete(0, 1);

			if (s != " " || s != "") {}
		}
	}

	CString GetCurrent()
	{
		return current;
	}
};