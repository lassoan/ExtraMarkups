/*==============================================================================

  Copyright (c) The Intervention Centre
  Oslo University Hospital, Oslo, Norway. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Rafael Palomar (The Intervention Centre,
  Oslo University Hospital) and was supported by The Research Council of Norway
  through the ALive project (grant nr. 311393).

==============================================================================*/

#ifndef __vtkslicerShape_LOWERrepresentation3d_h_
#define __vtkslicerShape_LOWERrepresentation3d_h_

#include "vtkSlicerShapeModuleVTKWidgetsExport.h"

// Markups VTKWidgets includes
#include "vtkSlicerMarkupsWidgetRepresentation3D.h"

// VTK includes
#include <vtkWeakPointer.h>
#include <vtkDiskSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTubeFilter.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>

//------------------------------------------------------------------------------
class vtkCutter;
class vtkPlane;

/**
 * @class   vtkSlicerShapeRepresentation3D
 * @brief   Default representation for the line widget
 *
 * This class provides the default concrete representation for the
 * vtkMRMLAbstractWidget. See vtkMRMLAbstractWidget
 * for details.
 * @sa
 * vtkSlicerMarkupsWidgetRepresentation2D vtkMRMLAbstractWidget
*/

class VTK_SLICER_SHAPE_MODULE_VTKWIDGETS_EXPORT vtkSlicerShapeRepresentation3D
: public vtkSlicerMarkupsWidgetRepresentation3D
{
public:
  static vtkSlicerShapeRepresentation3D* New();
  vtkTypeMacro(vtkSlicerShapeRepresentation3D, vtkSlicerMarkupsWidgetRepresentation3D);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr) override;

  /// Methods to make this class behave as a vtkProp.
  void GetActors(vtkPropCollection*) override;
  void ReleaseGraphicsResources(vtkWindow*) override;
  int RenderOverlay(vtkViewport* viewport) override;
  int RenderOpaqueGeometry(vtkViewport* viewport) override;
  int RenderTranslucentPolygonalGeometry(vtkViewport* viewport) override;
  vtkTypeBool HasTranslucentPolygonalGeometry() override;

protected:
  vtkSlicerShapeRepresentation3D();
  ~vtkSlicerShapeRepresentation3D() override;

  vtkSmartPointer<vtkPolyData> MiddlePoint;
  vtkSmartPointer<vtkPolyDataMapper> MiddlePointMapper;
  vtkSmartPointer<vtkActor> MiddlePointActor;
  vtkSmartPointer<vtkSphereSource> MiddlePointSource;

  void BuildMiddlePoint();
  
  vtkSmartPointer<vtkDiskSource> DiskSource;
  vtkSmartPointer<vtkDiskSource> RingSource;
  vtkSmartPointer<vtkSphereSource> SphereSource;
  
  vtkSmartPointer<vtkLineSource> RadiusSource;
  vtkSmartPointer<vtkPolyDataMapper> RadiusMapper;
  vtkSmartPointer<vtkActor> RadiusActor;
  
  vtkSmartPointer<vtkParametricSpline> Spline;
  vtkSmartPointer<vtkParametricFunctionSource> SplineFunctionSource;
  vtkSmartPointer<vtkTubeFilter> Tube;
  vtkSmartPointer<vtkTubeFilter> CappedTube;
  
  vtkSmartPointer<vtkPolyDataMapper> ShapeMapper;
  vtkSmartPointer<vtkActor> ShapeActor;
  vtkSmartPointer<vtkProperty> ShapeProperty;
  
  void UpdateDiskFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);
  void UpdateRingFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);
  void UpdateSphereFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);
  void UpdateTubeFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);

private:
  vtkSlicerShapeRepresentation3D(const vtkSlicerShapeRepresentation3D&) = delete;
  void operator=(const vtkSlicerShapeRepresentation3D&) = delete;
  
  // Ring : p3 is moved during UpdateFromMRML, block recursion.
  bool DoUpdateFromMRML = true;
};

#endif // __vtkslicerShape_LOWERrepresentation3d_h_
