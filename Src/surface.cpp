#include <fstream>
#include <assert.h>
#include <time.h>
#include <math.h>


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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
    Color c;
    for (int i=0; i<pixel_count; i++) {
        c = m_data[i] * 0xFF;
        buffer[i] = (uint32_t) (
            ((uint8_t) c.r<<24) |
            ((uint8_t) c.g<<16) |
            ((uint8_t) c.b<< 8) |
            0xFF);
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
    m_data[y*width + x] = color;
}


void Surface::fill(const Color &color) {
	for (int i=0; i<pixel_count; ++i) {
		m_data[i] = color;
	}
}

void Surface::fill_random() {
	srand(time(NULL));
	for (int i = 0; i < pixel_count; ++i) {
		m_data[i] = rand_color();
	}
}



void Surface::drawRect(int x0, int y0, int w, int h, const Color &color, int thickness) {
    this->drawLine(x0, y0, x0+w, y0, color, thickness);   
    this->drawLine(x0, y0+h, x0+w, y0+h, color, thickness);   
    this->drawLine(x0, y0, x0, y0+h, color, thickness);   
    this->drawLine(x0+w, y0, x0+w, y0+h, color, thickness);   
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

void Surface::fillRect(const Rect &rect, const Color &color) {
    this->fillRect(rect.x, rect.y, rect.w, rect.h, color);
}



void Surface::drawTris(int x0, int y0, int x1, int y1, int x2, int y2, const Color &color, int thickness) {
    this->drawLine(x0, y0, x1, y1,  color, thickness);
    this->drawLine(x1, y1, x2, y2,  color, thickness);
    this->drawLine(x0, y0, x2, y2,  color, thickness);
}

void Surface::drawTris(const Tris &tris, const Color &color, int thickness) {
    this->drawTris( tris.x0, tris.y0, tris.x1, tris.y1, tris.x2, tris.y2, color, thickness);    
}

void Surface::fillTris(int x0, int y0, int x1, int y1, int x2, int y2, const Color &color) {
    if (y0 > y1) { std::swap(x0, x1); std::swap(y0, y1); }
    if (y0 > y2) { std::swap(x0, x2); std::swap(y0, y2); }
    if (y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); }

    int dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0;
    int slope1 = (dy01 != 0) ? (dx01 << 16) / dy01 : 0, slope2 = (dy02 != 0) ? (dx02 << 16) / dy02 : 0;

    for (int y = y0; y <= y2; y++) {
        int startX1 = x0 + ((y - y0) * slope1 >> 16);
        int endX1 = x0 + ((y - y0) * slope2 >> 16);

        if (startX1 > endX1) std::swap(startX1, endX1);

        for (int x = startX1; x <= endX1; x++)
            this->set_at(x, y, color);
    }
}

void Surface::fillTris(const Tris &tris, const Color &color) {
    this->fillTris(tris.x0,  tris.y0,  tris.x1,  tris.y1,  tris.x2, tris.y2, color);
}



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

void Surface::drawLine(const Line &line, const Color &color, int lineWidth) {
    this->drawLine(line.x0, line.y0, line.x1, line.y1, color, lineWidth);
}
