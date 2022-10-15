void DrawImage(
et024006_color_t const * pixmap,
uint16_t map_width,
uint16_t x,
uint16_t y,
uint16_t width,
uint16_t height)
{
	for(uint16_t i = 0; i< height; i++)
	{
		for(uint16_t j = 0; j< width; j++)
		{	
			if(*pixmap!=0x0){
				et024006_DrawPixel( x+j, y+i, *pixmap++ );
			}
			else{
				 pixmap++;
			}
		}
	}
}
