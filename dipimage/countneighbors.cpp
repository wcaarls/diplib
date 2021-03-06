/*
 * DIPimage 3.0
 * This MEX-file implements the `countneighbors` function
 *
 * (c)2017, Cris Luengo.
 * Based on original DIPlib code: (c)1995-2014, Delft University of Technology.
 * Based on original DIPimage code: (c)1999-2014, Delft University of Technology.
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

#include "dip_matlab_interface.h"
#include "diplib/binary.h"

void mexFunction( int /*nlhs*/, mxArray* plhs[], int nrhs, const mxArray* prhs[] ) {
   try {

      DML_MIN_ARGS( 1 );
      DML_MAX_ARGS( 4 );

      dml::MatlabInterface mi;
      dip::Image const in = dml::GetImage( prhs[ 0 ] );
      dip::Image out = mi.NewImage();

      dip::uint connectivity = 0;
      if( nrhs > 2 ) {
         connectivity = dml::GetUnsigned( prhs[ 2 ] );
      }

      dip::String mode = dip::S::FOREGROUND;
      if( nrhs > 1 ) { // Note order difference between MATLAB and C++
         if( mxIsChar( prhs[ 1 ] )) {
            mode = dml::GetString( prhs[ 1 ] );
         } else {
            if( !dml::GetBoolean( prhs[ 1 ] )) {
               mode = dip::S::ALL;
            }
         }
      }

      dip::String edgeCondition = dip::S::BACKGROUND;
      if( nrhs > 3 ) {
         if( mxIsChar( prhs[ 3 ] )) {
            edgeCondition = dml::GetString( prhs[ 3 ] );
         } else {
            if( dml::GetBoolean( prhs[ 3 ] )) {
               edgeCondition = dip::S::OBJECT;
            }
         }
      }

      dip::CountNeighbors( in, out, connectivity, mode, edgeCondition );

      plhs[ 0 ] = dml::GetArray( out );

   } catch( const dip::Error& e ) {
      mexErrMsgTxt( e.what() );
   }
}
