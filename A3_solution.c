/* FILE: A3_solutions.c is where you will code your answers for Assignment 3.
 * 
 * Each of the functions below can be considered a start for you. They have 
 * the correct specification and are set up correctly with the header file to
 * be run by the tester programs.  
 *
 * You should leave all of the code as is, especially making sure not to change
 * any return types, function name, or argument lists, as this will break
 * the automated testing. 
 *
 * Your code should only go within the sections surrounded by
 * comments like "REPLACE EVERTHING FROM HERE... TO HERE.
 *
 * The assignment document and the header A3_solutions.h should help
 * to find out how to complete and test the functions. Good luck!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "A3_provided_functions.h"

unsigned char*
bmp_open( char* bmp_filename,        unsigned int *width, 
          unsigned int *height,      unsigned int *bits_per_pixel, 
          unsigned int *padding,     unsigned int *data_size, 
          unsigned int *data_offset                                  )
{

  FILE *bmpfile = fopen( bmp_filename, "rb" );
  
  if( bmpfile == NULL ){
    printf( "I was unable to open the file.\n" );
    return NULL;
  }

  // Read the B and M characters into chars
  // char b, m;
  // fread (&b,1,1,bmpfile);
  // fread (&m,1,1,bmpfile);

  // // Print the B and M to terminal
  // printf( "The first byte was: %c.\n", b );
  // printf( "The second byte was: %c.\n", m );

  // Read the overall file size 
  unsigned int overallFileSize;
  
  fseek(bmpfile,0,SEEK_END);
  overallFileSize=ftell(bmpfile);


  //printf( "The size was: %d.\n", overallFileSize );

  // Rewind file pointer to the beginning and read the entire contents.
  rewind(bmpfile);

  char* imageData=(char*)malloc(overallFileSize+1);

  if( fread( imageData, 1, overallFileSize, bmpfile ) != overallFileSize ){
    printf( "I was unable to read the requested %d bytes.\n", overallFileSize );
    return NULL;
  }

  fseek(bmpfile,10,0);
  fread(data_offset,1,4,bmpfile);
   
  fseek(bmpfile,18,0);
  fread(width,1,4,bmpfile);

  fseek(bmpfile,22,0);
  fread(height,1,4,bmpfile);

  
  fseek(bmpfile,28,0);
  unsigned short a;
  fread(&a,1,2,bmpfile);

  *bits_per_pixel=a;
  
  *data_size= overallFileSize;

  // data_offset= (unsigned int*)(imageData+10);
  // // Read the width size into unsigned int (hope = 500 since this is the width of utah.bmp)
  // width = (unsigned int*)(imageData+18);

  // // Print the width size to terminal 
  // printf( "The width is: %d.\n", *width );
  
  // height = (unsigned int*)(imageData+22);

  //*bits_per_pixel= *((unsigned short*)(imageData+28));

  *padding= (*width)%4;

  return imageData;  
}

void 
bmp_close( unsigned char **img_data )
{
  free(*img_data);
  img_data=NULL;
}

unsigned char***  
bmp_scale( unsigned char*** pixel_array, unsigned char* header_data, unsigned int header_size,
           unsigned int* width, unsigned int* height, unsigned int num_colors,
           float scale )
{
 
   int oldWidth=*width;
  int oldHeight=*height;


  *((unsigned int *)(header_data + 18))=(unsigned int)((*width)*scale);
  
  *((unsigned int *)(header_data +22))=(unsigned int)((*height)*scale);
    
    unsigned short bits_per_pixel=*((unsigned short*)(header_data +28));
    
    //size
    *((unsigned int*)(header_data+2))=(unsigned int)(oldHeight*oldWidth*scale*scale*(*(unsigned short*)(header_data+28))/8+header_size);
  
    
    unsigned char*** new_pixel_array = NULL;
    

    *width=(unsigned int)*width*scale;
    *height=(unsigned int)*height*scale;
    
    new_pixel_array=(unsigned char***)malloc((*height)*sizeof(unsigned char**));
    
    for( int row=0; row<*height; row++ ){
        new_pixel_array[row] = (unsigned char**)malloc( sizeof(unsigned char*) * (*width) );
        for(int col=0; col<*width; col++ ){
            new_pixel_array[row][col] = (unsigned char*)malloc( sizeof(unsigned char) * (num_colors) );
        }
    }
    
    for(int row=0 ; row< *height ; row++ ){
        for(int col=0 ; col<*width ; col++){
            for(int color=0 ; color<num_colors ; color++ ){
                
                int oldrow=(int)(row/scale);
                int oldcol=(int)(col/scale);
                
                if(oldrow>oldHeight)  oldrow=oldHeight;
                
                if(oldcol>oldWidth)   oldcol=oldWidth;

                new_pixel_array[row][col][color]= pixel_array [oldrow][oldcol][color] ;
            }
        }
    }
  return new_pixel_array;
}         

int 
bmp_collage( char* background_image_filename,     char* foreground_image_filename, 
             char* output_collage_image_filename, int row_offset,                  
             int col_offset,                      float scale )
{

  unsigned char* front_header_data;
  unsigned int front_header_size, front_width, front_height, front_num_color;
  unsigned char*** back_pixel_array=NULL;

  back_pixel_array=bmp_to_3D_array(foreground_image_filename, &front_header_data, &front_header_size, &front_width,&front_height, &front_num_color);


  unsigned char* back_header_data;
  unsigned int back_header_size, back_width, back_height, back_num_color;
 
  if (back_pixel_array==NULL){
    printf("Memory cannot be allocated\n");
  }

  unsigned char*** scaled_front_array=bmp_scale(back_pixel_array, front_header_data, front_header_size, &front_width, &front_height, front_num_color,scale);

  back_pixel_array=bmp_to_3D_array(background_image_filename, &back_header_data, &back_header_size, &back_width,&back_height, &back_num_color);

  
  for (int i =0; i< front_height; i++){
    for(int j=0; j< front_width; j++){
      if(scaled_front_array[i][j][0]!=0){
        for (int k = 0; k< back_num_color; k++ ){
          back_pixel_array[i+row_offset][j+col_offset][k]=scaled_front_array[i][j][k];

        }
      }
    }

  }

  bmp_from_3D_array(output_collage_image_filename,back_header_data, back_header_size, back_pixel_array, back_width, back_height, back_num_color);


  free(back_pixel_array);
  
  return 0;

  // unsigned int backWidth, backHeight, foreWidth,foreHeight,
  //              back_header_size, fore_header_size,
  //              back_num_colors, fore_num_colors;

  // // unsigned int back_padding,fore_padding,
  // //               back_data_size,fore_data_size,
  // //               back_data_offset,fore_data_offset;

  // unsigned char*   back_image_data, fore_image_data;

  // unsigned char*   back_header_data;
  // unsigned char* fore_header_data;

  // unsigned char*** fore_pixel_array = NULL;
  // unsigned char*** back_pixel_array = NULL;
  
  // back_pixel_array = bmp_to_3D_array(background_image_filename, &back_header_data, 
  //                                &back_header_size,  &backWidth, 
  //                                &backHeight,       &back_num_colors  );

  // fore_pixel_array = bmp_to_3D_array(foreground_image_filename, &fore_header_data, 
  //                                &fore_header_size,  &foreWidth, 
  //                                &foreHeight,       &fore_num_colors  );

  
                                 
  // if( fore_pixel_array == NULL ){
  //   printf( "Error: bmp_to_3D_array failed for fore_file.\n" );
  //   return -1;
  // }
                                 
  // unsigned char*** scaled_pixel_array = bmp_scale( fore_pixel_array, fore_header_data, fore_header_size,
  //                                                  &foreWidth, &foreHeight, fore_num_colors,  scale   );

  // printf("foreHeight is %d\n",foreHeight );
  // printf("foreWidth is %d\n",foreWidth );
  // if(fore_num_colors!=4|back_num_colors!=4){
  //   printf("at least one input picture does not have 4 aRGB format\n");
  //   return -1;
  // }

  // if((foreHeight+row_offset)>backHeight|(foreWidth+col_offset)>backWidth){
  //   printf("incompatible size of pictures\n");
  //   return -1;
  // }

  // for(int i=0;i<foreHeight;i++){
  //   for(int j=0;j<foreWidth;j++){
  //     int curRow=row_offset+i;
  //     int curCol=col_offset+j;

  //     if(fore_pixel_array[i][j][0]!=0){
  //       for(int k=0;k<4;i++){


  //       //memset(back_pixel_array[curRow][curCOl], *scaled_pixel_array[i][j], 8);
  //         back_pixel_array[curRow][curCol][k]=scaled_pixel_array[i][j][k];
  //       }
  //     }
      
  //   }
  // }
  //  //bmp_from_3D_array(output_collage_image_filename,fore_header_data,fore_header_size,scaled_pixel_array,foreWidth,foreHeight,fore_num_colors);
  // bmp_from_3D_array(output_collage_image_filename,back_header_data,back_header_size,back_pixel_array,backWidth,backHeight,back_num_colors);

  // return 0;
}              

