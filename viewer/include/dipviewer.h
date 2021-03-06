/*
 * DIPlib 3.0 viewer
 * This file contains definitions for all classes need to use the DIPviewer.
 *
 * (c)2017, Wouter Caarls
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

#ifndef DIPVIEWER_H
#define DIPVIEWER_H

#include "diplib.h"
#include "diplib/viewer/export.h"

/// \file
/// \brief Declares the high-level interface to \ref viewer.

namespace dip {

/// \brief Contains all functionality for \ref viewer.
namespace viewer {

/// \addtogroup viewer
/// \{

/// \brief Show an image in the slice viewer.
///
/// A new interactive `dip::viewer::SliceViewer` window is created for the image. `title` sets the window title.
///
/// Call `dip::viewer::Draw` or `dip::Viewer::Spin` to enable user interaction.
///
/// Calling `%dip::viewer::Show` or `dip::viewer::ShowSimple` creates an internal `dip::viewer::Manager` object,
/// which needs to be freed before exiting the application to prevent memory leaks. `dip::viewer::Spin` and
/// `dip::viewer::CloseAll` will free the internal manager object. You need to call one of these two functions
/// at an appropriate time after calling `%dip::viewer::Show` or `dip::viewer::ShowSimple`.
DIPVIEWER_EXPORT void Show( Image const& image, String const& title = "" );

/// \brief Show a 2D grey-value or RGB image, of type `dip::DT_UINT8`.
///
/// A new non-interactive `dip::viewer::ImageViewer` window is created for the image. `title` sets the window title.
///
/// A scalar (grey-value) image will be replicated across three channels to form an RGB image (meaning that data
/// will be copied).
///
/// Calling `dip::viewer::Show` or `%dip::viewer::ShowSimple` creates an internal `dip::viewer::Manager` object,
/// which needs to be freed before exiting the application to prevent memory leaks. `dip::viewer::Spin` and
/// `dip::viewer::CloseAll` will free the internal manager object. You need to call one of these two functions
/// at an appropriate time after calling `dip::viewer::Show` or `%dip::viewer::ShowSimple`.
DIPVIEWER_EXPORT void ShowSimple( Image const& image, String const& title = "" );

/// \brief Wait until all windows are closed.
///
/// This function allows user interaction in all slice viewer windows created. Returns when all windows are closed.
/// It also frees the internal window manager object.
///
/// Calling `dip::viewer::Show` or `dip::viewer::ShowSimple` creates an internal `dip::viewer::Manager` object,
/// which needs to be freed before exiting the application to prevent memory leaks. `%dip::viewer::Spin` and
/// `dip::viewer::CloseAll` will free the internal manager object. You need to call one of these two functions
/// at an appropriate time after calling `dip::viewer::Show` or `dip::viewer::ShowSimple`.
DIPVIEWER_EXPORT void Spin();

/// \brief Process user event queue.
///
/// This function allows user interaction in all slice viewer windows created. Returns immediately, and needs to be
/// called repeatedly for continuous interaction.
///
/// **Note** that `dip::viewer::Spin` or `dip::viewer::CloseAll` must still be called before exiting, to prevent
/// memory leaks.
DIPVIEWER_EXPORT void Draw();

/// \brief Close all open windows.
///
/// Closes all open windows and frees the internal window manager object.
///
/// Calling `dip::viewer::Show` or `dip::viewer::ShowSimple` creates an internal `dip::viewer::Manager` object,
/// which needs to be freed before exiting the application to prevent memory leaks. `dip::viewer::Spin` and
/// `%dip::viewer::CloseAll` will free the internal manager object. You need to call one of these two functions
/// at an appropriate time after calling `dip::viewer::Show` or `dip::viewer::ShowSimple`.
DIPVIEWER_EXPORT void CloseAll();

/// \}

}} // namespace dip::viewer

#endif // DIPVIEWER_H
