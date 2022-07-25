// HammUEr
// Copyright 2015-2021 Turfster / NT Entertainment
// All Rights Reserved.


// This code is held together with kitten whiskers and toothpicks.
// There's nothing to see or learn here.
// Seriously.
// Turn back.

#pragma once
#include <cmath>

#define tSMALL_NUMBER (1.e-8)
#define tKINDA_SMALL_NUMBER	(1.e-4)
#define f_PI (3.1415926535897932f)


struct MAPColor
{
public:
	int R;
	int G;
	int B;
	int A;

	const static MAPColor White;

	MAPColor();
	MAPColor(int inR, int inG, int inB);
	MAPColor(int inR, int inG, int inB, int inA);
	MAPColor(float inR, float inG, float inB, float inA);
};

struct MAPVector
{
public:
	double X;
	double Y;
	double Z;

	static const MAPVector ZeroVector;
	static const MAPVector UpVector;
	static const MAPVector ForwardVector;
	static const MAPVector RightVector;

	MAPVector();
	MAPVector(double x, double y, double z);
	static double DistSquared(const MAPVector& V1, const MAPVector& V2);
	bool operator==(const MAPVector& V) const;
	MAPVector operator*(double Scale) const;
	MAPVector operator*(const MAPVector& V) const;
	bool operator!=(const MAPVector& V1)const;
	MAPVector operator-() const;
	double Size() const;
	double& operator[] (int index);
	double operator[] (int index) const;
	MAPVector operator*=(const double Scale);
	MAPVector operator/(double Scale) const;
	MAPVector operator/=(const double Scale);
	static double Dist(const MAPVector& V1, const MAPVector& V2);
	double SizeSquared() const;
	MAPVector operator+(MAPVector v2) const;
	MAPVector operator+=(const MAPVector v2);
	MAPVector operator-(MAPVector v2) const;
	static double DotProduct(const MAPVector& A, const MAPVector& B);
	static MAPVector CrossProduct(const MAPVector A, const MAPVector B);
	bool Normalize(double Tolerance = tSMALL_NUMBER);
	MAPVector GetSafeNormal(double Tolerance = tSMALL_NUMBER) const;
	static bool PointsAreNear(const MAPVector& Point1, const MAPVector& Point2, double Dist);
	MAPVector Absolute() const;
};

struct MAPVector2D
{
public:
	double X;
	double Y;

	static const MAPVector2D ZeroVector;

	MAPVector2D();
	MAPVector2D(double x, double y);
	double& operator[] (int index);
	double operator[] (int index) const;
	bool operator==(const MAPVector2D& V) const;
	MAPVector2D operator-() const;
	MAPVector2D operator*(double Scale) const;
	MAPVector2D operator+(const MAPVector2D& V) const;
	MAPVector2D operator-(const MAPVector2D& V) const;
	MAPVector2D operator+=(const MAPVector2D V);
	MAPVector2D operator/(double Scale) const;
	MAPVector2D operator/=(const double Scale);
	static double DotProduct(const MAPVector2D& A, const MAPVector2D& B);
};


struct MAPQuat
{
public:
	double X;
	double Y;
	double Z;
	double W;

	static const MAPQuat Identity;
	MAPQuat();
	MAPQuat(double inX, double inY, double inZ, double inW);
	MAPQuat(MAPVector axis, double angle);
	MAPVector RotateVector(MAPVector V) const;
	MAPVector operator*(const MAPVector v) const;
	MAPQuat operator*(const double Scale);
};

struct MAPMatrix
{
public:
	double M[4][4];

	MAPMatrix();
	MAPMatrix(double a1, double a2, double a3, double a4, double b1, double b2, double b3, double b4, double c1, double c2, double c3, double c4, double d1, double d2, double d3, double d4);
	double Determinant();
};