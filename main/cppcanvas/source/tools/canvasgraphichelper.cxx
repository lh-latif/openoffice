/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_cppcanvas.hxx"

#include <canvasgraphichelper.hxx>

#include <com/sun/star/rendering/XGraphicDevice.hpp>
#include <com/sun/star/rendering/XCanvas.hpp>
#include <com/sun/star/rendering/XPolyPolygon2D.hpp>

#include <canvas/canvastools.hxx>
#include <basegfx/tools/canvastools.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>

#include <cppcanvas/polypolygon.hxx>
#include "tools.hxx"


using namespace ::com::sun::star;

/* Implementation of CanvasGraphicHelper class */

namespace cppcanvas
{

    namespace internal
    {
        CanvasGraphicHelper::CanvasGraphicHelper( const CanvasSharedPtr& rParentCanvas ) :
            maClipPolyPolygon(),
            mpCanvas( rParentCanvas ),
            mxGraphicDevice()
        {
            OSL_ENSURE( mpCanvas.get() != NULL &&
                        mpCanvas->getUNOCanvas().is(), 
                        "CanvasGraphicHelper::CanvasGraphicHelper: no valid canvas" );

            if( mpCanvas.get() != NULL &&
                mpCanvas->getUNOCanvas().is() )
            {
                mxGraphicDevice = mpCanvas->getUNOCanvas()->getDevice();
            }

            ::canvas::tools::initRenderState( maRenderState );
        }

        void CanvasGraphicHelper::setTransformation( const ::basegfx::B2DHomMatrix& rMatrix )
        {
            ::canvas::tools::setRenderStateTransform( maRenderState, rMatrix );
        }
        
        ::basegfx::B2DHomMatrix CanvasGraphicHelper::getTransformation() const
        {
            ::basegfx::B2DHomMatrix aMatrix;
            return ::canvas::tools::getRenderStateTransform( aMatrix, 
                                                             maRenderState );
        }

        void CanvasGraphicHelper::setClip( const ::basegfx::B2DPolyPolygon& rClipPoly )
        {
            // TODO(T3): not thread-safe. B2DPolyPolygon employs copy-on-write
            maClipPolyPolygon.reset( rClipPoly );
            maRenderState.Clip.clear();
        }

        void CanvasGraphicHelper::setClip()
        {
            maClipPolyPolygon.reset();
            maRenderState.Clip.clear();
        }

        ::basegfx::B2DPolyPolygon const* CanvasGraphicHelper::getClip() const
        {
            return !maClipPolyPolygon ? NULL : &(*maClipPolyPolygon);
        }

        const rendering::RenderState& CanvasGraphicHelper::getRenderState() const
        {
            if( maClipPolyPolygon && !maRenderState.Clip.is() )
            {
                uno::Reference< rendering::XCanvas > xCanvas( mpCanvas->getUNOCanvas() );
                if( !xCanvas.is() )
                    return maRenderState;

                maRenderState.Clip = ::basegfx::unotools::xPolyPolygonFromB2DPolyPolygon(
                    xCanvas->getDevice(),
                    *maClipPolyPolygon );
            }

            return maRenderState;
        }

        void CanvasGraphicHelper::setRGBAColor( Color::IntSRGBA aColor )
        {
            maRenderState.DeviceColor = tools::intSRGBAToDoubleSequence( mxGraphicDevice, 
                                                                         aColor );
        }

        Color::IntSRGBA CanvasGraphicHelper::getRGBAColor() const
        {
            return tools::doubleSequenceToIntSRGBA( mxGraphicDevice, 
                                                    maRenderState.DeviceColor );
        }

        void CanvasGraphicHelper::setCompositeOp( CompositeOp aOp )
        {
            maRenderState.CompositeOperation = (sal_Int8)aOp;
        }

        CanvasGraphic::CompositeOp CanvasGraphicHelper::getCompositeOp() const
        {
            return static_cast<CompositeOp>(maRenderState.CompositeOperation);
        }

        CanvasSharedPtr CanvasGraphicHelper::getCanvas() const
        {
            return mpCanvas;
        }

        uno::Reference< rendering::XGraphicDevice >	CanvasGraphicHelper::getGraphicDevice() const
        {
            return mxGraphicDevice;
        }
        
    }
}
