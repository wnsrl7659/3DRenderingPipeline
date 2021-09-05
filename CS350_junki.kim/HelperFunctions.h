/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: HelperFunctions.h
Purpose: It contains math functions
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/7/2021
End Header --------------------------------------------------------*/
#pragma once

#include <glm/glm.hpp>

const float DistanceSQ(const glm::vec3 A, const glm::vec3 B)
{
  glm::vec3 diff = B - A;
  return float((diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z));
}

const glm::vec3 MidPosition(const glm::vec3 A, const glm::vec3 B)
{
  return glm::vec3((A + B) / 2.f);
}

void Mat3Identity(glm::mat3& m)
{
  m[0][0] = 1.f;
  m[0][1] = 0.f;
  m[0][2] = 0.f;

  m[1][0] = 0.f;
  m[1][1] = 1.f;
  m[1][2] = 0.f;

  m[2][0] = 0.f;
  m[2][1] = 0.f;
  m[2][2] = 1.f;
}

void Mat4Identity(glm::mat4& m)
{
  m[0][0] = 1.f;
  m[0][1] = 0.f;
  m[0][2] = 0.f;
  m[0][3] = 0.f;

  m[1][0] = 0.f;
  m[1][1] = 1.f;
  m[1][2] = 0.f;
  m[1][3] = 0.f;

  m[2][0] = 0.f;
  m[2][1] = 0.f;
  m[2][2] = 1.f;
  m[2][3] = 0.f;

  m[3][0] = 0.f;
  m[3][1] = 0.f;
  m[3][2] = 0.f;
  m[3][3] = 1.f;
}

void SymmetricSchur(glm::mat3& a, int p, int q, float& c, float& s)
{
  if (glm::abs(a[p][q]) > 0.0001f)
  {
    float r = (a[q][q] - a[p][p]) / (2.0f * a[p][q]);
    float t;
    if (r >= 0.0f)
      t = 1.0f / (r + glm::sqrt(1.0f + r * r));
    else
      t = -1.0f / (-r + glm::sqrt(1.0f + r * r));
    c = 1.0f / glm::sqrt(1.0f + t * t);
    s = t * c;
  }
  else
  {
    c = 1.0f;
    s = 0.0f;
  }
}

glm::mat3 Covariance(std::vector<Vertex>& vertices)
{
  const float div = 1.0f / vertices.size();

  // center position
  glm::vec3 centerPos(0.f);
  for (auto& v : vertices)
    centerPos += v.Position;

  centerPos *= div;

  // calcuate covariance
  glm::mat3 m(0.f);
  for (auto& v : vertices)
  {
    const glm::vec3& pos = v.Position - centerPos;

    m[0][0] += pos.x * pos.x;
    m[1][1] += pos.y * pos.y;
    m[2][2] += pos.z * pos.z;
    m[0][1] += pos.x * pos.y;
    m[0][2] += pos.x * pos.z;
    m[1][2] += pos.y * pos.z;
  }

  // result
  m[0][0] *= div;
  m[1][1] *= div;
  m[2][2] *= div;
  m[1][0] = (m[0][1] *= div);
  m[2][0] = (m[0][2] *= div);
  m[2][1] = (m[1][2] *= div);

  return m;
}

void JacobianMatrix(glm::mat3& _mat, glm::mat3& _vecs)
{
  // identity matrix
  Mat3Identity(_vecs);
  float prevOff = 0.f;

  // Jacobi Iter = 50
  for (unsigned n = 0; n < 50; ++n)
  {
    int p = 0;
    int q = 1;

    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        if (i == j)
          continue;

        if (std::abs(_mat[i][j]) > abs(_mat[p][q]))
        {
          p = i;
          q = j;
        }
      }
    }

    float c, s;
    glm::mat3 J;
    Mat3Identity(J);

    // rotation matrix
    SymmetricSchur(_mat, p, q, c, s);

    J[p][p] = c;
    J[p][q] = s;
    J[q][p] = -s;
    J[q][q] = c;

    _vecs = _vecs * J;
    _mat = (glm::transpose(J) * _mat) * J;

    float off = 0.0f;

    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        if (i == j)
          continue;

        off += _mat[i][j] * _mat[i][j];
      }
    }

    // decreasing, exit!
    if (n > 2 && off >= prevOff)
      return;

    prevOff = off;
  }
}

void FarestPointsAlongDir(const glm::vec3& _direction, const std::vector<Vertex>& _vertices, glm::vec3& _minPos, glm::vec3& _maxPos)
{
  float minProj = FLT_MAX;
  float maxProj = -FLT_MAX;

  int minIndex = 0;
  int maxIndex = 0;

  for (unsigned i = 0; i < _vertices.size(); ++i)
  {
    const glm::vec3& vertex = _vertices[i].Position;

    // onto the direction
    float proj = glm::dot(vertex, _direction);

    // max
    if (proj > maxProj)
    {
      maxProj = proj;
      maxIndex = i;
    }

    // min
    if (proj < minProj)
    {
      minProj = proj;
      minIndex = i;
    }
  }

  // set
  _maxPos = _vertices[maxIndex].Position;
  _minPos = _vertices[minIndex].Position;
}

// Eigenvectors are a special set of vectors associated with a linear system of
// equations (i.e., a matrix equation) that are sometimes also known as
// characteristic vectors, proper vectors, or latent vectors
void Eigenvectors(const glm::mat3& covariance, glm::vec3& vx, glm::vec3& vy, glm::vec3& vz)
{
  glm::mat3 eigenValues(covariance);
  glm::mat3 eigenVecs;

  JacobianMatrix(eigenValues, eigenVecs);

  float values[3] = { eigenValues[0][0], eigenValues[1][1], eigenValues[2][2] };
  glm::vec3 vecs[3] = {
    glm::vec3(eigenVecs[0][0], eigenVecs[1][0], eigenVecs[2][0]),
    glm::vec3(eigenVecs[0][1], eigenVecs[1][1], eigenVecs[2][1]),
    glm::vec3(eigenVecs[0][2], eigenVecs[1][2], eigenVecs[2][2])
  };

  if (values[0] < values[1])
  {
    std::swap(values[0], values[1]);
    std::swap(vecs[0], vecs[1]);
  }

  if (values[0] < values[2])
  {
    std::swap(values[0], values[2]);
    std::swap(vecs[0], vecs[2]);
  }

  if (values[1] < values[2])
  {
    std::swap(values[1], values[2]);
    std::swap(vecs[1], vecs[2]);
  }

  vx = vecs[0];
  vy = vecs[1];
  vz = vecs[2];
}

glm::vec3 EigenvectorCorrectness(const glm::vec3 va, const glm::vec3 vb, const glm::vec3 vc, const int index)
{
  if (va[index] <= vb[index])
  {
    if (vb[index] <= vc[index])
      return vc;
    else
      return vb;
  }
  else
    return va;
}

void CalcuatingPCA3Axises(std::vector<Vertex>& _vertices, glm::vec3& _centerPos, glm::vec3& _widths, glm::mat3& rotationMat)
{
  glm::mat3 m = Covariance(_vertices);

  // find eigen vectors
  glm::vec3 vx;
  glm::vec3 vy;
  glm::vec3 vz;

  Eigenvectors(m, vx, vy, vz);

  // normalize axes
  vx = glm::normalize(vx);
  vy = glm::normalize(vy);
  vz = glm::normalize(vz);

  rotationMat = glm::mat3(
    vx.x, vx.y, vx.z,
    vy.x, vy.y, vy.z,
    vz.x, vz.y, vz.z
  );

  // find extents
  glm::vec3 minX, maxX;
  FarestPointsAlongDir(vx, _vertices, minX, maxX);

  glm::vec3 minY, maxY;
  FarestPointsAlongDir(vy, _vertices, minY, maxY);

  glm::vec3 minZ, maxZ;
  FarestPointsAlongDir(vz, _vertices, minZ, maxZ);

  glm::mat3 rotInv = glm::inverse(rotationMat);
  minX = rotInv * minX;
  maxX = rotInv * maxX;
  minY = rotInv * minY;
  maxY = rotInv * maxY;
  minZ = rotInv * minZ;
  maxZ = rotInv * maxZ;

  _widths.x = (maxX.x - minX.x) / 2.0f;
  _widths.y = (maxY.y - minY.y) / 2.0f;
  _widths.z = (maxZ.z - minZ.z) / 2.0f;

  // calculate center
  _centerPos = glm::vec3(
    (maxX.x + minX.x) / 2.0f,
    (maxY.y + minY.y) / 2.0f,
    (maxZ.z + minZ.z) / 2.0f);
  _centerPos = rotationMat * _centerPos;
}