#include "stdafx.h"
#include "Mesh3D.h"
#include <fstream>
using namespace std;

CMesh3D::CMesh3D(CString Filename)
{
	NrPoints=0;
	x=NULL;
	y=NULL;
	z=NULL;
	if (Filename!="") {
		ifstream in(Filename);
		if (!in.is_open()) {
			ControlMessageBox("Mesh3D::Mesh3D : file "+Filename+" not found");
			return;
		}
		char StartString[1024];
		double Value;
		int xIndex=0;
		int yIndex=1;
		int zIndex=2;
		double xSign=1.0;
		double ySign=1.0;
		double zSign=1.0;
		bool UpInfo=false;
		in.getline(StartString,1024,' ');
		CString help=StartString;
		if (help=="UP") {
			UpInfo=true;
			in>>xIndex;
			if (xIndex<-3) xIndex=-3;
			if (xIndex>3) xIndex=3;
			if (xIndex==0) xIndex=1;
			xSign=(xIndex<0) ? -1.0 : 1.0;
			xIndex=abs(xIndex)-1;
			in>>yIndex;
			if (yIndex<-3) yIndex=-3;
			if (yIndex>3) yIndex=3;
			if (yIndex==0) yIndex=1;
			ySign=(yIndex<0) ? -1.0 : 1.0;			
			yIndex=abs(yIndex)-1;
			in>>zIndex;
			if (zIndex<-3) zIndex=-3;
			if (zIndex>3) zIndex=3;
			if (zIndex==0) zIndex=1;
			zSign=(zIndex<0) ? -1.0 : 1.0;			
			zIndex=abs(zIndex)-1;
		} else {
			in>>Value;
			in>>Value;
			NrPoints++;
		}
		while (!in.eof()) {
			in>>Value;
			in>>Value;
			in>>Value;
			NrPoints++;
		}
		in.close();
		if (NrPoints==0) {
			ControlMessageBox("Mesh3D::Mesh3D : file "+Filename+" empty");
			return;
		}
		x=new double[NrPoints];
		y=new double[NrPoints];
		z=new double[NrPoints];
		int Nr=0;
		ifstream in2(Filename);
		if (UpInfo) in2.getline(StartString,1024);
		double MaxX=-999;
		double MinX=999;
		double MaxY=-999;
		double MinY=999;
		double MaxZ=-999;
		double MinZ=999;
		double InVector[3];
		while (!in2.eof()) {
			for (int i=0;i<3;i++) in2>>InVector[i];
			x[Nr]=xSign*InVector[xIndex];
			y[Nr]=ySign*InVector[yIndex];
			z[Nr]=zSign*InVector[zIndex];
			if (x[Nr]>MaxX) MaxX=x[Nr];
			if (x[Nr]<MinX) MinX=x[Nr];			
			if (y[Nr]>MaxY) MaxY=y[Nr];
			if (y[Nr]<MinY) MinY=y[Nr];			
			if (z[Nr]>MaxZ) MaxZ=z[Nr];
			if (z[Nr]<MinZ) MinZ=z[Nr];
			Nr++;
		}
		in2.close();
		if (Nr!=NrPoints) ControlMessageBox("Mesh3D::Mesh3D : weird error");
		double SizeX=MaxX-MinX;
		double SizeY=MaxY-MinY;
		double SizeZ=MaxZ-MinZ;
		double MaxSize=SizeX;
		if (SizeY>MaxSize) MaxSize=SizeY;
		if (SizeZ>MaxSize) MaxSize=SizeZ;
		double Scale=1.95/MaxSize;
		double CentreX=0.5*(MaxX+MinX);
		double CentreY=0.5*(MaxY+MinY);
		double CentreZ=0.5*(MaxZ+MinZ);
		for (int Nr=0;Nr<NrPoints;Nr++) {
			x[Nr]=Scale*(x[Nr]-CentreX);
			y[Nr]=Scale*(y[Nr]-CentreY);
			z[Nr]=Scale*(z[Nr]-CentreZ);
		}
	}
}

CMesh3D::~CMesh3D(void)
{
	if (x) delete[] x;
	if (z) delete[] y;
	if (y) delete[] z;
}
