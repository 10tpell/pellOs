#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

void framebuffer_init();
void framebuffer_drawPixel(int x, int y, unsigned char attr);
void framebuffer_drawChar(unsigned char ch, int x, int y, unsigned char attr, int zoom);
void framebuffer_drawString(int x, int y, char *s, unsigned char attr);

#endif