#pragma once

#include <glm/glm.hpp>

struct compareVec
{
	bool operator() (const glm::vec3& lhs, const glm::vec3& rhs) const
	{
		if (lhs.x < rhs.x)
			return true;
		else if (lhs.x > rhs.x)
			return false;
		else
		{
			if (lhs.y < rhs.y)
				return true;
			else if (lhs.y > rhs.y)
				return false;
			else
			{
				if (lhs.z < rhs.z)
					return true;
				else if (lhs.z > rhs.z)
					return false;
				else
				{
					return false;
				}
			}
		}
	}
};
