#pragma once
struct TextComparer
{
public:
	bool operator()(CComBSTR x, CComBSTR y) const noexcept
	{
		return x.CompareTo(y, true) == -1;
	}
};