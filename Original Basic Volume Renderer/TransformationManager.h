/*
TransformationManager.h
Anurag Arnab - 10 January 2014

Header for class which manages the rotation matrix.
It also changes the OpenGL rotation state accordingly
The rotation matrix is updated according to keyboard or mouse input
*/

#ifndef TRANMAN_H
#define TRANMAN_H_

class TransformationManager
{
public:
	TransformationManager(void);
	virtual ~TransformationManager(void);
	const double* GetMatrix()
	{
		return mdRotation;
	}

	// Call these only after the OpenGL is initialized.
	void Rotate(float fx_i, float fy_i, float fz_i);
	void ResetRotation();

private:

	float mfRot[3];
	double mdRotation[16];
};

#endif