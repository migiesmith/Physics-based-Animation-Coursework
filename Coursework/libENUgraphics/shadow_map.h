#pragma once

#include "stdafx.h"
#include "depth_buffer.h"

namespace graphics_framework
{
	// Data structure representing a shadow map
	struct shadow_map
	{
		// Matrix used as bias when calculating shadow matrix
		static glm::mat4 bias;
		// The position of the light used in the shadow map development
		glm::vec3 light_position = glm::vec3(0.0f, 0.0f, 0.0f);
		// The direction of the light used in the shadow map development
		glm::vec3 light_dir = glm::vec3(-1.0f, 0.0f, 0.0f);
		// The depth buffer storing the depth information
		std::shared_ptr<depth_buffer> buffer = nullptr;

        // Default constructor
        shadow_map()
        {
            // Buffer not initialised
        }

		// Creates a new shadow map
		shadow_map(GLuint width, GLuint height)
			: buffer(std::make_shared<depth_buffer>(width, height))
		{
		}
	};

}