#include <target/video.h>

void video_set_underline(void)
{
}

void video_clear_underline(void)
{
}

void video_set_foreground(video_rgb_t fg)
{
}

void video_set_background(video_rgb_t bg)
{
}

void video_draw_text(video_pt_t x, video_pt_t y, const char *string, int len)
{
}

void video_init(void)
{
	video_hw_ctrl_init();
}
