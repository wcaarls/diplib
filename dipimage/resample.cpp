/*
 * DIPimage 3.0
 * This MEX-file implements the `resample` function
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
#include "diplib/geometry.h"

void mexFunction( int /*nlhs*/, mxArray* plhs[], int nrhs, const mxArray* prhs[] ) {
   try {

      DML_MIN_ARGS( 1 );
      DML_MAX_ARGS( 5 );

      dml::MatlabInterface mi;
      dip::Image const in = dml::GetImage( prhs[ 0 ] );
      dip::Image out = mi.NewImage();

      dip::FloatArray zoom = { 2.0 };
      if( nrhs > 1 ) {
         zoom = dml::GetFloatArray( prhs[ 1 ] );
      }

      dip::FloatArray shift = { 0.0 };
      if( nrhs > 2 ) {
         shift = dml::GetFloatArray( prhs[ 2 ] );
      }

      dip::String method = "";
      if( nrhs > 3 ) {
         method = dml::GetString( prhs[ 3 ] );
      }

      dip::StringArray boundaryCondition = {};
      if( nrhs > 4 ) {
         boundaryCondition = dml::GetStringArray( prhs[ 4 ] );
      }

      dip::Resampling( in, out, zoom, shift, method, boundaryCondition );

      plhs[ 0 ] = dml::GetArray( out );

   } catch( const dip::Error& e ) {
      mexErrMsgTxt( e.what() );
   }
}
