#include <fstream>
#include <iostream>
#include <math.h>
#include <time.h>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb_image_write.h"
#pragma GCC diagnostic pop

#include "surface.hpp"
#include "utils.hpp" 



#define ACES_a 0.0245786f
#define ACES_b 0.0245786f
#define ACES_c 0.000090537f
#define ACES_d 0.983729f
#define ACES_e 0.4329510f



// --------- Constructors ---------
Surface::Surface() {}

Surface::Surface(Color* data, int w, int h): width(w), height(h) {
    pixel_count  = width * height;
    aspect_ratio = (float)width/height;
    m_data = data;
}




// --------- Private Methods ---------
void Surface::_aces() {
    for (int i=0; i<pixel_count; i++) {
        m_data[i].r = std::max(0.f, (float)(m_data[i].r*(m_data[i].r+ACES_a) - ACES_b) / (m_data[i].r * (m_data[i].r*ACES_c + ACES_d) + ACES_e));
        m_data[i].g = std::max(0.f, (float)(m_data[i].g*(m_data[i].g+ACES_a) - ACES_b) / (m_data[i].g * (m_data[i].g*ACES_c + ACES_d) + ACES_e));
        m_data[i].b = std::max(0.f, (float)(m_data[i].b*(m_data[i].b+ACES_a) - ACES_b) / (m_data[i].b * (m_data[i].b*ACES_c + ACES_d) + ACES_e));
    }
}

void Surface::_reinhard() {
    for (int i=0; i<pixel_count; i++) {
        m_data[i].r = (float) m_data[i].r/(1+m_data[i].r);
        m_data[i].g = (float) m_data[i].g/(1+m_data[i].g);
        m_data[i].b = (float) m_data[i].b/(1+m_data[i].b);
    }
}

void Surface::_gamma() {
    for (int i=0; i<pixel_count; i++) {
        m_data[i].r = sqrtf(m_data[i].r);
        m_data[i].g = sqrtf(m_data[i].g);
        m_data[i].b = sqrtf(m_data[i].b);
    }
}




// --------- Public Methods ---------
void Surface::tonemap() {
    // this->_aces();  // ACES TMO
    this->_reinhard();  // Reinhard TMO    
    this->_gamma();
}

void Surface::toU32Surface(uint32_t *buffer) {

    uint8_t r;
    uint8_t g;
    uint8_t b;

    for (int i=0; i<pixel_count; i++) {
        r = m_data[i].r * 0xFF;
        g = m_data[i].g * 0xFF;
        b = m_data[i].b * 0xFF;

        buffer[i] = (uint32_t) (r<<24) | (g<<16) | (b<< 8) | 0xFF;
    }
}


// Saving
int Surface::save_float_buffer(const char* file_path) {
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        return -1;
    }

    int size = pixel_count * 3;
    int dim[2] = {width, height};
    fwrite(dim, sizeof(int)*2, 1, file);

    fwrite(m_data, sizeof(float)*size, 1, file);
    fclose(file);
    
    return 0;
}

int Surface::save_ppm(const char* file_path) {
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        return -1;
    }

    fprintf(file, "P6\n%d %d\n255\n", width, height);
    uint8_t *bytes = new uint8_t[3 * pixel_count];

    int j = 0;
    Color c;
    for (int i = 0; i < pixel_count; i++) {
        c = m_data[i] * 255;
        bytes[j++] = std::max(0, std::min(0xff, int(c.r)));  // R
        bytes[j++] = std::max(0, std::min(0xff, int(c.g)));  // G
        bytes[j++] = std::max(0, std::min(0xff, int(c.b)));  // B
    }

    fwrite(bytes, 3*pixel_count*sizeof(uint8_t), 1, file);
    fclose(file);

    delete[] bytes;
    return 0;
}

int Surface::save_png(const char* file_name) {
    uint8_t *bytes = new uint8_t[3 * pixel_count];
    Color c;

    int j = 0;
    for (int i = 0; i < pixel_count; i++) {
        c = m_data[i] * 255.f;
        bytes[j++] = std::max(0, std::min(0xff, (int)c.r));  // R
        bytes[j++] = std::max(0, std::min(0xff, (int)c.g));  // G
        bytes[j++] = std::max(0, std::min(0xff, (int)c.b));  // B
    }

    stbi_write_png(file_name, width, height, 3, bytes, 3*width*sizeof(uint8_t));
    delete[] bytes;
    return 0;
}


// Drawing Methods
void Surface::set_at(int x, int y, const Color &color) {
    int idx = y*width + x;
    if (idx < pixel_count) {
        m_data[idx] = color;
    }
}


// Base
void Surface::fill(const Color &color) {
    std::fill(&m_data[0], &m_data[pixel_count], color);
	// for (int i=0; i<pixel_count; ++i) {
	// 	m_data[i] = color;
	// }
}

void Surface::fill_random() {
	srand(time(NULL));
	for (int i = 0; i < pixel_count; ++i) {
		m_data[i] = rand_color();
	}
}



// Cicles
void Surface::drawCircle(int x0, int y0, int r, const Color &color, int thickness) {
    if ( (r < 1) || (thickness < 1) ) {
        return;
    }

    int y_st = std::max(0, std::min( height, y0 - r));
    int y_en = std::max(0, std::min( height, y0+r+1 ));
    int x_st = std::max(0, std::min( width,  x0 - r));
    int x_en = std::max(0, std::min( width,  x0+r+1 ));

    int r_sq = r*r;
    int rt_sq = (r-thickness)*(r-thickness);

    int d_sq;

    for (int y=y_st; y<y_en; y++) {
        for (int x=x_st; x<x_en; x++) {

            d_sq = (x-x0)*(x-x0) + (y-y0)*(y-y0);
            if ( (d_sq > rt_sq)  && (d_sq < r_sq) ) {
                m_data[(y) * width + (x)] = color;
            }

        }
    }

}

void Surface::drawCircle(const Vec3 &pos_vec, int r, const Color &color, int thickness) {
    this->drawCircle(pos_vec.x, pos_vec.y, r, color, thickness);
}

void Surface::drawCircle(const Circle &circle, const Color &color, int thickness) {
    this->drawCircle(circle.x, circle.y, circle.r, color, thickness);
}


void Surface::fillCircle(int x0, int y0, int r, const Color &color) {
        if ( r < 1 ) {
        return;
    }

    int y_st = std::max(0, std::min( height, y0 - r));
    int y_en = std::max(0, std::min( height, y0+r+1 ));
    int x_st = std::max(0, std::min( width,  x0 - r));
    int x_en = std::max(0, std::min( width,  x0+r+1 ));
    
    for (int y=y_st; y<y_en; y++) {
        for (int x=x_st; x<x_en; x++) {
            if ( ((x-x0)*(x-x0) + (y-y0)*(y-y0)) < (r*r) ) {
                m_data[y*width + x] = color;
            }
        }
    }

}

void Surface::fillCircle(const Vec3 &pos_vec, int r, const Color &color) {
    this->fillCircle(pos_vec.x, pos_vec.y, r, color);
}

void Surface::fillCircle(const Circle &circle, const Color &color) {
    this->fillCircle(circle.x, circle.y, circle.r, color);
}


// Rectangles
void Surface::drawRect(int x0, int y0, int w, int h, const Color &color, int thickness) {
    this->drawLine(x0, y0, x0+w, y0, color, thickness);
    this->drawLine(x0, y0+h, x0+w, y0+h, color, thickness);
    this->drawLine(x0, y0, x0, y0+h, color, thickness);
    this->drawLine(x0+w, y0, x0+w, y0+h, color, thickness);
}

void Surface::drawRect(const Vec3 &pos_vec, const Vec3 &size_vec, const Color &color, int thickness) {
    this->drawRect(pos_vec.x, pos_vec.y, size_vec.x, size_vec.y, color, thickness);
}

void Surface::drawRect(const Rect &rect, const Color &color, int thickness) {
    this->drawRect(rect.x, rect.y, rect.w, rect.h, color, thickness);
}


void Surface::fillRect(int x0, int y0, int w, int h, const Color &color) {
    x0 = std::max(0, x0);
    y0 = std::max(0, y0);

    int x1 = std::min((x0 + w), width);
    int y1 = std::min((y0 + h), height);

    for (int y=y0; y<y1; y++) {
        Color* rowStart = &m_data[y*width + x0];
        std::fill(rowStart, rowStart + (x1 - x0), color);
    }
}

void Surface::fillRect(const Vec3 &pos_vec, const Vec3 &size_vec, const Color &color) {
    this->fillRect(pos_vec.x, pos_vec.y, size_vec.x, size_vec.y, color);
}

void Surface::fillRect(const Rect &rect, const Color &color) {
    this->fillRect(rect.x, rect.y, rect.w, rect.h, color);
}


// Triangles
void Surface::drawTris(int x0, int y0, int x1, int y1, int x2, int y2, const Color &color, int thickness) {
    this->drawLine(x0, y0, x1, y1,  color, thickness);
    this->drawLine(x1, y1, x2, y2,  color, thickness);
    this->drawLine(x0, y0, x2, y2,  color, thickness);
}

void Surface::drawTris(const Vec3 &v1, const Vec3 &v2, const Vec3 &v3, const Color &color, int thickness) {
    this->drawTris(v1.x,  v1.y,  v2.x,  v2.y,  v3.x, v3.y, color, thickness);
}

void Surface::drawTris(const Tris &tris, const Color &color, int thickness) {
    this->drawTris( tris.x0, tris.y0, tris.x1, tris.y1, tris.x2, tris.y2, color, thickness);    
}


void Surface::fillTris(int x0, int y0, int x1, int y1, int x2, int y2, const Color &color) {
    if ((x0 == x1 && y0 == y1) || (x0 == x2 && y0 == y2) || (x1 == x2 && y1 == y2)) {
        return;
    }

    // ScanLine Approach
    if (y0 > y1) { std::swap(x0, x1); std::swap(y0, y1); }
    if (y0 > y2) { std::swap(x0, x2); std::swap(y0, y2); }
    if (y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); }

    float invslope1 = (x1 - x0) / static_cast<float>(y1 - y0);
    float invslope2 = (x2 - x0) / static_cast<float>(y2 - y0);

    float curx1 = x0;
    float curx2 = x0;

    for (int y = y0; y < y1; y++) {
        this->drawLine(static_cast<int>(curx1), y, static_cast<int>(curx2), y, color, 1);
        curx1 += invslope1;
        curx2 += invslope2;
    }

    invslope1 = (x2 - x1) / static_cast<float>(y2 - y1);
    curx1 = x1;

    for (int y = y1; y <= y2; y++) {
        this->drawLine(static_cast<int>(curx1), y, static_cast<int>(curx2), y, color, 1);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void Surface::fillTris(const Vec3 &v1, const Vec3 &v2, const Vec3 &v3, const Color &color) {
    this->fillTris(v1.x,  v1.y,  v2.x,  v2.y,  v3.x, v3.y, color);
}

void Surface::fillTris(const Tris &tris, const Color &color) {
    this->fillTris(tris.x0,  tris.y0,  tris.x1,  tris.y1,  tris.x2, tris.y2, color);
}


// Lines
void Surface::drawLine(int x0, int y0, int x1, int y1, const Color &color, int lineWidth) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    if (dx == 0 && dy == 0) {
        this->set_at(x0, y0, color);  // Single point
        return;
    }

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    // Iterate for each line in the line width
    for (int i = 0; i < lineWidth; i++) {
        int x = x0;
        int y = y0;

        while (x != x1 || y != y1) {
            this->set_at(x, y, color);

            int err2 = 2 * err;
            if (err2 > -dy) {
                err -= dy;
                x += sx;
            }
            if (err2 < dx) {
                err += dx;
                y += sy;
            }
        }

        // Set the final point to the specified color
        this->set_at(x1, y1, color);

        // Offset the start and end points for the next line
        x0 += sy;
        x1 += sy;
        y0 -= sx;
        y1 -= sx;
    }
}

void Surface::drawLine(const Vec3 &v1, const Vec3 &v2, const Color &color, int lineWidth) {
    this->drawLine(v1.x, v1.y, v2.x, v2.y, color, lineWidth);
}

void Surface::drawLine(const Line &line, const Color &color, int lineWidth) {
    this->drawLine(line.x0, line.y0, line.x1, line.y1, color, lineWidth);
}
