//"BeardGeometry.h" would be a better name

#pragma once

//3-vectors utils
void mulvecV(const float*, const float*, float*); //computes the vector product of two 3-vectors
float mulvecS(const float*, const float*); //computes the scalar product of two 3-vectors
float mod3(const float*); //computes the euclidean module of a 3-vector
void norm(float*); //normalizes a 3-vector
float distance(const float*, const float*); //computes the euclidean distance
void triNorm(float*, const float*, const float*, const float*); //computes the normal to the given triangle using right hand rule
float cosV(const float*, const float*); //computes the cos of the angle between two vectors

/*mat1, mat2, res, n, m, o
 *mat1 n * m matrix
 *mat2 m * o matrix
 *res n * o matrix
 */
void mulmat(const float*, const float*, float*, int, int, int);

//return various 4x4 matrices
void identity(float*);
void translate(float*, float, float, float);
void rotateX(float*, float); //radians angles here
void rotateY(float*, float);
void rotateZ(float*, float);
void scale(float*, float, float, float);

//pos, center, up
void camera(float*, float, float, float, float, float, float, float, float, float);

//FOV, screen ratio, near, far
void perspective(float*, float, float, float, float);

//dest, src, cnt, matrix
//void applyMatrix(float*, const float*, int, const float*);
void applyMatrixToVec3(float*, const float*, int, const float*);
