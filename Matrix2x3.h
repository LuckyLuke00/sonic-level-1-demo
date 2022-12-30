#pragma once
#include <vector>
#include "Vector2f.h"

struct Matrix2x3 final
{
	// -------------------------
	// Constructors
	// -------------------------
	// Default constructor results in a unity matrix
	explicit Matrix2x3(Vector2f dirX = Vector2f{ 1, 0 }, Vector2f dirY = Vector2f{ 0, 1 }, Vector2f origTrans = Vector2f{ 0, 0 });
	// Constructor, using floats, all required
	explicit Matrix2x3(float e1X, float e1Y, float e2X, float e2Y, float oX, float oY);

	// -------------------------
	// General Methods
	// -------------------------
	// Transform a vector by this matrix, no translation
	// Vector2f vTransformed = mat.Transform(v);
	Vector2f Transform(const Vector2f& v) const;

	// Transform a point by this matrix, including translation
	// Point2f pTransformed = mat.Transform(p);
	Point2f Transform(const Point2f& p) const;

	// Transform a Rectf by this matrix, including translation
	// std::vector<Point2f> transformedVertices = mat.Transform(r);
	std::vector<Point2f> Transform(const Rectf& r) const;

	// Transform a Rectf by this matrix, including translation
	// Pass pointer to resulting array (size should be at least 4)
	//   Point2f transformedVertices[4];
	//   Transform(r, transformedVertices);
	void Transform(const Rectf& r, Point2f* transVertices) const;

	// Transform a Polygon by this matrix, including translation
	// std::vector<Point2f> transformedVertices = mat.Transform(vertices);
	std::vector<Point2f> Transform(const std::vector<Point2f>& vertices) const;

	// Transform a Polygon by this matrix, including translation
	// Pass pointer to resulting array (size should be at least size of polygon)
	//   Point2f transformedVertices[vertices.size()];
	//   Transform(vertices, transformedVertices);
	void Transform(const std::vector<Point2f>& vertices, Point2f* transVertices) const;

	// Transform a Polygon by this matrix, including translation
	//   Point2f transformedVertices[nrVertices];
	//   Transform(vertices, nrVertices, transformedVertices);
	void Transform(const Point2f* vertices, Point2f* transVertices, size_t nrVertices) const;

	// Calculate the determinant
	float Determinant() const;

	// Calculate the inverse matrix
	Matrix2x3 Inverse() const;

	// Are two matrices equal within a threshold?
	// mat1.Equals(mat2)
	bool Equals(const Matrix2x3& other, float epsilon = 0.001f) const;

	// Creates a string containing a text representation of the values of the matrix
	std::string ToString() const;

	// Converts this matrix into a Identity matrix
	void SetAsIdentity();
	// Converts this matrix into a Rotate matrix
	void SetAsRotate(float degrees);
	// Converts this matrix into a Translation matrix
	void SetAsTranslate(float tx, float ty);
	// Converts this matrix into a Translation matrix
	void SetAsTranslate(Vector2f pt);
	// Converts this matrix into a Scale matrix
	void SetAsScale(float sx, float sy);
	// Converts this matrix into a Scale matrix
	void SetAsScale(float s);

	// -------------------------------------------
	// Static Matrix2x3 object creation methods
	// -------------------------------------------
	// Instantiate a rotation matrix:
	// Matrix matRot = Matrix::Rotation(45.0f);
	static Matrix2x3 CreateRotationMatrix(float degrees);
	// Instantiate an identity matrix:
	// Matrix2x3 matId = Matrix2x3::Identity();
	static Matrix2x3 CreateIdentityMatrix();
	// Instantiate a scale matrix:
	// Matrix matScale = Matrix::Scaling(2.0f);
	static Matrix2x3 CreateScalingMatrix(float scale);
	// Instantiate a scale matrix:
	// Matrix matScale = Matrix::Scaling(2.0f,-3.0f);
	static Matrix2x3 CreateScalingMatrix(float scaleX, float scaleY);
	// Instantiate a scale matrix:
	// Matrix matScale = Matrix::Scaling( Vector2f(2.0f,-3.0f) );
	static Matrix2x3 CreateScalingMatrix(Vector2f scaleVector);
	// Instantiate a translation matrix:
	// Matrix matTrans = Matrix::Translation( Vector2f(2.0f,3.0f) );
	static Matrix2x3 CreateTranslationMatrix(Vector2f origin);
	// Instantiate a translation matrix:
	// Matrix matTrans = Matrix::Translation(2.0f, 3.0f);
	static Matrix2x3 CreateTranslationMatrix(float tx, float ty);

	// -------------------------
	// Datamembers
	// -------------------------
	Vector2f dirX;	// The first matrix vector (the "x-axis"), 1st column
	Vector2f dirY;	// The second matrix vector (the "y-axis"), second column
	Vector2f orig; 	// The origin of  the coordinate matrix (the "translation"), third column
};

// -------------------------
// Operators
// -------------------------
// Are two matrices exactly equal?
// mat1 == mat2
bool operator==(const Matrix2x3& lhs, const Matrix2x3& rhs);
// Are two matrices exactly unequal?
// mat1 != mat2
bool operator!=(const Matrix2x3& lhs, const Matrix2x3& rhs);
// Multiply matrices
// Matrix2x3 matProduct {mat1 * mat2};
Matrix2x3 operator*(const Matrix2x3& lhs, const Matrix2x3& rhs);
// Send matrix to output stream
// std::cout << mat;
std::ostream& operator<<(std::ostream& os, const Matrix2x3& matrix);
