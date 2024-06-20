#ifndef MODELS_H
#define MODELS_H

#include <glm/glm.hpp>

namespace models
{
	namespace cube
	{
		inline constexpr size_t NUM_VERTS = 24;
		inline constexpr size_t NUM_INDICES = 36;

		inline constexpr glm::vec3 positions[NUM_VERTS] =
		{
			// face +x
			{0.5f, 0.5f, 0.5f}, // I quadrant
			{0.5f, -0.5f, 0.5f}, // II quadrant
			{0.5f, -0.5f, -0.5f}, // III quadrant
			{0.5f, 0.5f, -0.5f}, // IV quadrant

			// face -x
			{-0.5f, 0.5f, 0.5f},
			{-0.5f, -0.5f, 0.5f},
			{-0.5f, -0.5f, -0.5f},
			{-0.5f, 0.5f, -0.5f},

			// face +y
			{0.5f, 0.5f, 0.5f},
			{-0.5f, 0.5f, 0.5f},
			{-0.5f, 0.5f, -0.5f},
			{0.5f, 0.5f, -0.5f},

			// face -y
			{0.5f, -0.5f, 0.5f},
			{-0.5f, -0.5f, 0.5f},
			{-0.5f, -0.5f, -0.5f},
			{0.5f, -0.5f, -0.5f},

			// face +z
			{0.5f, 0.5f, 0.5f},
			{-0.5f, 0.5f, 0.5f},
			{-0.5f, -0.5f, 0.5f},
			{0.5f, -0.5f, 0.5f},

			// face -z
			{0.5f, 0.5f, -0.5f},
			{-0.5f, 0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f},
			{0.5f, -0.5f, -0.5f}
		};

		inline constexpr glm::vec3 normals[NUM_VERTS] =
		{
			{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
			{-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
			{0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}
		};

		inline constexpr unsigned int indices[NUM_INDICES] =
		{
			// upper-left triangle                 // lower-right triangle
			0 + (4 * 0), 1 + (4 * 0), 2 + (4 * 0), 2 + (4 * 0), 3 + (4 * 0), 0 + (4 * 0),
			0 + (4 * 1), 2 + (4 * 1), 1 + (4 * 1), 2 + (4 * 1), 0 + (4 * 1), 3 + (4 * 1),
			0 + (4 * 2), 2 + (4 * 2), 1 + (4 * 2), 2 + (4 * 2), 0 + (4 * 2), 3 + (4 * 2),
			0 + (4 * 3), 1 + (4 * 3), 2 + (4 * 3), 2 + (4 * 3), 3 + (4 * 3), 0 + (4 * 3),
			0 + (4 * 4), 1 + (4 * 4), 2 + (4 * 4), 2 + (4 * 4), 3 + (4 * 4), 0 + (4 * 4),
			0 + (4 * 5), 2 + (4 * 5), 1 + (4 * 5), 2 + (4 * 5), 0 + (4 * 5), 3 + (4 * 5)
		};
	}

	namespace xysquare
	{
		inline constexpr size_t NUM_VERTS = 4;
		inline constexpr size_t NUM_INDICES = 6;

		inline constexpr glm::vec3 positions[NUM_VERTS] =
		{
			{0.5f, 0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f}, {-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}
		};

		inline constexpr glm::vec3 normals[NUM_VERTS] =
		{
			{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}
		};

		inline constexpr unsigned int indices[NUM_INDICES] =
		{
			0, 1, 2, 2, 3, 0
		};
	}
}

#endif