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


#ifndef _CHART2_SELECTIONHELPER_HXX
#define _CHART2_SELECTIONHELPER_HXX

#include "DrawViewWrapper.hxx"
#include "ObjectIdentifier.hxx"

class SdrObject;
// header for enum SdrDragMode
#include <svx/svdtypes.hxx>
#include <com/sun/star/drawing/XShape.hpp>

//.............................................................................
namespace chart
{
//.............................................................................

class ObjectIdentifier;

//-----------------------------------------------------------------------------
/**
*/
class Selection
{
public: //methods
    bool hasSelection();

    rtl::OUString getSelectedCID();
    ::com::sun::star::uno::Reference<
                    ::com::sun::star::drawing::XShape > getSelectedAdditionalShape();
    ObjectIdentifier getSelectedOID() const;

    bool isResizeableObjectSelected();
    bool isRotateableObjectSelected( const ::com::sun::star::uno::Reference<
                        ::com::sun::star::frame::XModel >& xChartModel );
    bool isDragableObjectSelected();

    bool isAdditionalShapeSelected() const;

    //returns true if selection has changed
    bool setSelection( const ::rtl::OUString& rCID );
    bool setSelection( const ::com::sun::star::uno::Reference<
                    ::com::sun::star::drawing::XShape >& xShape );

    void clearSelection();

    //returns true if the selection has changed
    bool maybeSwitchSelectionAfterSingleClickWasEnsured();
    void resetPossibleSelectionAfterSingleClickWasEnsured();

    void remindSelectionBeforeMouseDown();
    bool isSelectionDifferentFromBeforeMouseDown();

    void adaptSelectionToNewPos( const Point& rMousePos, DrawViewWrapper* pDrawViewWrapper
                                       , bool bIsRightMouse, bool bWaitingForDoubleClick );

    void applySelection( DrawViewWrapper* pDrawViewWrapper );

private: //member
    //the selection could be given by a CID or by a shape
    //if m_aSelectedObjectCID is not empty this indicates the selection
    //the content of m_xSelectedShape is ignored in that case
    //the strings are used for autogenerated chart specific objects
    //the shape reference is used for additional shapes
    ObjectIdentifier    m_aSelectedOID; //only single object selection so far
    ObjectIdentifier    m_aSelectedOID_beforeMouseDown;
    ObjectIdentifier    m_aSelectedOID_selectOnlyIfNoDoubleClickIsFollowing;
};

class SelectionHelper : public MarkHandleProvider
{
public:
    static bool             findNamedParent( SdrObject*& pInOutObject
                                , rtl::OUString& rOutName
                                , bool bGivenObjectMayBeResult );
    static bool             findNamedParent( SdrObject*& pInOutObject
                                , ObjectIdentifier& rOutObject
                                , bool bGivenObjectMayBeResult );
    static SdrObject*       getMarkHandlesObject( SdrObject* pObj );
    static E3dScene*        getSceneToRotate( SdrObject* pObj );
    static bool             isDragableObjectHitTwice( const Point& rMPos
                                , const rtl::OUString& rNameOfSelectedObject
                                , const DrawViewWrapper& rDrawViewWrapper );

    static ::rtl::OUString getHitObjectCID(
        const Point& rMPos,
        DrawViewWrapper& rDrawViewWrapper,
        bool bGetDiagramInsteadOf_Wall=false );

    static bool isRotateableObject( const ::rtl::OUString& rCID
                    , const ::com::sun::star::uno::Reference<
                        ::com::sun::star::frame::XModel >& xChartModel );


    SelectionHelper( SdrObject* pSelectedObj );
    virtual ~SelectionHelper();


    //MarkHandleProvider:
    virtual bool getMarkHandles( SdrHdlList& rHdlList );
    virtual bool getFrameDragSingles();

    SdrObject*   getObjectToMark();//sets also internally the mark object
    //-> getMarkHandles will behave different if this method has found a Mark Object different from m_pSelectedObj

private:
    SdrObject*      m_pSelectedObj;//hit and logically selected object
    SdrObject*      m_pMarkObj;//object that is marked instead to have more pretty handles
};

//.............................................................................
} //namespace chart
//.............................................................................
#endif