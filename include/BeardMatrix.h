#pragma once

void mulvecV(const float*, const float*, float*);
float mulvecS(const float*, const float*);

/*mat1, mat2, res, n, m, o
 *mat1 n * m matrix
 *mat2 m * o matrix
 *res n * o matrix
 */
void mulmat(const float*, const float*, float*, int, int, int);

//return various 4x4 matrices
void identity(float*);
void translate(float*, float, float, float);
void rotateX(float*, float);
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
