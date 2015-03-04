#ifndef OPENGLDYNAMICBUFFER_H
#define OPENGLDYNAMICBUFFER_H OpenGLDynamicBuffer

#include <OpenGLBuffer>

template <typename T>
class OpenGLDynamicBuffer : public OpenGLBuffer
{
public:
  typedef T ElementType;
  typedef T* ElementPointer;
  typedef T& ElementReference;
  enum { ElementSize = sizeof(ElementType) };
  void reserve(size_t elements);
  ElementPointer mapRange(size_t offset, size_t count, RangeAccessFlags access);
};

template <typename T>
void OpenGLDynamicBuffer<T>::reserve(size_t elements)
{
  size_t sizeInBytes = sizeof(ElementType) * elements;
  if (!isCreated() || size() < sizeInBytes)
  {
    allocate(sizeInBytes);
  }
}

template <typename T>
auto OpenGLDynamicBuffer<T>::mapRange(size_t offset, size_t count, RangeAccessFlags access) -> ElementPointer
{
  return static_cast<ElementPointer>(OpenGLBuffer::mapRange(offset, ElementSize * count, access));
}

#endif // OPENGLDYNAMICBUFFER_H

