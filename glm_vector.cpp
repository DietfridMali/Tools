#if defined(COMPILE_VECTOR)

#include "glm_vector.hpp"

const Vector3f Vector3f::ZERO = Vector3f{ 0.0f, 0.0f, 0.0f };

const Vector3f Vector3f::NONE = Vector3f{ NAN, NAN, NAN };

const Vector3f Vector3f::ONE = Vector3f{ 1.0f, 1.0f, 1.0f };

const Vector4f Vector4f::ZERO = Vector4f{ 0.0f, 0.0f, 0.0f, 0.0f };

const Vector4f Vector4f::ONE = Vector4f{ 1.0f, 1.0f, 1.0f, 1.0f };

const Vector4f Vector4f::NONE = Vector4f{ NAN, NAN, NAN, NAN };

#endif