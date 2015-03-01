#ifndef KMATH_H
#define KMATH_H KMath

#include <KVector3D>
#include <KMatrix3x3>
#include <limits>
#include <KColor>

namespace Karma
{

  template <typename T>
  struct MinMax
  {
    T max;
    T min;
  };
  typedef MinMax<KVector3D> MinMaxKVector3D;
  typedef std::vector<MinMaxKVector3D> MinMaxKVector3DContainer;

  template <typename Val>
  struct DefaultAccessor : public std::unary_function<Val, Val>
  {
    Val &operator()(Val &v) const
    {
      return v;
    }
  };

  template <typename Val>
  struct DefaultMutator
  {
    Val operator()(Val v, float dotProd, KVector3D const &axis) const
    {
      (void)dotProd;
      (void)axis;
      return v;
    }
  };

  template <typename Val>
  struct AxisMutator
  {
    Val operator()(Val v, float dotProd, KVector3D const &axis) const
    {
      (void)v;
      return dotProd * axis;
    }
  };

  // Constants
  extern const float Pi;
  extern const float PiHalf;
  extern const float TwoPi;
  extern const float Log2;

  // Matrix Decomposition
  KVector3D minEigenExtents(KMatrix3x3 const &eigenVecs);
  KVector3D maxEigenExtents(KMatrix3x3 const &eigenVecs);
  void decomposeMatrixeByColumnVectors(KMatrix3x3 const &eigenVecs, KVector3D axes[3]);
  std::vector<KVector3D> decomposeMatrixeByColumnVectors(KMatrix3x3 const &mtx);
  void reconstructMatrixByColumnVectors(KMatrix3x3 *mtx, KVector3D const &a, KVector3D const &b, KVector3D const &c);

  // Covariance Matrix Calculations
  template <typename It, typename Accessor = DefaultAccessor<KVector3D>>
  KMatrix3x3 covarianceMatrix(It begin, It end, Accessor accessor = DefaultAccessor<KVector3D>());
  void symSchur2(KMatrix3x3 const &symMtx, int p, int q, float *cosine, float *sine);
  KMatrix3x3 jacobi(KMatrix3x3 covar, int iterations);

  // Covariance Axes information
  template <typename It, typename Accessor = DefaultAccessor<KVector3D>>
  MinMaxKVector3D findExtremalPointsAlongAxis(It begin, It end, KVector3D axis, Accessor accessor = DefaultAccessor<KVector3D>());
  template <typename It, typename Accessor = DefaultAccessor<KVector3D>>
  MinMaxKVector3D findExtremalProjectedPointsAlongAxis(It begin, It end, KVector3D axis, Accessor accessor = DefaultAccessor<KVector3D>());
  template <typename It, typename Accessor = DefaultAccessor<KVector3D>, typename Mutator = DefaultMutator<KVector3D>>
  MinMaxKVector3D findExtremalAlongAxis(It begin, It end, KVector3D axis, Accessor accessor = DefaultAccessor<KVector3D>(), Mutator mutator = DefaultMutator<KVector3D>());
  template <typename It1, typename It2, typename VecAccessor = DefaultAccessor<KVector3D>, typename AxisAccessor = DefaultAccessor<KVector3D>>
  MinMaxKVector3DContainer findExtremalPointsAlongAxes(It1 bVec, It1 eVec, It2 bAxis, It2 eAxis, VecAccessor vAccessor = DefaultAccessor<KVector3D>(), AxisAccessor aAccessor = DefaultAccessor<KVector3D>());
  template <typename It1, typename It2, typename VecAccessor = DefaultAccessor<KVector3D>, typename AxisAccessor = DefaultAccessor<KVector3D>>
  MinMaxKVector3DContainer findExtremalProjectedPointsAlongAxes(It1 bVec, It1 eVec, It2 bAxis, It2 eAxis, VecAccessor vAccessor = DefaultAccessor<KVector3D>(), AxisAccessor aAccessor = DefaultAccessor<KVector3D>());
  template <typename It1, typename It2, typename VecAccessor = DefaultAccessor<KVector3D>, typename AxisAccessor, typename Mutator = DefaultMutator<KVector3D>>
  MinMaxKVector3DContainer findExtremalAlongAxes(It1 bVec, It1 eVec, It2 bAxis, It2 eAxis, VecAccessor vAccessor = DefaultAccessor<KVector3D>(), AxisAccessor aAccessor = DefaultAccessor<KVector3D>(), Mutator mutator = DefaultMutator<KVector3D>());
  template <typename It, typename Accessor>
  KVector3D findAverageCentroid(It begin, It end, Accessor accessor = DefaultAccessor<KVector3D>());
  template <typename It, typename Accessor = DefaultAccessor<KVector3D>>
  MinMaxKVector3D findMinMaxBounds(It begin, It end, Accessor accessor = DefaultAccessor<KVector3D>());

  // Color Manipulaton
  KColor colorShift(KColor const &orig, float amt);

  template <typename It, typename Func>
  void maxSeperatedAlongAxis(It begin, It end, Func f, KVector3D axis, KVector3D *min, KVector3D *max);
  template <typename It, typename Func>
  void lengthsAlongAxes(It begin, It end, Func f, KVector3D axes[3], float dist[3], KVector3D *centroid);
  template <size_t N, typename It, typename Func>
  KVector3D calculateCentroid(It begin, It end, Func f, KVector3D axes[N], float extents[N]);
  template <typename It1, typename It2, typename Func1>
  std::vector<KVector3D> findExtremalPoints(It1 beginPoints, It1 endPoints, It2 beginNorms, It2 endNorms, Func1 pointAccessor);
}

template <typename It, typename Accessor>
Karma::MinMaxKVector3D Karma::findExtremalPointsAlongAxis(It begin, It end, KVector3D axis, Accessor accessor)
{
  return findExtremalAlongAxis(begin, end, axis, accessor, DefaultMutator<KVector3D>());
}

template <typename It, typename Accessor>
Karma::MinMaxKVector3D Karma::findExtremalProjectedPointsAlongAxis(It begin, It end, KVector3D axis, Accessor accessor)
{
  return findExtremalAlongAxis(begin, end, axis, accessor, AxisMutator<KVector3D>());
}

template <typename It, typename Accessor, typename Mutator>
Karma::MinMaxKVector3D Karma::findExtremalAlongAxis(It begin, It end, KVector3D axis, Accessor accessor, Mutator mutator)
{
  typedef std::numeric_limits<float> FloatLimits;

  float signedDist;
  float minProjDist =  FloatLimits::infinity();
  float maxProjDist = -FloatLimits::infinity();

  KVector3D vector;
  MinMaxKVector3D maxMin;
  while (begin != end)
  {
    vector = accessor(*begin);
    signedDist = KVector3D::dotProduct(vector, axis);
    if (signedDist > maxProjDist)
    {
      maxProjDist = signedDist;
      maxMin.max = mutator(vector, signedDist, axis);
    }
    if (signedDist < minProjDist)
    {
      minProjDist = signedDist;
      maxMin.min = mutator(vector, signedDist, axis);
    }
    ++begin;
  }

  return maxMin;
}

template <typename It1, typename It2, typename VecAccessor, typename AxisAccessor>
Karma::MinMaxKVector3DContainer Karma::findExtremalPointsAlongAxes(It1 bVec, It1 eVec, It2 bAxis, It2 eAxis, VecAccessor vAccessor, AxisAccessor aAccessor)
{
  return std::move(findExtremalAlongAxes(bVec, eVec, bAxis, eAxis, vAccessor, aAccessor, DefaultMutator<KVector3D>()));
}

template <typename It1, typename It2, typename VecAccessor, typename AxisAccessor>
Karma::MinMaxKVector3DContainer Karma::findExtremalProjectedPointsAlongAxes(It1 bVec, It1 eVec, It2 bAxis, It2 eAxis, VecAccessor vAccessor, AxisAccessor aAccessor)
{
  return std::move(findExtremalAlongAxes(bVec, eVec, bAxis, eAxis, vAccessor, aAccessor, AxisMutator<KVector3D>()));
}

template <typename It1, typename It2, typename VecAccessor, typename AxisAccessor, typename Mutator>
Karma::MinMaxKVector3DContainer Karma::findExtremalAlongAxes(It1 bVec, It1 eVec, It2 bAxis, It2 eAxis, VecAccessor vAccessor, AxisAccessor aAccessor, Mutator mutator)
{
  MinMaxKVector3DContainer results;
  while (bAxis != eAxis)
  {
    results.push_back(findExtremalAlongAxis(bVec, eVec, aAccessor(*bAxis), vAccessor, mutator));
    ++bAxis;
  }
  return std::move(results);
}

template <typename It, typename Accessor>
KVector3D Karma::findAverageCentroid(It begin, It end, Accessor accessor)
{
  size_t count = std::distance(begin, end);
  KVector3D centroid;
  while (begin != end)
  {
    centroid += accessor(*begin);
    ++begin;
  }
  return centroid / float(count);
}

template <typename It, typename Accessor>
Karma::MinMaxKVector3D Karma::findMinMaxBounds(It begin, It end, Accessor accessor)
{
  int test = 0;
  KVector3D vector;
  MinMaxKVector3D m;
  m.min = KVector3D( std::numeric_limits<float>::infinity(),  std::numeric_limits<float>::infinity(),  std::numeric_limits<float>::infinity());
  m.max = KVector3D(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
  while (begin != end)
  {
    ++test;
    vector = accessor(*begin);
    if (m.min.x() > vector.x()) m.min.setX(vector.x());
    if (m.min.y() > vector.y()) m.min.setY(vector.y());
    if (m.min.z() > vector.z()) m.min.setZ(vector.z());
    if (m.max.x() < vector.x()) m.max.setX(vector.x());
    if (m.max.y() < vector.y()) m.max.setY(vector.y());
    if (m.max.z() < vector.z()) m.max.setZ(vector.z());
    ++begin;
  }
  return m;
}

template <typename It, typename Accessor>
KMatrix3x3 Karma::covarianceMatrix(It begin, It end, Accessor accessor)
{
  It origBegin = begin;
  int count = std::distance(begin, end);
  float k = 1.0f / float(count);
  KVector3D center;
  float e00, e11, e22, e01, e02, e12;

  // Calculate average center
  while (begin != end)
  {
    center += accessor(*begin);
    ++begin;
  }
  center /= float(count);

  // Calculate covariance matrix
  begin = origBegin;
  KVector3D vCentered;
  e00 = e11 = e22 = e01 = e02 = e12 = 0.0f;
  while (begin != end)
  {
    vCentered = accessor(*begin) - center;
    e00 += vCentered.x() * vCentered.x();
    e11 += vCentered.y() * vCentered.y();
    e22 += vCentered.z() * vCentered.z();
    e01 += vCentered.x() * vCentered.y();
    e02 += vCentered.x() * vCentered.z();
    e12 += vCentered.y() * vCentered.z();
    ++begin;
  }

  // Apply scale to whole matrix
  KMatrix3x3 covariance;
  covariance[0][0] = k * e00;
  covariance[1][1] = k * e11;
  covariance[2][2] = k * e22;
  covariance[0][1] = covariance[1][0] = k * e01;
  covariance[0][2] = covariance[2][0] = k * e02;
  covariance[1][2] = covariance[2][1] = k * e12;

  return covariance;
}

template <typename It, typename Func>
void Karma::maxSeperatedAlongAxis(It begin, It end, Func f, KVector3D axis, KVector3D *min, KVector3D *max)
{
  float sDist;
  float maxProjDist = -std::numeric_limits<float>::infinity();
  float minProjDist =  std::numeric_limits<float>::infinity();
  while (begin != end)
  {
    sDist = KVector3D::dotProduct(f(*begin), axis);
    if (sDist > maxProjDist)
    {
      maxProjDist = sDist;
      (*max) = f(*begin);
    }
    if (sDist < minProjDist)
    {
      minProjDist = sDist;
      (*min) = f(*begin);
    }
    ++begin;
  }
}

template <size_t N, typename It, typename Func>
KVector3D Karma::calculateCentroid(It begin, It end, Func f, KVector3D axes[N], float extents[N])
{
  int i;
  float sDist;
  KVector3D maximum[N];
  KVector3D minimum[N];
  float maxProjDist[N];
  float minProjDist[N];

  // Set base cases for iterative computation
  for (size_t i = 0; i < N; ++i)
  {
    maxProjDist[i] = -std::numeric_limits<float>::infinity();
    minProjDist[i] =  std::numeric_limits<float>::infinity();
  }

  // Project all points onto each axis, store min/max projections.
  while (begin != end)
  {
    for (i = 0; i < N; ++i)
    {
      sDist = KVector3D::dotProduct(f(*begin), axes[i]);
      if (sDist > maxProjDist[i])
      {
        maxProjDist[i] = sDist;
        maximum[i] = sDist * axes[i];
      }
      if (sDist < minProjDist[i])
      {
        minProjDist[i] = sDist;
        minimum[i] = sDist * axes[i];
      }
    }
    ++begin;
  }

  // Calculate the centroid via the min/max of axes
  KVector3D centroid(0.0f, 0.0f, 0.0f);
  for (i = 0; i < 3; ++i)
  {
    extents[i] = (maximum[i] - minimum[i]).length();
    centroid += (maximum[i] + minimum[i]) / 2.0f;
  }

  return centroid;
}

template <typename It1, typename It2, typename Func1, typename Func2>
std::vector<KVector3D> findExtremalPoints(It1 origBeginPoints, It1 endPoints, It2 beginNorms, It2 endNorms, Func1 pointAccessor)
{
  It1 beginPoints;
  KVector3D minimum, maximum;
  std::vector<KVector3D> points;
  while (beginNorms != endNorms)
  {
    beginPoints = origBeginPoints;
    Karma::maxSeperatedAlongAxis(beginPoints, endPoints, pointsAccessor, normAccessor(*beginNorms), &minimum, &maximum);
    points.push_back(minimum);
    points.push_back(maximum);
    ++beginNorms;
  }
}

#endif // KMATH_H