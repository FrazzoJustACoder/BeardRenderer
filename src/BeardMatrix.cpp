#include "BeardMatrix.h"
#include "lib.h"
#include <math.h>

//computes a 3x3 matrix' discriminant
float det3(const float *m) {
	return
	+ m[0] * (m[4] * m[8] - m[5] * m[7])
	+ m[1] * (m[5] * m[6] - m[3] * m[8])
	+ m[2] * (m[3] * m[7] - m[4] * m[6]);
}

//computes the module of a 3-vector
float mod3(const float *v) {
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

//normalizes a 3-vector
void norm(float* v) {
	float d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}

//computes the vector product of two 3-vectors
void mulvecV(const float *a, const float *b, float *c) {
	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = a[2] * b[0] - a[0] * b[2];
	c[2] = a[0] * b[1] - a[1] * b[0];
}

//computes the scalar product of two 3-vectors
float mulvecS(const float *a, const float *b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void mulmat(const float *a, const float *b, float *c, int n, int m, int o) {
	float s;
	for(int i = 0; i < n; i++)
		for(int j = 0; j < o; j++) {
			s = 0.0f;
			for(int k = 0; k < m; k++)
				s += a[i * m + k] * b[k * o + j];
			c[i * o + j] = s;
		}
}

const float _identity[16] = {
1.0f, 0.0f, 0.0f, 0.0f,
0.0f, 1.0f, 0.0f, 0.0f,
0.0f, 0.0f, 1.0f, 0.0f,
0.0f, 0.0f, 0.0f, 1.0f
};

void identity(float *d) {
	strncpy(d, _identity, 64);
}

void translate(float *d, float x, float y, float z) {
	strncpy(d, _identity, 64);
	d[3] = x;
	d[7] = y;
	d[11] = z;
}

void rotateX(float *d, float a) {
	strncpy(d, _identity, 64);
	float s = sin(a), c = cos(a);
	d[5] = c;
	d[10] = c;
	d[6] = -s;
	d[9] = s;
}

void rotateY(float *d, float a) {
	strncpy(d, _identity, 64);
	float s = sin(a), c = cos(a);
	d[0] = c;
	d[10] = c;
	d[8] = -s;
	d[2] = s;
}

void rotateZ(float *d, float a) {
	strncpy(d, _identity, 64);
	float s = sin(a), c = cos(a);
	d[0] = c;
	d[5] = c;
	d[1] = -s;
	d[4] = s;
}

void scale(float *d, float x, float y, float z) {
	strncpy(d, _identity, 64);
	d[0] = x;
	d[5] = y;
	d[10] = z;
}


/*Before complaining about this proc and the one below,
 *consider that I use a different reference frame
 *from the one used by OpenGL: visible area ranges from
 *(1, 1, 1) to (-1, -1, -1), and the default camera is
 *at (0, 0, -1)
 *Btw you are free to complain about my reference frame
 */

void camera(float *d, float px, float py, float pz,
		float cx, float cy, float cz,
		float ux, float uy, float uz) {
	float a[9];
	float *t = a, *u = a + 3, *l = a + 6;
	float p[3] = {px, py, pz};
	float ml;
	l[0] = cx - px;
	l[1] = cy - py;
	l[2] = cz - pz;
	ml = mod3(l);
	norm(l);
	u[0] = ux;
	u[1] = uy;
	u[2] = uz;
	{ // t is a nice name so changed scope
		//get the projection of u on a plane that is orthogonal with c-p
		float t = mulvecS(u, l);
		u[0] -= t * l[0];
		u[1] -= t * l[1];
		u[2] -= t * l[2];
	}
	norm(u);
	mulvecV(l, u, t);
	norm(t);
	if(mod3(t) == 0) {
		identity(d); //TODO better handling of error
		return;
	}
	float b[9];
	float e = 1.0f / det3(a);
	for(int i = 0; i < 3; i++) { //base rotation
		d[3 * 4 + i] = 0;
		float v[3] = {i == 0, i == 1, i == 2};
		for(int j = 0; j < 3; j++) {
			strncpy(b, a, 9 * 4);
			strncpy(b + 3 * j, v, 3 * 4);
			d[j * 4 + i] = det3(b) * e;
		}
		d[2 * 4 + i] *= -1.0f;
	}
	for(int i = 0; i < 3; i++) { //translation to center (applied before the rotation)
		float s = 0.0f;
		for(int j = 0; j < 3; j++)
			s -= d[i * 4 + j] * p[j];
		d[i * 4 + 3] = s;
	}
	d[11] += 1.0f; //translation after the rotation
	d[15] = 1.0f;
}

void perspective(float *d, float fov, float r, float n, float f) {
	if(r == 0.0f || n >= f || fov == M_PI || fov == 0.0f) {
		identity(d);
		return;
	}
	scale(d, 1.0f / (tan(fov / 2.0f)), r / tan(fov / 2.0f), 2.0f / (f - n));
	d[11] = (f + n - 2.0f) / (f - n);
	d[14] = -1.0f;
	d[15] = 1.0f;
}

/*void applyMatrix(float *d, const float *s, int n, const float *m) {
	while(n > 0) {
		mulmat(m, s, d, 4, 4, 1);
		//if(d[3] != 0.0f) {
			d[0] /= d[3];
			d[1] /= d[3];
		//}
		s += 4;
		d += 4;
		n--;
	}
}*/

void applyMatrixToVec3(float *d, const float *s, int n, const float *m) {
	float t[4];
	while(n > 0) {
		t[0] = s[0];
		t[1] = s[1];
		t[2] = s[2];
		t[3] = 1.0f;
		mulmat(m, t, d, 4, 4, 1);
		if(d[3] != 0.0f) { // TODO add a better clipping
			d[0] /= d[3];
			d[1] /= d[3];
			d[2] /= d[3] < 0.0f ? -d[3] : d[3];
		}
		else {
			d[0] /= 0.000001f;
			d[1] /= 0.000001f;
			d[2] /= 0.000001f;
		}
		s += 3;
		d += 4;
		n--;
	}
}
