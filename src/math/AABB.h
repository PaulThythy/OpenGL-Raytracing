#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>
#include <algorithm>

struct AABB {
	alignas(16) glm::vec3 m_Min;
	alignas(16) glm::vec3 m_Max;

	inline AABB() : m_Min(glm::vec3(INFINITY)), m_Max(glm::vec3(-INFINITY)) {}

	inline AABB(const glm::vec3& min, const glm::vec3& max) : m_Min(min), m_Max(max) {}

	inline void expandToInclude(const glm::vec3& point) {
		m_Min = glm::min(m_Min, point);
		m_Max = glm::max(m_Max, point);
	}

	inline void expandToInclude(const AABB& other) {
		m_Min = glm::min(m_Min, other.m_Min);
		m_Max = glm::max(m_Max, other.m_Max);
	}

	inline glm::vec3 getCenter() const {
		return (m_Min + m_Max) * 0.5f;
	}

	inline float getSurface() const {
		glm::vec3 extent = m_Max - m_Min;
		return 2.0f * (extent.x * extent.y + extent.y * extent.z + extent.z * extent.x);
	}
};

#endif