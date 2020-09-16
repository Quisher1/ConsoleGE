#pragma once

class Matrix {
public:
	int rows = 0; // строки - h
	int columns = 0; // столбцы - w
	float* Data;

	Matrix(){}

	Matrix(int rows, int columns) {
		this->rows = rows;
		this->columns = columns;

		Data = new float[rows*columns];

		for (int i = 0; i < rows*columns; ++i)
			Data[i] = 0;
	}

	Matrix(int rows, int columns, float* data) {
		this->rows = rows;
		this->columns = columns;

		Data = new float[rows*columns];

		for (int i = 0; i < rows*columns; ++i)
			Data[i] = data[i];
	}


	void SetUnionMatrix() {
		for (int i = 0; i < rows*columns; ++i)
			Data[i] = 0;
		for (int i = 0; i < rows && i < columns; ++i)
			Data[i * columns + i] = 1;
	}

	void Print() {
		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < columns; ++j)
			{
				printf("%f ", Data[i*columns + j]);
			}printf("\n");
		}
	}


	void ReleaseData() {
		delete[] Data;
	}
};


void Mult(int Ah, int Bw, int Bh, const float* A, const float* B, float* C) { // Не ест безразмерно память

	for (int i = 0; i < Ah; ++i)
	{
		for (int j = 0; j < Bw; ++j)
		{
			C[i*Bw + j] = 0;
			for (int k = 0; k < Bh; ++k)
				C[i*Bw + j] += A[i*Bh + k] * B[k*Bw + j];
		}
	}
}


namespace MatrixPrefs {

	float* Transpose(int n, float* Data) {
		Matrix m(4, 4);
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				m.Data[i*n + j] = Data[j*n + i];
			}
		}
		return m.Data;
	}

	float* RotateX(float angle, int w = 4, int h = 4) {
		Matrix m(4, 4);

		m.Data[0 * 4 + 0] = 1;
		m.Data[1 * 4 + 1] =  cos(angle * 3.1415926f / 180.0f);
		m.Data[1 * 4 + 2] = -sin(angle * 3.1415926f / 180.0f);
		m.Data[2 * 4 + 1] =  sin(angle * 3.1415926f / 180.0f);
		m.Data[2 * 4 + 2] =  cos(angle * 3.1415926f / 180.0f);
		m.Data[3 * 4 + 3] = 1;

		return m.Data;
	}

	float* MatrixPositing(float x, float y, float z) {
		Matrix m(4, 4);
		m.SetUnionMatrix();

		m.Data[12] = x;
		m.Data[13] = y;
		m.Data[14] = z;

		return m.Data;
	}

}