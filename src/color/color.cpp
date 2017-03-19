/*
 * DIPlib 3.0
 * This file contains main functionality for color image support.
 *
 * (c)2016, Cris Luengo.
 * Based on original DIPimage code: (c)2014, Cris Luengo;
 *                                  (c)1999-2014, Delft University of Technology.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "diplib.h"
#include "diplib/color.h"
#include "diplib/framework.h"

#include "rgb.h"
#include "cmyk.h"
#include "hsi.h"
#include "hcv.h"
#include "xyz.h"
#include "lab.h"

namespace dip {

ColorSpaceManager::ColorSpaceManager() {
   // grey (or gray)
   Define( "grey", 1 );
   DefineAlias( "gray", "grey" );
   // RGB
   Define( "RGB", 3 );
   DefineAlias( "rgb", "RGB" );
   Register( ColorSpaceConverterPointer( new grey2rgb ));
   Register( ColorSpaceConverterPointer( new rgb2grey ));
   // nlRGB (or R'G'B')
   Define( "R'G'B'", 3 );
   DefineAlias( "nlRGB", "R'G'B'" );
   Register( ColorSpaceConverterPointer( new rgb2nlrgb ));
   Register( ColorSpaceConverterPointer( new nlrgb2rgb ));
   // CMY
   Define( "CMY", 3 );
   DefineAlias( "cmy", "CMY" );
   Register( ColorSpaceConverterPointer( new rgb2cmy ));
   Register( ColorSpaceConverterPointer( new cmy2rgb ));
   // CMYK
   Define( "CMYK", 4 );
   DefineAlias( "cmyk", "CMYK" );
   Register( ColorSpaceConverterPointer( new cmy2cmyk ));
   Register( ColorSpaceConverterPointer( new cmyk2cmy ));
   // HSI
   Define( "HSI", 3 );
   DefineAlias( "hsi", "HSI" );
   Register( ColorSpaceConverterPointer( new grey2hsi ));
   Register( ColorSpaceConverterPointer( new hsi2grey ));
   Register( ColorSpaceConverterPointer( new rgb2hsi ));
   Register( ColorSpaceConverterPointer( new hsi2rgb ));
   // HCV
   Define( "HCV", 3 );
   DefineAlias( "hcv", "HCV" );
   Register( ColorSpaceConverterPointer( new rgb2hcv ));
   Register( ColorSpaceConverterPointer( new hcv2rgb ));
   // HSV
   Define( "HSV", 3 );
   DefineAlias( "hsv", "HSV" );
   Register( ColorSpaceConverterPointer( new hcv2hsv ));
   Register( ColorSpaceConverterPointer( new hsv2hcv ));
   // XYZ
   Define( "XYZ", 3 );
   DefineAlias( "xyz", "XYZ" );
   Register( ColorSpaceConverterPointer( new grey2xyz ));
   Register( ColorSpaceConverterPointer( new rgb2xyz ));
   Register( ColorSpaceConverterPointer( new xyz2grey ));
   Register( ColorSpaceConverterPointer( new xyz2rgb ));
   // Yxy
   Define( "Yxy", 3 );
   DefineAlias( "yxy", "Yxy" );
   Register( ColorSpaceConverterPointer( new xyz2yxy ));
   Register( ColorSpaceConverterPointer( new yxy2grey ));
   Register( ColorSpaceConverterPointer( new yxy2xyz ));
   // Lab (or L*a*b*, CIELAB)
   Define( "Lab", 3 );
   DefineAlias( "lab", "Lab" );
   DefineAlias( "L*a*b*", "Lab" );
   DefineAlias( "l*a*b*", "Lab" );
   DefineAlias( "CIELAB", "Lab" );
   DefineAlias( "cielab", "Lab" );
   Register( ColorSpaceConverterPointer( new grey2lab ));
   Register( ColorSpaceConverterPointer( new xyz2lab ));
   Register( ColorSpaceConverterPointer( new lab2grey ));
   Register( ColorSpaceConverterPointer( new lab2xyz ));
   // Luv (or L*u*v*, CIELUV)
   Define( "Luv", 3 );
   DefineAlias( "luv", "Luv" );
   DefineAlias( "L*u*v*", "Luv" );
   DefineAlias( "l*u*v*", "Luv" );
   DefineAlias( "CIELUV", "Luv" );
   DefineAlias( "cieluv", "Luv" );
   Register( ColorSpaceConverterPointer( new grey2luv ));
   Register( ColorSpaceConverterPointer( new xyz2luv ));
   Register( ColorSpaceConverterPointer( new luv2xyz ));
   Register( ColorSpaceConverterPointer( new luv2grey ));
   // LCH
   Define( "LCH", 3 );
   DefineAlias( "lch", "LCH" );
   DefineAlias( "L*C*H*", "LCH" );
   DefineAlias( "l*c*h*", "LCH" );
   Register( ColorSpaceConverterPointer( new grey2lch ));
   Register( ColorSpaceConverterPointer( new lab2lch ));
   Register( ColorSpaceConverterPointer( new lch2lab ));
   Register( ColorSpaceConverterPointer( new lch2grey ));
}


namespace {

struct ConvertionStep {
   ColorSpaceConverter const* converterFunction;
   dip::uint nOutputChannels;
   bool last = false;
};

using ConvertionStepArray = std::vector< ConvertionStep >;

class ConverterLineFilter : public Framework::ScanLineFilter {
   public:
      ConverterLineFilter( ConvertionStepArray const& steps ) : steps_( steps ) {
         maxIntermediateChannels = steps[ 0 ].nOutputChannels;
         for( dip::uint ii = 1; ii < steps.size() - 1; ++ii ) {
            maxIntermediateChannels = std::max( maxIntermediateChannels, steps[ 1 ].nOutputChannels );
         }
         nBuffers = std::min< dip::uint >( 2, steps.size() - 1 );
      }
      virtual void SetNumberOfThreads( dip::uint threads ) override {
         buffer1_.resize( threads );
         buffer2_.resize( threads );
      }
      virtual void Filter( Framework::ScanLineFilterParameters const& params ) override {
         dip::uint thread = params.thread;
         dip::uint nPixels = params.bufferLength;
         if( nBuffers > 0 ) {
            if( buffer1_[ thread ].size() != nPixels * maxIntermediateChannels ) {
               // TODO: we might need 0, 1 or 2 buffers.
               buffer1_[ thread ].resize( nPixels * maxIntermediateChannels );
               if( nBuffers > 1 ) {
                  buffer2_[ thread ].resize( nPixels * maxIntermediateChannels );
               }
            }
         }
         dfloat* buffer1 = buffer1_[ thread ].data(); // Might not be used...
         dfloat* buffer2 = buffer2_[ thread ].data(); // Might not be used...
         // We initialize 'out' parameters to the input.
         dfloat* out = static_cast< dfloat* >( params.inBuffer[ 0 ].buffer );
         dip::sint outStride = params.inBuffer[ 0 ].stride;
         dip::sint outTStride = params.inBuffer[ 0 ].tensorStride;
         dip::uint outChans = params.inBuffer[ 0 ].tensorLength;
         for( auto const& step : steps_ ) {
            // At each iteration, we read from the previous `out`
            ConstLineIterator< dfloat > input( out, nPixels, outStride, outChans, outTStride );
            // At each iteration, we write to either the output image, or an intermediate buffer
            outChans = step.nOutputChannels;
            if( step.last ) {
               out = static_cast< dfloat* >( params.outBuffer[ 0 ].buffer );
               outStride = params.outBuffer[ 0 ].stride;
               outTStride = params.outBuffer[ 0 ].tensorStride;
            } else {
               out = out == buffer1 ? buffer2 : buffer1;
               outStride = outChans;
               outTStride = 1;
            }
            LineIterator< dfloat > output( out, nPixels, outStride, outChans, outTStride );
            step.converterFunction->Convert( input, output );
         }
      }
   private:
      ConvertionStepArray const& steps_;
      dip::uint maxIntermediateChannels;
      dip::uint nBuffers;
      std::vector< std::vector< dfloat >> buffer1_; // one for each thread
      std::vector< std::vector< dfloat >> buffer2_; // one for each thread
      // We use up to 2 buffers. If there's 1 step, we don't need a buffer, we can read from in and write to out.
      // If we have 2 steps, we need one buffer (in->buffer->out). If we have more steps, then we need 2 buffers,
      // which are alternated at each step. The last steps always writes to out, and the first step always reads
      // from in.
      // This means that the conversion functions don't need to worry about input and output being the same buffer.
      // It also means we don't need to worry about how many channels an intermediate representation needs.
};

} // namespace

void ColorSpaceManager::Convert(
      Image const& in,
      Image& out,
      String const& endColorSpace
) const {
   DIP_START_STACK_TRACE
      // Make sure the input color space is consistent
      String const& startColorSpace = in.ColorSpace();
      dip::uint startIndex = Index( startColorSpace.empty() ? "grey" : startColorSpace );
      DIP_THROW_IF( in.TensorElements() != colorSpaces_[ startIndex ].nChannels, E::INCONSISTENT_COLORSPACE );
      // Get the output color space
      dip::uint endIndex = Index( endColorSpace.empty() ? "grey" : endColorSpace );
      if( startIndex == endIndex ) {
         // Nothing to do
         out = in;
         return;
      }
      // Find a path from start to end
      std::vector< dip::uint > path = FindPath( startIndex, endIndex );
      DIP_THROW_IF( path.empty(), "No conversion possible between color spaces " +
                                  ( startColorSpace.empty() ? "grey" : startColorSpace ) + " and " +
                                  ( endColorSpace.empty() ? "grey" : endColorSpace ) );
      DIP_ASSERT( path.size() > 1 ); // It should have at least start and stop on it!
      // Collect information about the converter functions along the path
      dip::uint nSteps = path.size() - 1;
      ConvertionStepArray steps( nSteps );
      //std::cout << "Found path: ";
      for( dip::uint ii = 0; ii < nSteps; ++ii ) {
         //std::cout << colorSpaces_[ path[ ii ] ].name << " -> ";
         steps[ ii ].nOutputChannels = colorSpaces_[ path[ ii + 1 ] ].nChannels;
         auto it = colorSpaces_[ path[ ii ] ].edges.find( path[ ii + 1 ] );
         DIP_ASSERT( it != colorSpaces_[ path[ ii ] ].edges.end() );
         steps[ ii ].converterFunction = it->second.get();
      }
      steps.back().last = true;
      //std::cout << colorSpaces_[ path.back() ].name << std::endl;
      // Call scan framework
      ConverterLineFilter lineFilter( steps );
      Framework::ScanMonadic(
            in,
            out,
            DT_DFLOAT,
            DataType::SuggestFloat( in.DataType() ),
            steps.back().nOutputChannels,
            lineFilter
      );
      out.SetColorSpace( colorSpaces_[ endIndex ].name );
   DIP_END_STACK_TRACE
}

namespace {
struct QueueElement {
   dip::uint cost;
   dip::uint index;
   bool operator >( QueueElement const& other ) const { return cost > other.cost; }
};
} // namespace

std::vector< dip::uint > ColorSpaceManager::FindPath( dip::uint start, dip::uint stop ) const {
   constexpr dip::uint NOT_VISITED = std::numeric_limits< dip::uint >::max();
   std::vector< dip::uint > cost( colorSpaces_.size(), NOT_VISITED );
   std::vector< dip::uint > previous( colorSpaces_.size(), 0 );
   PriorityQueueLowFirst< QueueElement > queue;
   queue.push( { 0, start } );
   cost[ start ] = 0;
   while( !queue.empty() ) {
      dip::uint k = queue.top().index;
      dip::uint c = queue.top().cost;
      queue.pop();
      if( cost[ k ] < c ) { // it was already processed earlier.
         continue;
      }
      if( k == stop ) {
         // We're done.
         break;
      }
      for( auto& edge : colorSpaces_[ k ].edges ) {
         dip::uint nk = edge.first; // target color space
         dip::uint nc = c + edge.second->Cost();
         if( cost[ nk ] > nc ) {
            cost[ nk ] = nc;
            previous[ nk ] = k;
            queue.push( { nc, nk } );
         }
      }
   }
   std::vector< dip::uint > path;
   if( cost[ stop ] != NOT_VISITED ) {
      dip::uint k = stop;
      while( k != start ) {
         dip::uint n = k;
         k = previous[ n ];
         path.push_back( k );
         // The path contains indices to the color spaces including "start", but not including "stop".
         // The path is reversed, "start" is the last entry pushed onto it.
      }
      // Reverse path so that "start" is the first element.
      std::reverse( path.begin(), path.end() );
      // Add the stop index to the end of the path.
      path.push_back( stop );
   }
   return path;
}

} // namespace dip


#ifdef DIP__ENABLE_DOCTEST
#include "doctest.h"

DOCTEST_TEST_CASE("[DIPlib] testing the ColorSpaceManager class") {
   dip::ColorSpaceManager csm;
   DOCTEST_CHECK( csm.NumberOfChannels( "rgb" ) == 3 );
   DOCTEST_CHECK( csm.NumberOfChannels( "CMYK" ) == 4 );
   DOCTEST_CHECK( csm.NumberOfChannels( "grey" ) == 1 );
   DOCTEST_CHECK( csm.CanonicalName( "CIELUV" ) == "Luv" );
   DOCTEST_CHECK( csm.GetColorSpaceConverter( "rgb", "cmy" )->Cost() == 1 );
   DOCTEST_CHECK( csm.GetColorSpaceConverter( "rgb", "grey" )->Cost() == 100 );
   dip::Image img( {}, 1 );
   img.Fill( 0 );
   dip::Image out = csm.Convert( img, "RGB" );
   DOCTEST_CHECK( out.ColorSpace() == "RGB" );
   DOCTEST_CHECK( out.TensorElements() == 3 );
   img.SetColorSpace( "CMYK" );
   DOCTEST_CHECK_THROWS( csm.Convert( img, "nlRGB" ) );
   img = dip::Image( {}, 4 );
   img.Fill( 0 );
   img.SetColorSpace( "CMYK" );
   csm.Convert( img, out, "LCH" ); // This is the longest path we have so far.
   DOCTEST_CHECK( out.ColorSpace() == "LCH" );
   DOCTEST_CHECK( out.TensorElements() == 3 );
}

#endif // DIP__ENABLE_DOCTEST
