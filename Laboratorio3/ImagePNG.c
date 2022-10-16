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

void resize( color[] img,int width_img,int height_img,int x, int y,int scale){
  a=0;
  for(int i = 0; i< height_img*scale;i+=1*scale){
    for(int j = 0; j< width_img*scale;j++){
      for(int k = 0; k<scale;k++){
        stroke(img[a/scale]);
        point(x+j,y+i+k);
        //println(a/scale);
      } 
      if(a<width_img*height_img*(scale)-1){
        a++;
      }  
    }
    
  }
  
}
