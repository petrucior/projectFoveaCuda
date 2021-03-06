/**
 * \file mmf.h
 * 
 * \brief This file contains the prototype and implementation of strategies
 * to move the foveae.
 *
 * \author
 * Petrucio Ricardo Tavares de Medeiros \n
 * Universidade Federal do Rio Grande do Norte \n 
 * Departamento de Computacao e Automacao Industrial \n
 * petrucior at gmail (dot) com
 *
 * \version 0.1
 * \date April 2019
 *
 * This file is part of projectFoveaCuda software.
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MMF_H
#define MMF_H

#include <stdio.h>
#include <iostream>
#ifdef _OPENMP
#include <omp.h>
#endif
#ifdef __CUDACC__
#include <cuda.h>
#include <cuda_runtime.h>
#endif

using namespace cv;

/**
 * \struct MMF
 *
 * \brief Struct for fovea using OpenMP and Cuda libraries.
 */
struct MMF{
  
  //
  // Methods
  //

  /**
   * \fn Point getDelta( int k, int m, Point w, Point u, Point f )
   *
   * \brief Calculates the initial pixel to build MMF.
   *
   * \param k - Level of fovea
   *        m - Number levels of fovea
   *        w - Size of levels
   *        u - Size of image
   *        f - Position (x, y) to build the fovea
   *
   * \return Return the initial pixel on the both axis of level k to build MMF.
   */
#ifdef __CUDACC__
  __device__
#endif
  Point getDelta( int k, int m, Point w, Point u, Point f );
  
  /**
   * \fn Point getSize( int k, int m, Point w, Point u )
   *
   * \brief Calculates the final pixel to build MMF.
   *
   * \param k - Level of fovea
   *        m - Number levels of fovea
   *        w - Size of levels
   *        u - Size of image
   *
   * \return Return the final pixel on the both axis of level k to build MMF.
   */
#ifdef __CUDACC__
  __device__
#endif
  Point getSize( int k, int m, Point w, Point u );

  /**
   * \fn Point mapLevel2Image( int k, int m, Point w, Point u, Point f, Point px )
   *
   * \brief Calculates the position of pixel on the level to image.
   *
   * \param k - Level of fovea
   *        m - Number levels of fovea
   *        w - Size of levels
   *        u - Size of image
   *        f - Position (x, y) of the fovea
   *        px - Pixel (x, y) that we want to map.
   *
   * \return Return the position of pixel on the both axis to image.
   */
  Point mapLevel2Image( int k, int m, Point w, Point u, Point f, Point px );

  /**
   * \fn Mat MMF_CPU( Mat img, int k, int m, Point w, Point u, Point f )
   *
   * \brief Calculates the levels of MMF method using CPUs.
   *
   * \param img - Image to be foveated
   *        k - Level of fovea
   *        m - Number levels of fovea
   *        w - Size of levels
   *        u - Size of image
   *        f - Position (x, y) of the fovea
   *
   * \return Return the level of MMF method.
   */
  Mat MMF_CPU( Mat img, int k, int m, Point w, Point u, Point f );
  
  /**
   * \fn __global__ void MMF_GPU( cv::cuda::GpuMat* img, std::vector< cv::cuda::GpuMat >* output, int m, Point w, Point u, Point f )
   *
   * \brief Calculates the levels of MMF method using GPUs. This approach consideres 
   * that MMF was built by n threads, where each thread will process a single level.
   *
   * \param img - Image pointer to be foveated
   *        output - Image pointer for level of MMF method
   *        m - Number levels of fovea
   *        w - Size of levels
   *        u - Size of image
   *        f - Position (x, y) of the fovea
   */
#ifdef __CUDACC__
  __global__
#endif
  void MMF_GPU( cv::cuda::GpuMat* img, std::vector< cv::cuda::GpuMat >* output, int m, Point w, Point u, Point f );
  
  /**
   * \fn Mat MMF_GPU( Mat img, int k, int m, Point w, Point u, Point f )
   *
   * \brief Calculates the levels of MMF method using GPUs.
   *
   * \param img - Image to be foveated
   *        k - Level of fovea
   *        m - Number levels of fovea
   *        w - Size of levels
   *        u - Size of image
   *        f - Position (x, y) of the fovea
   *
   * \return Return the level of MMF method.
   */
  Mat MMF_GPU( Mat img, int k, int m, Point w, Point u, Point f );
  

  /**
   * \fn Mat foveated( Mat img, int m, Point w, Point u, Point f, int method )
   *
   * \brief Calculates the levels of MMF method.
   *
   * \param img - Image to be foveated
   *        m - Number levels of fovea
   *        w - Size of levels
   *        u - Size of image
   *        f - Position (x, y) of the fovea
   *        method - If (0) by default will be considered MMF_CPU, else (1) will be 
   *        considered MMF_GPU. 
   *
   * \return Return the level of MMF method.
   */
  Mat foveated( Mat img, int m, Point w, Point u, Point f, int method );
  
  
};

#endif

// Implementation

/**
 * \fn Point getDelta( int k, int m, Point w, Point u, Point f )
 *
 * \brief Calculates the initial pixel to build MMF.
 *
 * \param k - Level of fovea
 *        m - Number levels of fovea
 *        w - Size of levels
 *        u - Size of image
 *        f - Position (x, y) to build the fovea
 *
 * \return Return the initial pixel on the both axis of level k to build MMF.
 */
#ifdef __CUDACC__
  __device__
#endif
Point 
MMF::getDelta( int k, int m, Point w, Point u, Point f ){
  int dx = int( k * ( u.x - w.x + ( 2 * f.x ) ) )/ ( 2 * m );
  int dy = int( k * ( u.y - w.y + ( 2 * f.y ) ) )/ ( 2 * m );
#ifdef DEBUG
  std::cout << "Delta: ( " << dx << ", " << dy << " ) " << std::endl;  
#endif
  return Point( dx, dy );
}

/**
 * \fn Point getSize( int k, int m, Point w, Point u )
 *
 * \brief Calculates the final pixel to build MMF.
 *
 * \param k - Level of fovea
 *        m - Number levels of fovea
 *        w - Size of levels
 *        u - Size of image
 *
 * \return Return the final pixel on the both axis of level k to build MMF.
 */
#ifdef __CUDACC__
  __device__
#endif
Point 
MMF::getSize( int k, int m, Point w, Point u ){
  int sx = ((m * u.x) + (w.x * k) - (k * u.x)) / m;
  int sy = ((m * u.y) + (w.y * k) - (k * u.y)) / m;
#ifdef DEBUG
  std::cout << "Size: ( " << sx << ", " << sy << " ) " << std::endl;  
#endif
  return Point( sx, sy );
}

/**
 * \fn Point mapLevel2Image( int k, int m, Point w, Point u, Point f, Point px )
 *
 * \brief Calculates the position of pixel on the level to image.
 *
 * \param k - Level of fovea
 *        m - Number levels of fovea
 *        w - Size of levels
 *        u - Size of image
 *        f - Position (x, y) of the fovea
 *        px - Pixel (x, y) that we want to map.
 *
 * \return Return the position of pixel on the both axis to image.
 */
Point 
MMF::mapLevel2Image( int k, int m, Point w, Point u, Point f, Point px ){
  int _px = ( (k * w.x) * (u.x - w.x) + (2 * k * w.x * f.x) + (2 * px.x) * ( (m * u.x) - (k * u.x) + (k * w.x) ) )/ (2 * m * w.x);
  int _py = ( (k * w.y) * (u.y - w.y) + (2 * k * w.y * f.y) + (2 * px.y) * ( (m * u.y) - (k * u.y) + (k * w.y) ) )/ (2 * m * w.y);
#ifdef DEBUG
  std::cout << "Map: ( " << _px << ", " << _py << " ) " << std::endl;  
#endif
  return Point( _px, _py );
}

/**
 * \fn Mat MMF_CPU( Mat img, int k, int m, Point w, Point u, Point f )
 *
 * \brief Calculates the levels of MMF method using CPUs.
 *
 * \param img - Image to be foveated
 *        k - Level of fovea
 *        m - Number levels of fovea
 *        w - Size of levels
 *        u - Size of image
 *        f - Position (x, y) of the fovea
 *
 * \return Return the level of MMF method.
 */
Mat 
MMF::MMF_CPU( Mat img, int k, int m, Point w, Point u, Point f ){
  Point d = getDelta( k,  m, w, u, f );
  Point s = getSize( k, m, w, u );
  Mat imgLevel = img( Rect( d.x, d.y, s.x, s.y ) ); // Getting ROI of image
  if ( k < m )
    resize( imgLevel, imgLevel, Size(w.x, w.y), 0, 0, CV_INTER_LINEAR );
#ifdef DEBUG
  imshow( "levels", imgLevel );
  waitKey( 0 ); // Waiting enter
#endif
  return imgLevel;
}

/**
 * \fn __global__ void MMF_GPU( cv::cuda::GpuMat* img, std::vector< cv::cuda::GpuMat >* output, int m, Point w, Point u, Point f )
 *
 * \brief Calculates the levels of MMF method using GPUs. This approach consideres 
 * that MMF was built by n threads, where each thread will process a single level.
 *
 * \param img - Image pointer to be foveated
 *        output - Image pointer for level of MMF method
 *        m - Number levels of fovea
 *        w - Size of levels
 *        u - Size of image
 *        f - Position (x, y) of the fovea
 */
#ifdef __CUDACC__
__global__ 
#endif
void 
MMF::MMF_GPU( cv::cuda::GpuMat* img, std::vector< cv::cuda::GpuMat >* output, int m, Point w, Point u, Point f ){
#ifdef __CUDACC__
  // Getting index of current kernel
  int tid = threadIdx.x + blockIdx.x * blockDim.x;
  while ( tid < m + 1 ){ // Each level will be accessed
    Point d = getDelta( tid,  m, w, u, f );
    Point s = getSize( tid, m, w, u );
    cv::cuda::GpuMat imgLevel = img( Rect( d.x, d.y, s.x, s.y ) ); // Getting ROI of image
    if ( k < m )
      cv::cuda::resize( imgLevel, imgLevel, w, cv::INTER_LINEAR); // Read page 171 of book Hands on GPU Accelerated Computer Vision With OpenCV And Cuda*/
    output[tid] = imgLevel; // Updating vector with levels of multiresolution
    tid += blockDim.x * gridDim.x;
  }
#endif
}

/**
 * \fn Mat MMF_GPU( Mat img, int k, int m, Point w, Point u, Point f )
 *
 * \brief Calculates the levels of MMF method using GPUs.
 *
 * \param img - Image to be foveated
 *        k - Level of fovea
 *        m - Number levels of fovea
 *        w - Size of levels
 *        u - Size of image
 *        f - Position (x, y) of the fovea
 *
 * \return Return the level of MMF method.
 */
Mat 
MMF::MMF_GPU( Mat img, int k, int m, Point w, Point u, Point f ){
  /*Point d = getDelta( k,  m, w, u, f );
  Point s = getSize( k, m, w, u );
  //Mat imgLevel( s.y - d.y, s.x - d.x, img.type() );
  Mat h_imgLevel = img( Rect( d.x, d.y, s.x, s.y ) ); // Getting ROI of image
  cv::cuda::GpuMat d_imgLevel, d_imgLevelResult; // Declaring images on device
  d_imgLevel.upload( h_imgLevel ); // Uploading imgLevel to device
  if ( k < m )
  cv::cuda::resize( d_imgLevel, d_imgLevelResult, w, cv::INTER_LINEAR); // Read page 171 of book Hands on GPU Accelerated Computer Vision With OpenCV And Cuda*/
  Mat h_imgLevelResult;
  /*d_imgLevelResult.download( h_imgLevelResult );
#ifdef DEBUG
  imshow( "levels", h_imgLevelResult );
  waitKey( 0 ); // Waiting enter
  #endif*/
  return h_imgLevelResult;
}

/**
 * \fn Mat foveated( Mat img, int m, Point w, Point u, Point f, int method )
 *
 * \brief Calculates the levels of MMF method.
 *
 * \param img - Image to be foveated
 *        m - Number levels of fovea
 *        w - Size of levels
 *        u - Size of image
 *        f - Position (x, y) of the fovea
 *        method - If (0) by default will be considered MMF_CPU, else (1) will be 
 *        considered MMF_GPU. 
 *
 * \return Return the level of MMF method.
 */
Mat 
MMF::foveated( Mat img, int m, Point w, Point u, Point f, int method ){
  Mat imgFoveated = img.clone();
  if ( method == 0 ){ // MMF_CPU
#ifdef _OPENMP
#pragma omp parallel for schedule(static, m+1) // Schedule(static, m+1) keeps the order
#endif
    for ( int k = 0; k <= m; k++ ){ // Levels
      Mat imgLevel = MMF_CPU( img, k, m, w, u, f );
      // Mapping levels to foveated image
      Point initial = mapLevel2Image( k, m, w, u, f, Point( 0, 0 ) ); 
      Point final = mapLevel2Image( k, m, w, u, f, Point( w.x, w.y ) );
#ifdef DEBUG
      std::cout << "(xi, yi) = (" << initial.x << ", " << initial.y << ")" << std::endl;
      std::cout << "(xf, yf) = (" << final.x << ", " << final.y << ")" << std::endl;
#endif
      Rect roi = Rect( initial.x, initial.y, final.x - initial.x, final.y - initial.y );
      if ( k < m ){ // Copying levels to foveated image
	resize( imgLevel, imgLevel, Size(final.x - initial.x, final.y - initial.y), 0, 0, CV_INTER_LINEAR );
	imgLevel.copyTo( imgFoveated( roi ) );
      }
      else
	imgLevel.copyTo( imgFoveated( roi ) );      
    }
  }
  else{ // MMF_GPU
#ifdef __CUDACC__
    cv::cuda::GpuMat d_img, d_imgFoveated; // Declaring images on device
    std::vector< cv::cuda::GpuMat > d_output;
    d_img.upload( img ); // Uploading img to device
    d_imgFoveated.upload( img ); // Copying img to d_imgFoveated
    MMF_GPU << < 512, 512 > >> ( d_img, d_output, m, w, u, f );
    std::vector< Mat > output;
    //output.download( d_output );
    // This part of "blend images" can be done by a function
    
    for ( int k = 0; k <= m; k++ ){ // Levels
      // Mapping levels to foveated image
      Point initial = mapLevel2Image( k, m, w, u, f, Point( 0, 0 ) ); 
      Point final = mapLevel2Image( k, m, w, u, f, Point( w.x, w.y ) );
#ifdef DEBUG
      std::cout << "(xi, yi) = (" << initial.x << ", " << initial.y << ")" << std::endl;
      std::cout << "(xf, yf) = (" << final.x << ", " << final.y << ")" << std::endl;
#endif
      Rect roi = Rect( initial.x, initial.y, final.x - initial.x, final.y - initial.y );
      if ( k < m ){ // Copying levels to foveated image
	cv::cuda::resize( output[k], output[k], Size(final.x - initial.x, final.y - initial.y), 0, 0, CV_INTER_LINEAR );
	output[k].copyTo( d_imgFoveated( roi ) );
      }
      else
	output[k].copyTo( d_imgFoveated( roi ) );
    }
    imgFoveated.download( d_imgFovated );
#endif      
  }
  return imgFoveated;
}
 
