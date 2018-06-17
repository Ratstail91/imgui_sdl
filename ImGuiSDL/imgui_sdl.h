#pragma once

#include "SDL.h"

#include "SDL_image.h"
#include "SDL2_gfxPrimitives.h"
#include "imgui.h"

#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

namespace ImGuiSDL
{
	// TODO: Separate declarations for cleaner documentation.
	// TODO: Improved README with usage, include stuff etc.
	// TODO: Reference at least IMGUI
	// TODO: Clean the public API.

	struct Color
	{
		const double R, G, B, A;

		explicit Color(uint32_t color)
			: R(((color >> 0) & 0xff) / 255.0f), G(((color >> 8) & 0xff) / 255.0f), B(((color >> 16) & 0xff) / 255.0f), A(((color >> 24) & 0xff) / 255.0f) { }

		Color(float r, float g, float b, float a) : R(r), G(g), B(b), A(a) { }

		Color operator*(const Color& c) const { return Color(R * c.R, G * c.G, B * c.B, A * c.A); }

		uint32_t ToInt() const
		{
			return	((static_cast<int>(R * 255) & 0xff) << 0) 
				  | ((static_cast<int>(G * 255) & 0xff) << 8) 
				  | ((static_cast<int>(B * 255) & 0xff) << 16)
				  | ((static_cast<int>(A * 255) & 0xff) << 24);
		}
	};

	struct Texture
	{
		SDL_Surface* Surface;
		SDL_Texture* Source;

		Color Sample(float u, float v) const
		{
			const int x = static_cast<int>(roundf(u * (Surface->w - 1) + 0.5f));
			const int y = static_cast<int>(roundf(v * (Surface->h - 1) + 0.5f));

			const int location = y * Surface->w + x;
			assert(location < Surface->w * Surface->h);

			return Color(static_cast<uint32_t*>(Surface->pixels)[location]);
		}
	};

	struct ClipRect
	{
		int X, Y, Width, Height;
	};

	struct Target
	{
		std::vector<uint32_t> Pixels;
		int Width, Height;

		ClipRect Clip;

		SDL_Renderer* Renderer;

		// TODO: Implement an LRU cache and make its size configurable.
		// Caches by UV coordinates and color. The format is Vertex1: X, Y, U, V, Color, Vertex2: X, Y, U, V, Color... Width, Height
		using TextureCacheKey = std::tuple<double, double, double, double, uint32_t, double, double, double, double, uint32_t, double, double, double, double, uint32_t, int, int>;
		std::map<TextureCacheKey, SDL_Texture*> CacheTextures;

		Target(int width, int height, SDL_Renderer* renderer);

		void Resize(int width, int height);

		void EnableClip();
		void DisableClip();
		void SetClipRect(const ClipRect& rect);

		void SetAt(int x, int y, const Color& color);
		SDL_Texture* MakeTexture(int width, int height);
		void UseAsRenderTarget(SDL_Texture* texture);
	};

	template <typename T> T Min3(T a, T b, T c) { return std::min(a, std::min(b, c)); }
	template <typename T> T Max3(T a, T b, T c) { return std::max(a, std::max(b, c)); }

	void DoImGuiRender(Target& target, ImDrawData* drawData);
};