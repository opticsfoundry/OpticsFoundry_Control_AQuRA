#pragma once
class CMesh3D
{
public:
	long NrPoints;
	double* x;
	double* y;
	double* z;
	CMesh3D(CString Filename);
	~CMesh3D(void);
};

