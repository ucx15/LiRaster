#pragma once

#include "color.hpp"
#include "line.hpp"
#include "tris.hpp"
#include "rect.hpp"


class Surface {

	public:
		int width;
		int height;
		int pixel_count;
		float aspect_ratio;
	
	private:
		Color *m_data;


	//Methods
	public:
		Surface();
		Surface(Color* data, int w, int h);

		// tonemapping
		void tonemap();
		
		// conversion
		void toU32Surface(uint32_t* buffer);


		// Saving
		int save_float_buffer(const char *file_path);
		int save_ppm(const char *file_path);
		int save_png(const char *file_path);
		

		// Drawing Methods
		void set_at(int x, int y, const Color &color);
		
		void fill(const Color &color);
		void fill_random();
		

		void drawRect(int x0, int y0, int w, int h, const Color &color, int thickness);
		void drawRect(const Vec3 &pos_vec, const Vec3 &size_vec, const Color &color, int thickness);
		void drawRect(const Rect &rect, const Color &color, int thickness);

		void fillRect(int x0, int y0, int w, int h, const Color &color);
		void fillRect(const Vec3 &pos_vec, const Vec3 &size_vec, const Color &color);
		void fillRect(const Rect &rect, const Color &color);


		void drawTris(int x0, int y0, int x1, int y1, int x2, int y2, const Color &color, int thickness);
		void drawTris(const Vec3 &v1,const Vec3 &v2, const Vec3 &v3, const Color &color, int thickness);
		void drawTris(const Tris &tris, const Color &color, int thickness);

		void fillTris(int x0, int y0, int x1, int y1, int x2, int y2, const Color &color);
		void fillTris(const Vec3 &v1, const Vec3 &v2, const Vec3 &v3, const Color &color);
		void fillTris(const Tris &tris, const Color &color);


		void drawLine(int x0, int y0, int x1, int y1, const Color &color, int lineWidth);
		void drawLine(const Vec3 &v1, const Vec3 &v2, const Color &color, int lineWidth);
		void drawLine(const Line &line, const Color &color, int lineWidth);


	private:
		void _aces();
		void _reinhard();
		void _gamma();
};
