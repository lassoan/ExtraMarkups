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

#ifndef __vtkslicerringrepresentation3d_h_
#define __vtkslicerringrepresentation3d_h_

#include "vtkSlicerRingModuleVTKWidgetsExport.h"

// Markups VTKWidgets includes
#include "vtkSlicerMarkupsWidgetRepresentation3D.h"

// VTK includes
#include <vtkWeakPointer.h>
#include <vtkDiskSource.h>
#include <vtkLineSource.h>

//------------------------------------------------------------------------------
class vtkCutter;
class vtkPlane;

/**
 * @class   vtkSlicerRingRepresentation3D
 * @brief   Default representation for the line widget
 *
 * This class provides the default concrete representation for the
 * vtkMRMLAbstractWidget. See vtkMRMLAbstractWidget
 * for details.
 * @sa
 * vtkSlicerMarkupsWidgetRepresentation2D vtkMRMLAbstractWidget
*/

class VTK_SLICER_RING_MODULE_VTKWIDGETS_EXPORT vtkSlicerRingRepresentation3D
: public vtkSlicerMarkupsWidgetRepresentation3D
{
public:
  static vtkSlicerRingRepresentation3D* New();
  vtkTypeMacro(vtkSlicerRingRepresentation3D, vtkSlicerMarkupsWidgetRepresentation3D);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr) override;

  /// Methods to make this class behave as a vtkProp.
  void GetActors(vtkPropCollection*) override;
  void ReleaseGraphicsResources(vtkWindow*) override;
  int RenderOverlay(vtkViewport* viewport) override;
  int RenderOpaqueGeometry(vtkViewport* viewport) override;
  int RenderTranslucentPolygonalGeometry(vtkViewport* viewport) override;
  vtkTypeBool HasTranslucentPolygonalGeometry() override;
  
  double * GetBounds() override;

protected:
  vtkSlicerRingRepresentation3D();
  ~vtkSlicerRingRepresentation3D() override;

  vtkSmartPointer<vtkPolyData> MiddlePoint;
  vtkSmartPointer<vtkPolyDataMapper> MiddlePointMapper;
  vtkSmartPointer<vtkActor> MiddlePointActor;
  vtkSmartPointer<vtkSphereSource> MiddlePointSource;
  vtkSmartPointer<vtkPlane> SlicingPlane;
  
  vtkSmartPointer<vtkDiskSource> RingSource;
  vtkSmartPointer<vtkPolyDataMapper> RingMapper;
  vtkSmartPointer<vtkActor> RingActor;
  
  vtkSmartPointer<vtkLineSource> RadiusSource;
  vtkSmartPointer<vtkPolyDataMapper> RadiusMapper;
  vtkSmartPointer<vtkActor> RadiusActor;
  
private:
  vtkSlicerRingRepresentation3D(const vtkSlicerRingRepresentation3D&) = delete;
  void operator=(const vtkSlicerRingRepresentation3D&) = delete;
  
  // p3 is moved during UpdateFromMRML, block recursion.
  bool DoUpdateFromMRML = true;
};

#endif // __vtkslicerringrepresentation3d_h_
