#include "stdafx.h"
#include "utilities.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString itos(long i) {
	CString help;
	help.Format("%i",i);
	return help;
}

double Max(double a, double b) {
	return (a<b)? b:a;
}

double Min(double a, double b) {
	return (a>b)? b:a;
}